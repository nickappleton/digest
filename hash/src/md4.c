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
 * ARE DISCLAIMED. IN NO EVENT SHALL NICHOLAS APPLETON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE. */

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "mccl/mccl_op_uint64.h"
#include "mccl/mccl_bufcvt.h"
#include "hash/md4.h"

#define ROL(x, c) (((x) << (c)) | (((x) & 0xFFFFFFFFu) >> (32 - (c))))

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))

static const mccl_uif32 md4_rc[4][4] =
	{	{3, 7, 11, 19}
	,	{3, 5,  9, 13}
	,	{3, 9, 11, 15}
	};

static void md4_process_buffer(const unsigned char *data, mccl_uif32 *state)
{
	mccl_uif32 w[16];
	mccl_uif32 a, b, c, d, f, g, temp;
	unsigned i;

	bufcvt_le32_to_uif32(w, data, 16);

	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];

	for (i = 0; i < 16; i++) {
		f = F(b, c, d);
		g = i;
		temp = d;
		d = c;
		c = b;
		b = ROL((a + f + 0x00000000u + w[g]), md4_rc[0][i&3]);
		a = temp;
	}

	for (; i < 32; i++) {
		f = G(b, c, d);
		g = ((i & 0x3) << 2) | ((i & 0xC) >> 2);
		temp = d;
		d = c;
		c = b;
		b = ROL((a + f + 0x5A827999u + w[g]), md4_rc[1][i&3]);
		a = temp;
	}

	for (; i < 48; i++) {
		f = H(b, c, d);
		g = ((i & 0x5) << 1) | ((i & 0xA) >> 1);
		g = ((g & 0x3) << 2) | ((g & 0xC) >> 2);
		temp = d;
		d = c;
		c = b;
		b = ROL((a + f + 0x6ED9EBA1u + w[g]), md4_rc[2][i&3]);
		a = temp;
	}

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
}

struct hash_pvt_s {
	mccl_uif32    h[4];
	UINT64        length;
	unsigned      buffer_index;
	unsigned char buffer_data[64];
};

static void md4_begin(struct hash_s *hash)
{
	hash->state->buffer_index = 0;
	hash->state->h[0] = 0x67452301u;
	hash->state->h[1] = 0xEFCDAB89u;
	hash->state->h[2] = 0x98BADCFEu;
	hash->state->h[3] = 0x10325476u;
}

static
void
md4_process
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
			md4_process_buffer(hash->state->buffer_data, hash->state->h);
			context->length = UINT64_ADD(context->length, incr);
			assert(UINT64_HIGH(context->length) || UINT64_LOW(context->length));
			hash->state->buffer_index = 0;
		}
	}
	while (size >= 64) {
		md4_process_buffer(data, hash->state->h);
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
md4_end(struct hash_s *hash, unsigned char *result)
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
		md4_process_buffer(context->buffer_data, context->h);
		context->buffer_index = 0;
	}

	while (context->buffer_index < 56)
		context->buffer_data[context->buffer_index++] = 0;

	while (context->buffer_index < 64) {
		context->buffer_data[context->buffer_index++] = (unsigned char)(UINT64_LOW(context->length) & 0xFFu);
		context->length = UINT64_SHR(context->length, 8);
	}

	md4_process_buffer(context->buffer_data, context->h);
	for (i = 0; i < 16; i++)
		result[i] = (unsigned char)((context->h[i>>2] >> 8 * ((i & 0x03u))) & 0xFFu);
}

static
void
md4_destroy(struct hash_s *hash)
{
	free(hash->state);
}

static
unsigned
md4_query_digest_size(const struct hash_s *hash)
{
	return 128;
}

int md4_create(struct hash_s *hash)
{
	hash->state = malloc(sizeof(struct hash_pvt_s));
	if (!hash->state)
		return -1;
	hash->begin = md4_begin;
	hash->process = md4_process;
	hash->end = md4_end;
	hash->destroy = md4_destroy;
	hash->query_digest_size = md4_query_digest_size;
	return 0;
}

