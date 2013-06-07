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

#include "sha2_256.h"
#include "mccl/mccl_fastints.h"
#include "mccl/mccl_bufcvt.h"
#include <assert.h>
#include <stdlib.h>

static const mccl_uif32 sha256_224_initial[8] =
{0xC1059ED8u
,0x367CD507u
,0x3070DD17u
,0xF70E5939u
,0xFFC00B31u
,0x68581511u
,0x64F98FA7u
,0xBEFA4FA4u
};

static const mccl_uif32 sha256_256_initial[8] =
{0x6A09E667u
,0xBB67AE85u
,0x3C6EF372u
,0xA54FF53Au
,0x510E527Fu
,0x9B05688Cu
,0x1F83D9ABu
,0x5BE0CD19u
};

static const mccl_uif32 sha256_table[64] =
{0x428A2F98u, 0x71374491u, 0xB5C0FBCFu, 0xE9B5DBA5u
,0x3956C25Bu, 0x59F111F1u, 0x923F82A4u, 0xAB1C5ED5u
,0xD807AA98u, 0x12835B01u, 0x243185BEu, 0x550C7DC3u
,0x72BE5D74u, 0x80DEB1FEu, 0x9BDC06A7u, 0xC19BF174u
,0xE49B69C1u, 0xEFBE4786u, 0x0FC19DC6u, 0x240CA1CCu
,0x2DE92C6Fu, 0x4A7484AAu, 0x5CB0A9DCu, 0x76F988DAu
,0x983E5152u, 0xA831C66Du, 0xB00327C8u, 0xBF597FC7u
,0xC6E00BF3u, 0xD5A79147u, 0x06CA6351u, 0x14292967u
,0x27B70A85u, 0x2E1B2138u, 0x4D2C6DFCu, 0x53380D13u
,0x650A7354u, 0x766A0ABBu, 0x81C2C92Eu, 0x92722C85u
,0xA2BFE8A1u, 0xA81A664Bu, 0xC24B8B70u, 0xC76C51A3u
,0xD192E819u, 0xD6990624u, 0xF40E3585u, 0x106AA070u
,0x19A4C116u, 0x1E376C08u, 0x2748774Cu, 0x34B0BCB5u
,0x391C0CB3u, 0x4ED8AA4Au, 0x5B9CCA4Fu, 0x682E6FF3u
,0x748F82EEu, 0x78A5636Fu, 0x84C87814u, 0x8CC70208u
,0x90BEFFFAu, 0xA4506CEBu, 0xBEF9A3F7u, 0xC67178F2u
};

#define ROR32(x, c)  (((x) << (32 - (c))) | (((x) & 0xFFFFFFFFu) >> (c)))
#define CH(x, y, z)  (((x) & (y)) ^ ((z) & ~(x)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define BSIG0(x)     (ROR32(x, 2) ^  ROR32(x, 13) ^ ROR32(x, 22))
#define BSIG1(x)     (ROR32(x, 6) ^  ROR32(x, 11) ^ ROR32(x, 25))
#define SSIG0(x)     (ROR32(x, 7) ^  ROR32(x, 18) ^ ((x) >> 3))
#define SSIG1(x)     (ROR32(x, 17) ^ ROR32(x, 19) ^ ((x) >> 10))

static void process_block(mccl_uif32 *state, const unsigned char *words)
{
	mccl_uif32 work[64];
	mccl_uif32 a = state[0];
	mccl_uif32 b = state[1];
	mccl_uif32 c = state[2];
	mccl_uif32 d = state[3];
	mccl_uif32 e = state[4];
	mccl_uif32 f = state[5];
	mccl_uif32 g = state[6];
	mccl_uif32 h = state[7];
	unsigned i;

	bufcvt_be32_to_uif32(work, words, 16);
	for (i = 16; i < 64; i++)
		work[i] = SSIG1(work[i-2]) + work[i-7] + SSIG0(work[i-15]) + work[i-16];

	for (i = 0; i < 64; i++) {
		mccl_uif32 T1 = h + BSIG1(e) + CH(e, f, g) + sha256_table[i] + work[i];
		mccl_uif32 T2 = BSIG0(a) + MAJ(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + T1;
		d = c;
		c = b;
		b = a;
		a = T1 + T2;
	}

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
	state[5] += f;
	state[6] += g;
	state[7] += h;
}


struct hash_pvt_s {
	const mccl_uif32 *initial;
	unsigned          digest_bits;
	mccl_uif32        hash[8];
	UINT64            length;
	unsigned          buffer_index;
	unsigned char     buffer_data[64];
};

static
void
sha2_256_begin(struct hash_s *hash)
{
	memcpy(hash->state->hash, hash->state->initial, sizeof(hash->state->hash));
	hash->state->length = UINT64_MAKE(0, 0);
	hash->state->buffer_index = 0;
}

static
void
sha2_256_process(struct hash_s *hash, const unsigned char *data, size_t size)
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
			process_block(hash->state->hash, hash->state->buffer_data);
			context->length = UINT64_ADD(context->length, incr);
			assert(UINT64_HIGH(context->length) || UINT64_LOW(context->length));
			hash->state->buffer_index = 0;
		}
	}
	while (size >= 64) {
		process_block(hash->state->hash, data);
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
sha2_256_end(struct hash_s *hash, unsigned char *result)
{
	unsigned i;
	struct hash_pvt_s *context = hash->state;
	const UINT64 incr = UINT64_MAKE(0, 8*context->buffer_index);

	context->length = UINT64_ADD(context->length, incr);
	assert(UINT64_HIGH(context->length) || UINT64_LOW(context->length));

	context->buffer_data[context->buffer_index++] = 0x80;

	if (context->buffer_index > 48) {
		while (context->buffer_index < 64)
			context->buffer_data[context->buffer_index++] = 0;
		process_block(context->hash, context->buffer_data);
		context->buffer_index = 0;
	}

	while (context->buffer_index < 48)
		context->buffer_data[context->buffer_index++] = 0;

	context->buffer_index = 63;
	while (context->buffer_index >= 48) {
		context->buffer_data[context->buffer_index--] = (unsigned char)(UINT64_LOW(context->length) & 0xFFu);
		context->length = UINT64_SHR(context->length, 8);
	}

	process_block(context->hash, context->buffer_data);
	for (i = 0; i < hash->state->digest_bits / 8; i++)
		result[i] = (unsigned char)((context->hash[i/4] >> (8u * (3u - (i & 0x03u)))) & 0xFFu);
}

static
unsigned
sha2_256_query_digest_size(const struct hash_s *hash)
{
	return hash->state->digest_bits;
}

static
void
sha2_256_destroy(struct hash_s *hash)
{
	free(hash->state);
}

int sha2_256_create(struct hash_s *hash, unsigned digest_bits)
{
	hash->destroy = sha2_256_destroy;
	hash->query_digest_size = sha2_256_query_digest_size;
	hash->state = malloc(sizeof(*hash->state));
	hash->state->digest_bits = digest_bits;
	hash->begin = sha2_256_begin;
	hash->process = sha2_256_process;
	hash->end = sha2_256_end;

	if (!hash->state)
		return -1;

	if (digest_bits == 224) {
		hash->state->initial = sha256_224_initial;
	} else if (digest_bits == 256) {
		hash->state->initial = sha256_256_initial;
	} else {
		free(hash->state);
		return -2;
	}

	return 0;
}




