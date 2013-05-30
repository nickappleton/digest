/* Copyright (c) 2013, Nicholas Appleton (http://www.appletonaudio.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither Nicholas Appleton nor the names of its contributors may be
 *       used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE. */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hash/sha1.h"
#include "mccl/mccl_op_uint64.h"
#include "mccl/mccl_bufcvt.h"

struct hash_pvt_s
{
	mccl_uif32    state[5];
	UINT64        length;
	unsigned      buffer_index;
	unsigned char buffer_data[64];
};

static
void
sha1_begin(struct hash_s *hash)
{
	static UINT64 initial_length = UINT64_INIT(0, 0);
	struct hash_pvt_s *context = hash->state;
	context->length       = initial_length;
	context->buffer_index = 0;
	context->state[0]     = 0x67452301u;
	context->state[1]     = 0xEFCDAB89u;
	context->state[2]     = 0x98BADCFEu;
	context->state[3]     = 0x10325476u;
	context->state[4]     = 0xC3D2E1F0u;
}

#define rol32(word, bits) (((word) << (bits)) | (((word) & 0xFFFFFFFFu) >> (32-(bits))))

static
void
process_block
	(mccl_uif32         *state
	,const unsigned char *data
	)
{
	static const mccl_uif32 K[] =
		{0x5A827999u
		,0x6ED9EBA1u
		,0x8F1BBCDCu
		,0xCA62C1D6u
		};

	unsigned         t;
	mccl_uif32      temp;
	mccl_uif32      W[80];
	mccl_uif32      A, B, C, D, E;

	bufcvt_be32_to_uif32(W, data, 16);

	for (t = 16; t < 80; t++) {
		W[t] = rol32(W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16], 1);
	}

	A = state[0];
	B = state[1];
	C = state[2];
	D = state[3];
	E = state[4];

	for (t = 0; t < 20; t++) {
		temp = rol32(A, 5) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
		E = D;
		D = C;
		C = rol32(B, 30);
		B = A;
		A = temp;
	}
	for (; t < 40; t++) {
		temp = rol32(A, 5) + (B ^ C ^ D) + E + W[t] + K[1];
		E = D;
		D = C;
		C = rol32(B, 30);
		B = A;
		A = temp;
	}
	for (; t < 60; t++) {
		temp = rol32(A, 5) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
		E = D;
		D = C;
		C = rol32(B, 30);
		B = A;
		A = temp;
	}
	for (; t < 80; t++) {
		temp = rol32(A, 5) + (B ^ C ^ D) + E + W[t] + K[3];
		E = D;
		D = C;
		C = rol32(B, 30);
		B = A;
		A = temp;
	}

	state[0] += A;
	state[1] += B;
	state[2] += C;
	state[3] += D;
	state[4] += E;
}

static
void
sha1_process
	(struct hash_s        *hash
	,const unsigned char  *data
	,size_t                size
	)
{
	static const UINT64 incr = UINT64_INIT(0, 8*64);
	struct hash_pvt_s *context = hash->state;

	if (size && hash->state->buffer_index) {
		size_t cpy = 64 - hash->state->buffer_index;
		if (cpy > size)
			cpy = size;
		memcpy
			(hash->state->buffer_data + hash->state->buffer_index
			,data
			,cpy
			);
		size -= cpy;
		data += cpy;
		hash->state->buffer_index += cpy;
		if (hash->state->buffer_index == 64) {
			process_block(hash->state->state, hash->state->buffer_data);
			context->length = UINT64_ADD(context->length, incr);
			assert(UINT64_HIGH(context->length) || UINT64_LOW(context->length));
			hash->state->buffer_index = 0;
		}
	}
	while (size >= 64) {
		process_block(hash->state->state, data);
		context->length = UINT64_ADD(context->length, incr);
		assert(UINT64_HIGH(context->length) || UINT64_LOW(context->length));
		data += 64;
		size -= 64;
	}
	if (size) {
		memcpy
			(hash->state->buffer_data
			,data
			,size
			);
		hash->state->buffer_index = size;
	}
}

static
void
sha1_end(struct hash_s *hash, unsigned char *result)
{
	unsigned i;
	struct hash_pvt_s *context = hash->state;
	const UINT64 incr = UINT64_MAKE(0, 8*context->buffer_index);

	context->length = UINT64_ADD(context->length, incr);
	assert(UINT64_HIGH(context->length) || UINT64_LOW(context->length) || !context->buffer_index);

	context->buffer_data[context->buffer_index++] = 0x80;

	if (context->buffer_index > 56) {
		while (context->buffer_index < 64)
			context->buffer_data[context->buffer_index++] = 0;
		process_block(context->state, context->buffer_data);
		context->buffer_index = 0;
	}

	while (context->buffer_index < 56)
		context->buffer_data[context->buffer_index++] = 0;

	context->buffer_index = 63;
	while (context->buffer_index >= 56) {
		context->buffer_data[context->buffer_index--] = (unsigned char)(UINT64_LOW(context->length) & 0xFFu);
		context->length = UINT64_SHR(context->length, 8);
	}

	process_block(context->state, context->buffer_data);
	for (i = 0; i < 20; i++)
		result[i] = (unsigned char)((context->state[i>>2] >> 8 * (3 - (i & 0x03u))) & 0xFFu);
}

static
unsigned
sha1_query_digest_size(const struct hash_s *hash)
{
	return 20*8;
}

static
void
sha1_destroy(struct hash_s *hash)
{
	free(hash->state);
}

int sha1_create(struct hash_s *hash)
{
	struct hash_pvt_s *context = malloc(sizeof(*context));
	if (!context)
		return -1;
	hash->state = context;
	hash->begin = sha1_begin;
	hash->end = sha1_end;
	hash->process = sha1_process;
	hash->query_digest_size = sha1_query_digest_size;
	hash->destroy = sha1_destroy;
	return 0;
}


