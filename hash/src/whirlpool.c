/* Copyright (c) 2014, Nicholas Appleton (http://www.appletonaudio.com)
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
 * ARE DISCLAIMED. IN NO EVENT SHALL NICHOLAS APPLETON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE. */

#include "hash/whirlpool.h"
#include "whirlpool_coefs.h"
#include "mccl/mccl_bufcvt.h"
#include <assert.h>
#include <stdlib.h>

static void whirlpool_process_buffer(const unsigned char *buffer, UINT64 *hash)
{
	UINT64 block[8];
	UINT64 k[8];
	UINT64 state[8];
	unsigned i;
	unsigned r;

	bufcvt_be64_to_UINT64(block, buffer, 8);

	for (i = 0; i < 8; i++) {
		k[i] = hash[i];
		state[i] = UINT64_XOR(block[i], hash[i]);
	}

	for (r = 0; r < WHIRLPOOL_NB_ROUNDS; r++) {
		UINT64 l[8];

		for (i = 0; i < 8; i++) {
			unsigned t;
			unsigned s;
			l[i] = UINT64_MAKE(0, 0);
			for (t = 0, s = 56; t < 8; t++, s -= 8)
				l[i] =
					UINT64_XOR
						(l[i]
						,whirlpool_sboxes[t][UINT64_LOW(UINT64_SHR(k[(i - t) & 7], s)) & 0xFFu]
						);
		}

		for (i = 0; i < 8; i++)
			k[i] = l[i];

		k[0] = UINT64_XOR(k[0], whirlpool_rounds[r]);

		for (i = 0; i < 8; i++) {
			unsigned t;
			unsigned s;
			l[i] = k[i];
			for (t = 0, s = 56; t < 8; t++, s -= 8)
				l[i] =
					UINT64_XOR
						(l[i]
						,whirlpool_sboxes[t][UINT64_LOW(UINT64_SHR(state[(i - t) & 7], s)) & 0xFFu]
						);
		}

		for (i = 0; i < 8; i++)
			state[i] = l[i];
	}

	for (i = 0; i < 8; i++)
		hash[i] = UINT64_XOR(hash[i], UINT64_XOR(state[i], block[i]));
}


struct hash_pvt_s {
	UINT64        h[8];
	UINT64        length;
	unsigned      buffer_index;
	unsigned char buffer_data[64];
};

static void whirlpool_begin(struct hash_s *hash)
{
	unsigned i;
	hash->state->buffer_index = 0;
	hash->state->length = UINT64_MAKE(0, 0);
	for (i = 0; i < 8; i++)
		hash->state->h[i] = UINT64_MAKE(0, 0);
}

static
void
whirlpool_process
	(struct hash_s        *hash
	,const unsigned char  *data
	,size_t                size
	)
{
	static const UINT64 incr = UINT64_INIT(0, 8*8*8);
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
			whirlpool_process_buffer(hash->state->buffer_data, hash->state->h);
			context->length = UINT64_ADD(context->length, incr);
			assert(UINT64_HIGH(context->length) || UINT64_LOW(context->length));
			hash->state->buffer_index = 0;
		}
	}
	while (size >= 64) {
		whirlpool_process_buffer(data, hash->state->h);
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
whirlpool_end(struct hash_s *hash, unsigned char *result)
{
	unsigned i;
	struct hash_pvt_s *context = hash->state;
	const UINT64 incr = UINT64_MAKE(0, 8*context->buffer_index);

	context->length = UINT64_ADD(context->length, incr);
	assert(UINT64_HIGH(context->length) || UINT64_LOW(context->length) || !context->buffer_index);

	context->buffer_data[context->buffer_index++] = 0x80;

	if (context->buffer_index > 32) {
		while (context->buffer_index < 64)
			context->buffer_data[context->buffer_index++] = 0;
		whirlpool_process_buffer(context->buffer_data, context->h);
		context->buffer_index = 0;
	}

	while (context->buffer_index < 32)
		context->buffer_data[context->buffer_index++] = 0;

	context->buffer_index = 64;
	while (context->buffer_index > 32) {
		context->buffer_data[--context->buffer_index] = (unsigned char)(UINT64_LOW(context->length) & 0xFFu);
		context->length = UINT64_SHR(context->length, 8);
	}

	whirlpool_process_buffer(context->buffer_data, context->h);

	for (i = 0; i < 64; i++)
		result[i] = UINT64_LOW(UINT64_SHR(context->h[i / 8], 56 - (i % 8) * 8)) & 0xFFu;
}

static
void
whirlpool_destroy(struct hash_s *hash)
{
	free(hash->state);
}

static
unsigned
whirlpool_query_digest_size(const struct hash_s *hash)
{
	return 512;
}

int whirlpool_create(struct hash_s *hash)
{
	hash->state = malloc(sizeof(struct hash_pvt_s));
	if (!hash->state)
		return -1;
	hash->begin = whirlpool_begin;
	hash->process = whirlpool_process;
	hash->end = whirlpool_end;
	hash->destroy = whirlpool_destroy;
	hash->query_digest_size = whirlpool_query_digest_size;
	return 0;
}


