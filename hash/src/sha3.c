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

#include "mccl/mccl_op_uint64.h"
#include "mccl/mccl_bufcvt.h"
#include "hash/sha3.h"
#include <stdlib.h>
#include <assert.h>

static void theta(UINT64 *A)
{
	unsigned x, y;
	UINT64 C[5], D[5];

	for (x = 0; x < 5; x++)
		C[x] = UINT64_MAKE(0, 0);

	for (y = 0; y < 5; y++)
		for (x = 0; x < 5; x++)
			C[x] = UINT64_XOR(C[x], A[x + 5*y]);

	D[0] = UINT64_XOR(UINT64_ROL(C[1], 1), C[4]);
	D[1] = UINT64_XOR(UINT64_ROL(C[2], 1), C[0]);
	D[2] = UINT64_XOR(UINT64_ROL(C[3], 1), C[1]);
	D[3] = UINT64_XOR(UINT64_ROL(C[4], 1), C[2]);
	D[4] = UINT64_XOR(UINT64_ROL(C[0], 1), C[3]);

	for (y = 0; y < 5; y++)
		for (x = 0; x < 5; x++)
			A[x + 5*y] = UINT64_XOR(A[x + 5*y], D[x]);
}

static void rho(UINT64 *A)
{
	static const unsigned offsets[25] =
	{	0,  1,  62, 28, 27
	,	36, 44, 6,  55, 20
	,	3,  10, 43, 25, 39
	,	41, 45, 15, 21, 8
	,	18, 2,  61, 56, 14
	};
	unsigned x;
	for (x = 0; x < 25; x++)
		A[x] = UINT64_ROL(A[x], offsets[x]);
}

static void pi(UINT64 *out, const UINT64 *in)
{
	static const unsigned offsets[25] =
	{	0,  10, 20, 5,  15
	,	16, 1,  11, 21, 6
	,	7,  17, 2,  12, 22
	,	23, 8,  18, 3,  13
	,	14, 24, 9,  19, 4
	};
	unsigned int x;
	for (x = 0; x < 25; x++)
		out[offsets[x]] = in[x];
}

static void chi(UINT64 *out, const UINT64 *in)
{
	unsigned int y;
	for (y = 0; y < 5; y++, in+=5, out+=5) {
		out[0] = UINT64_XOR(in[0], UINT64_AND(UINT64_COMP(in[1]), in[2]));
		out[1] = UINT64_XOR(in[1], UINT64_AND(UINT64_COMP(in[2]), in[3]));
		out[2] = UINT64_XOR(in[2], UINT64_AND(UINT64_COMP(in[3]), in[4]));
		out[3] = UINT64_XOR(in[3], UINT64_AND(UINT64_COMP(in[4]), in[0]));
		out[4] = UINT64_XOR(in[4], UINT64_AND(UINT64_COMP(in[0]), in[1]));
	}
}

static void iota(UINT64 *A, unsigned round)
{
	static const UINT64 k[] =
	{	UINT64_INIT(0x00000000u, 0x00000001u), UINT64_INIT(0x00000000u, 0x00008082u)
	,	UINT64_INIT(0x80000000u, 0x0000808Au), UINT64_INIT(0x80000000u, 0x80008000u)
	,	UINT64_INIT(0x00000000u, 0x0000808Bu), UINT64_INIT(0x00000000u, 0x80000001u)
	,	UINT64_INIT(0x80000000u, 0x80008081u), UINT64_INIT(0x80000000u, 0x00008009u)
	,	UINT64_INIT(0x00000000u, 0x0000008Au), UINT64_INIT(0x00000000u, 0x00000088u)
	,	UINT64_INIT(0x00000000u, 0x80008009u), UINT64_INIT(0x00000000u, 0x8000000Au)
	,	UINT64_INIT(0x00000000u, 0x8000808Bu), UINT64_INIT(0x80000000u, 0x0000008Bu)
	,	UINT64_INIT(0x80000000u, 0x00008089u), UINT64_INIT(0x80000000u, 0x00008003u)
	,	UINT64_INIT(0x80000000u, 0x00008002u), UINT64_INIT(0x80000000u, 0x00000080u)
	,	UINT64_INIT(0x00000000u, 0x0000800Au), UINT64_INIT(0x80000000u, 0x8000000Au)
	,	UINT64_INIT(0x80000000u, 0x80008081u), UINT64_INIT(0x80000000u, 0x00008080u)
	,	UINT64_INIT(0x00000000u, 0x80000001u), UINT64_INIT(0x80000000u, 0x80008008u)
	};
	A[0] = UINT64_XOR(A[0], k[round]);
}

struct hash_pvt_s {
	unsigned      buffer_index;
	unsigned      buffer_length; /* == rate / 8 */
	unsigned      digest_bits; /* was fixedOutputLength */
	unsigned int  capacity;
	unsigned char buffer_data[192];
	UINT64        state[25];
} spongeState;

static void sha3_begin(struct hash_s *hash)
{
	struct hash_pvt_s *ctx = hash->state;
	ctx->buffer_index = 0;
	memset(ctx->state, 0, sizeof(ctx->state));
	memset(ctx->buffer_data, 0, sizeof(ctx->buffer_data));
}

static void sha3_absorb(UINT64 *state, const unsigned char *data, size_t size)
{
	UINT64 buf[25];
	unsigned elements, i;
	assert((size & 0x7u) == 0);
	elements = size / 8;
	bufcvt_le64_to_UINT64(buf, data, elements);
	for (i = 0; i < elements; i++)
		state[i] = UINT64_XOR(state[i], buf[i]);
	for (i = 0; i < 24; i++) {
		theta(state);
		rho(state);
		pi(buf, state);
		chi(state, buf);
		iota(state, i);
	}
}

static void sha3_process(struct hash_s *hash, const unsigned char *data, size_t size)
{
	struct hash_pvt_s *context = hash->state;

	if (size && context->buffer_index) {
		size_t cpy = context->buffer_length - context->buffer_index;
		if (cpy > size)
			cpy = size;
		memcpy
			(context->buffer_data + context->buffer_index
			,data
			,cpy
			);
		size -= cpy;
		data += cpy;
		context->buffer_index += cpy;
		if (context->buffer_index == context->buffer_length) {
			sha3_absorb(context->state, context->buffer_data, context->buffer_length);
			context->buffer_index = 0;
		}
	}
	while (size >= context->buffer_length) {
		sha3_absorb(context->state, data, context->buffer_length);
		data += context->buffer_length;
		size -= context->buffer_length;
	}
	if (size) {
		memcpy
			(context->buffer_data
			,data
			,size
			);
		context->buffer_index = size;
	}
}

static
void
sha3_end(struct hash_s *hash, unsigned char *result)
{
	unsigned i;
	struct hash_pvt_s *context = hash->state;

	/* add ending bit which sits in LSB... */
	if (context->buffer_index+1 < context->buffer_length) {
		context->buffer_data[context->buffer_index++] = 0x01;
		while (context->buffer_index+1 < context->buffer_length)
			context->buffer_data[context->buffer_index++] = 0x00;
		context->buffer_data[context->buffer_index++] = 0x80;
	} else {
		context->buffer_data[context->buffer_index++] = 0x81;
	}

	sha3_absorb(context->state, context->buffer_data, context->buffer_length);

	for (i = 0; i < context->digest_bits / 8; i++)
		result[i] = (unsigned char)(UINT64_LOW(UINT64_SHR(context->state[i/8], 8u * (i & 0x07u))) & 0xFFu);
}

static
unsigned
sha3_query_digest_size(const struct hash_s *hash)
{
	return hash->state->digest_bits;
}

static
void
sha3_destroy(struct hash_s *hash)
{
	free(hash->state);
}

int sha3_create(struct hash_s *hash, unsigned digest_bits)
{
	struct hash_pvt_s *ctx;

	switch (digest_bits) {
	case 224:
	case 256:
	case 384:
	case 512:
		break;
	default:
		return -1;
	}

	ctx = malloc(sizeof(*ctx));
	if (!ctx)
		return -1;

	ctx->capacity          = 2 * digest_bits;
	ctx->buffer_length     = (1600u - ctx->capacity) / 8u;
	ctx->digest_bits       = digest_bits;
	ctx->buffer_index      = 0;
	memset(ctx->state, 0, sizeof(ctx->state));
	memset(ctx->buffer_data, 0, sizeof(ctx->buffer_data));

	hash->state = ctx;
	hash->begin = sha3_begin;
	hash->process = sha3_process;
	hash->end = sha3_end;
	hash->query_digest_size = sha3_query_digest_size;
	hash->destroy = sha3_destroy;

	return 0;
}


