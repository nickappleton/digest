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
#include "hash/md5.h"

static const mccl_uif32 md5_k[64] =
	{0xD76AA478u, 0xE8C7B756u, 0x242070DBu, 0xC1BDCEEEu
	,0xF57C0FAFu, 0x4787C62Au, 0xA8304613u, 0xFD469501u
	,0x698098D8u, 0x8B44F7AFu, 0xFFFF5BB1u, 0x895CD7BEu
	,0x6B901122u, 0xFD987193u, 0xA679438Eu, 0x49B40821u
	,0xF61E2562u, 0xC040B340u, 0x265E5A51u, 0xE9B6C7AAu
	,0xD62F105Du, 0x02441453u, 0xD8A1E681u, 0xE7D3FBC8u
	,0x21E1CDE6u, 0xC33707D6u, 0xF4D50D87u, 0x455A14EDu
	,0xA9E3E905u, 0xFCEFA3F8u, 0x676F02D9u, 0x8D2A4C8Au
	,0xFFFA3942u, 0x8771F681u, 0x6D9D6122u, 0xFDE5380Cu
	,0xA4BEEA44u, 0x4BDECFA9u, 0xF6BB4B60u, 0xBEBFBC70u
	,0x289B7EC6u, 0xEAA127FAu, 0xD4EF3085u, 0x04881D05u
	,0xD9D4D039u, 0xE6DB99E5u, 0x1FA27CF8u, 0xC4AC5665u
	,0xF4292244u, 0x432AFF97u, 0xAB9423A7u, 0xFC93A039u
	,0x655B59C3u, 0x8F0CCC92u, 0xFFEFF47Du, 0x85845DD1u
	,0x6FA87E4Fu, 0xFE2CE6E0u, 0xA3014314u, 0x4E0811A1u
	,0xF7537E82u, 0xBD3AF235u, 0x2AD7D2BBu, 0xEB86D391u
	};

static const unsigned r[4][8] =
	{	{7, 12, 17, 22, 7, 12, 17, 22}
	,	{5,  9, 14, 20, 5,  9, 14, 20}
	,	{4, 11, 16, 23, 4, 11, 16, 23}
	,	{6, 10, 15, 21, 6, 10, 15, 21}
	};

#define ROL(x, c) (((x) << (c)) | (((x) & 0xFFFFFFFFu) >> (32 - (c))))

static void md5_process_buffer(const unsigned char *data, mccl_uif32 *state)
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
		f = (b & c) | ((~b) & d);
		g = i;
		temp = d;
		d = c;
		c = b;
		b = b + ROL((a + f + md5_k[i] + w[g]), r[0][i&7]);
		a = temp;
	}

	for (; i < 32; i++) {
		f = (d & b) | ((~d) & c);
		g = (5*i + 1) & 0xf;
		temp = d;
		d = c;
		c = b;
		b = b + ROL((a + f + md5_k[i] + w[g]), r[1][i&7]);
		a = temp;
	}

	for (; i < 48; i++) {
		f = b ^ c ^ d;
		g = (3*i + 5) & 0xf;
		temp = d;
		d = c;
		c = b;
		b = b + ROL((a + f + md5_k[i] + w[g]), r[2][i&7]);
		a = temp;
	}

	for (; i < 64; i++) {
		f = c ^ (b | (~d));
		g = (7*i) & 0xf;
		temp = d;
		d = c;
		c = b;
		b = b + ROL((a + f + md5_k[i] + w[g]), r[3][i&7]);
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

static void md5_begin(struct hash_s *hash)
{
	hash->state->buffer_index = 0;
	hash->state->h[0] = 0x67452301u;
	hash->state->h[1] = 0xEFCDAB89u;
	hash->state->h[2] = 0x98BADCFEu;
	hash->state->h[3] = 0x10325476u;
}

static
void
md5_process
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
			md5_process_buffer(hash->state->buffer_data, hash->state->h);
			context->length = UINT64_ADD(context->length, incr);
			assert(UINT64_HIGH(context->length) || UINT64_LOW(context->length));
			hash->state->buffer_index = 0;
		}
	}
	while (size >= 64) {
		md5_process_buffer(data, hash->state->h);
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
md5_end(struct hash_s *hash, unsigned char *result)
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
		md5_process_buffer(context->buffer_data, context->h);
		context->buffer_index = 0;
	}

	while (context->buffer_index < 56)
		context->buffer_data[context->buffer_index++] = 0;

	while (context->buffer_index < 64) {
		context->buffer_data[context->buffer_index++] = (unsigned char)(UINT64_LOW(context->length) & 0xFFu);
		context->length = UINT64_SHR(context->length, 8);
	}

	md5_process_buffer(context->buffer_data, context->h);
	for (i = 0; i < 16; i++)
		result[i] = (unsigned char)((context->h[i>>2] >> 8 * ((i & 0x03u))) & 0xFFu);
}

static
void
md5_destroy(struct hash_s *hash)
{
	free(hash->state);
}

static
unsigned
md5_query_digest_size(const struct hash_s *hash)
{
	return 128;
}

int md5_create(struct hash_s *hash)
{
	hash->state = malloc(sizeof(struct hash_pvt_s));
	if (!hash->state)
		return -1;
	hash->begin = md5_begin;
	hash->process = md5_process;
	hash->end = md5_end;
	hash->destroy = md5_destroy;
	hash->query_digest_size = md5_query_digest_size;
	return 0;
}

