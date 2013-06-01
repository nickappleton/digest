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

#include "sha2_512.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "mccl/mccl_op_uint64.h"
#include "mccl/mccl_bufcvt.h"

static const UINT64 sha512_512_initial[8] =
{	UINT64_INIT(0x6A09E667u, 0xF3BCC908u), UINT64_INIT(0xBB67AE85u, 0x84CAA73Bu)
,	UINT64_INIT(0x3C6EF372u, 0xFE94F82Bu), UINT64_INIT(0xA54FF53Au, 0x5F1D36F1u)
,	UINT64_INIT(0x510E527Fu, 0xADE682D1u), UINT64_INIT(0x9B05688Cu, 0x2B3E6C1Fu)
,	UINT64_INIT(0x1F83D9ABu, 0xFB41BD6Bu), UINT64_INIT(0x5BE0CD19u, 0x137E2179u)
};

static const UINT64 sha512_384_initial[8] =
{	UINT64_INIT(0xCBBB9D5Du, 0xC1059ED8u), UINT64_INIT(0x629A292Au, 0x367CD507u)
,	UINT64_INIT(0x9159015Au, 0x3070DD17u), UINT64_INIT(0x152FECD8u, 0xF70E5939u)
,	UINT64_INIT(0x67332667u, 0xFFC00B31u), UINT64_INIT(0x8EB44A87u, 0x68581511u)
,	UINT64_INIT(0xDB0C2E0Du, 0x64F98FA7u), UINT64_INIT(0x47B5481Du, 0xBEFA4FA4u)
};

static const UINT64 sha512_table[80] =
{	UINT64_INIT(0x428A2F98u, 0xD728AE22u), UINT64_INIT(0x71374491u, 0x23EF65CDu)
,	UINT64_INIT(0xB5C0FBCFu, 0xEC4D3B2Fu), UINT64_INIT(0xE9B5DBA5u, 0x8189DBBCu)
,	UINT64_INIT(0x3956C25Bu, 0xF348B538u), UINT64_INIT(0x59F111F1u, 0xB605D019u)
,	UINT64_INIT(0x923F82A4u, 0xAF194F9Bu), UINT64_INIT(0xAB1C5ED5u, 0xDA6D8118u)
,	UINT64_INIT(0xD807AA98u, 0xA3030242u), UINT64_INIT(0x12835B01u, 0x45706FBEu)
,	UINT64_INIT(0x243185BEu, 0x4EE4B28Cu), UINT64_INIT(0x550C7DC3u, 0xD5FFB4E2u)
,	UINT64_INIT(0x72BE5D74u, 0xF27B896Fu), UINT64_INIT(0x80DEB1FEu, 0x3B1696B1u)
,	UINT64_INIT(0x9BDC06A7u, 0x25C71235u), UINT64_INIT(0xC19BF174u, 0xCF692694u)
,	UINT64_INIT(0xE49B69C1u, 0x9EF14AD2u), UINT64_INIT(0xEFBE4786u, 0x384F25E3u)
,	UINT64_INIT(0x0FC19DC6u, 0x8B8CD5B5u), UINT64_INIT(0x240CA1CCu, 0x77AC9C65u)
,	UINT64_INIT(0x2DE92C6Fu, 0x592B0275u), UINT64_INIT(0x4A7484AAu, 0x6EA6E483u)
,	UINT64_INIT(0x5CB0A9DCu, 0xBD41FBD4u), UINT64_INIT(0x76F988DAu, 0x831153B5u)
,	UINT64_INIT(0x983E5152u, 0xEE66DFABu), UINT64_INIT(0xA831C66Du, 0x2DB43210u)
,	UINT64_INIT(0xB00327C8u, 0x98FB213Fu), UINT64_INIT(0xBF597FC7u, 0xBEEF0EE4u)
,	UINT64_INIT(0xC6E00BF3u, 0x3DA88FC2u), UINT64_INIT(0xD5A79147u, 0x930AA725u)
,	UINT64_INIT(0x06CA6351u, 0xE003826Fu), UINT64_INIT(0x14292967u, 0x0A0E6E70u)
,	UINT64_INIT(0x27B70A85u, 0x46D22FFCu), UINT64_INIT(0x2E1B2138u, 0x5C26C926u)
,	UINT64_INIT(0x4D2C6DFCu, 0x5AC42AEDu), UINT64_INIT(0x53380D13u, 0x9D95B3DFu)
,	UINT64_INIT(0x650A7354u, 0x8BAF63DEu), UINT64_INIT(0x766A0ABBu, 0x3C77B2A8u)
,	UINT64_INIT(0x81C2C92Eu, 0x47EDAEE6u), UINT64_INIT(0x92722C85u, 0x1482353Bu)
,	UINT64_INIT(0xA2BFE8A1u, 0x4CF10364u), UINT64_INIT(0xA81A664Bu, 0xBC423001u)
,	UINT64_INIT(0xC24B8B70u, 0xD0F89791u), UINT64_INIT(0xC76C51A3u, 0x0654BE30u)
,	UINT64_INIT(0xD192E819u, 0xD6EF5218u), UINT64_INIT(0xD6990624u, 0x5565A910u)
,	UINT64_INIT(0xF40E3585u, 0x5771202Au), UINT64_INIT(0x106AA070u, 0x32BBD1B8u)
,	UINT64_INIT(0x19A4C116u, 0xB8D2D0C8u), UINT64_INIT(0x1E376C08u, 0x5141AB53u)
,	UINT64_INIT(0x2748774Cu, 0xDF8EEB99u), UINT64_INIT(0x34B0BCB5u, 0xE19B48A8u)
,	UINT64_INIT(0x391C0CB3u, 0xC5C95A63u), UINT64_INIT(0x4ED8AA4Au, 0xE3418ACBu)
,	UINT64_INIT(0x5B9CCA4Fu, 0x7763E373u), UINT64_INIT(0x682E6FF3u, 0xD6B2B8A3u)
,	UINT64_INIT(0x748F82EEu, 0x5DEFB2FCu), UINT64_INIT(0x78A5636Fu, 0x43172F60u)
,	UINT64_INIT(0x84C87814u, 0xA1F0AB72u), UINT64_INIT(0x8CC70208u, 0x1A6439ECu)
,	UINT64_INIT(0x90BEFFFAu, 0x23631E28u), UINT64_INIT(0xA4506CEBu, 0xDE82BDE9u)
,	UINT64_INIT(0xBEF9A3F7u, 0xB2C67915u), UINT64_INIT(0xC67178F2u, 0xE372532Bu)
,	UINT64_INIT(0xCA273ECEu, 0xEA26619Cu), UINT64_INIT(0xD186B8C7u, 0x21C0C207u)
,	UINT64_INIT(0xEADA7DD6u, 0xCDE0EB1Eu), UINT64_INIT(0xF57D4F7Fu, 0xEE6ED178u)
,	UINT64_INIT(0x06F067AAu, 0x72176FBAu), UINT64_INIT(0x0A637DC5u, 0xA2C898A6u)
,	UINT64_INIT(0x113F9804u, 0xBEF90DAEu), UINT64_INIT(0x1B710B35u, 0x131C471Bu)
,	UINT64_INIT(0x28DB77F5u, 0x23047D84u), UINT64_INIT(0x32CAAB7Bu, 0x40C72493u)
,	UINT64_INIT(0x3C9EBE0Au, 0x15C9BEBCu), UINT64_INIT(0x431D67C4u, 0x9C100D4Cu)
,	UINT64_INIT(0x4CC5D4BEu, 0xCB3E42B6u), UINT64_INIT(0x597F299Cu, 0xFC657E2Au)
,	UINT64_INIT(0x5FCB6FABu, 0x3AD6FAECu), UINT64_INIT(0x6C44198Cu, 0x4A475817u)
};

#define SHA_MIX(x, r1, r2, r3) \
	UINT64_XOR \
		(UINT64_ROR((x), r1) \
		,UINT64_XOR \
			(UINT64_ROR((x), r2) \
			,UINT64_ROR((x), r3) \
			) \
		)

#define SHA_MIX2(x, r1, r2, r3) \
	UINT64_XOR \
		(UINT64_ROR((x), r1) \
		,UINT64_XOR \
			(UINT64_ROR((x), r2) \
			,UINT64_SHR((x), r3) \
			) \
		)

#define SHA_Ch(x,y,z)        UINT64_XOR(UINT64_AND(x, y), UINT64_AND(UINT64_COMP(x), z))
#define SHA_Maj(x,y,z)       UINT64_XOR(UINT64_XOR(UINT64_AND(x, y), UINT64_AND(x, z)), UINT64_AND(y, z))

void process_block(UINT64 *state, const unsigned char *words)
{
	UINT64 work[80];
	UINT64 h[8];
	unsigned i;
	for (i = 0; i < 8; i++)
		h[i] = state[i];

	bufcvt_be64_to_UINT64(work, words, 16); i = 16;

	for (; i < 80; i++)
		work[i] =
			UINT64_ADD
				(UINT64_ADD(SHA_MIX2(work[i-15], 1, 8, 7), work[i-7])
				,UINT64_ADD(SHA_MIX2(work[i-2], 19, 61, 6), work[i-16])
				);
	for (i = 0; i < 80; i++) {
		UINT64 S0    = SHA_MIX(h[0], 28, 34, 39);
		UINT64 S1    = SHA_MIX(h[4], 14, 18, 41);
		UINT64 ch    = SHA_Ch(h[4], h[5], h[6]);
		UINT64 maj   = SHA_Maj(h[0], h[1], h[2]);
		UINT64 temp1 = UINT64_ADD(h[7], UINT64_ADD(S1, UINT64_ADD(ch, UINT64_ADD(work[i], sha512_table[i]))));
		UINT64 temp2 = UINT64_ADD(S0, maj);
		h[7] = h[6];
		h[6] = h[5];
		h[5] = h[4];
		h[4] = UINT64_ADD(h[3], temp1);
		h[3] = h[2];
		h[2] = h[1];
		h[1] = h[0];
		h[0] = UINT64_ADD(temp1, temp2);
	}
	for (i = 0; i < 8; i++)
		state[i] = UINT64_ADD(h[i], state[i]);
}

struct hash_pvt_s {
	UINT64        ivt[8];
	const UINT64 *initial;
	unsigned      digest_bits;
	UINT64        hash[8];
	UINT64        length;
	unsigned      buffer_index;
	unsigned char buffer_data[128];
};

static
void
sha2_512_begin(struct hash_s *hash)
{
	memcpy(hash->state->hash, hash->state->initial, sizeof(hash->state->hash));
	hash->state->length = UINT64_MAKE(0, 0);
	hash->state->buffer_index = 0;
}

static
void
sha2_512_process(struct hash_s *hash, const unsigned char *data, size_t size)
{
	static const UINT64 incr = UINT64_INIT(0, 8*128);
	struct hash_pvt_s *context = hash->state;

	if (size && hash->state->buffer_index) {
		size_t cpy = 128 - hash->state->buffer_index;
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
		if (hash->state->buffer_index == 128) {
			process_block(hash->state->hash, hash->state->buffer_data);
			context->length = UINT64_ADD(context->length, incr);
			assert(UINT64_HIGH(context->length) || UINT64_LOW(context->length));
			hash->state->buffer_index = 0;
		}
	}
	while (size >= 128) {
		process_block(hash->state->hash, data);
		context->length = UINT64_ADD(context->length, incr);
		assert(UINT64_HIGH(context->length) || UINT64_LOW(context->length));
		data += 128;
		size -= 128;
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
sha2_512_end(struct hash_s *hash, unsigned char *result)
{
	unsigned i;
	struct hash_pvt_s *context = hash->state;
	const UINT64 incr = UINT64_MAKE(0, 8*context->buffer_index);

	context->length = UINT64_ADD(context->length, incr);
	assert(UINT64_HIGH(context->length) || UINT64_LOW(context->length));

	context->buffer_data[context->buffer_index++] = 0x80;

	if (context->buffer_index > 112) {
		while (context->buffer_index < 128)
			context->buffer_data[context->buffer_index++] = 0;
		process_block(context->hash, context->buffer_data);
		context->buffer_index = 0;
	}

	while (context->buffer_index < 112)
		context->buffer_data[context->buffer_index++] = 0;

	context->buffer_index = 127;
	while (context->buffer_index >= 112) {
		context->buffer_data[context->buffer_index--] = (unsigned char)(UINT64_LOW(context->length) & 0xFFu);
		context->length = UINT64_SHR(context->length, 8);
	}

	process_block(context->hash, context->buffer_data);
	for (i = 0; i < hash->state->digest_bits / 8; i++)
		result[i] = (unsigned char)(UINT64_LOW(UINT64_SHR(context->hash[i/8], 8u * (7u - (i & 0x07u)))) & 0xFFu);
}

static
unsigned
sha2_512_query_digest_size(const struct hash_s *hash)
{
	return hash->state->digest_bits;
}

static
void
sha2_512_destroy(struct hash_s *hash)
{
	free(hash->state);
}

static void create_gen_string(unsigned char *p, unsigned digest_bits)
{
	static const char *prefix_str = "SHA-512/";
	unsigned i;
	for (i = 0; prefix_str[i]; i++)
		*p++ = prefix_str[i];
	i = digest_bits;
	if (i >= 100)
		*p++ = '0' + (i / 100);
	if (i >= 10)
		*p++ = '0' + ((i % 100) / 10);
	*p++ = '0' + (i % 10);
	*p++ = '\0';
}

int sha2_512_create(struct hash_s *hash, unsigned digest_bits)
{
	hash->destroy = sha2_512_destroy;
	hash->query_digest_size = sha2_512_query_digest_size;
	hash->state = malloc(sizeof(*hash->state));
	hash->state->digest_bits = digest_bits;
	hash->begin = sha2_512_begin;
	hash->process = sha2_512_process;
	hash->end = sha2_512_end;

	if (!hash->state)
		return -1;

	if (digest_bits == 512) {
		hash->state->initial = sha512_512_initial;
	} else if (digest_bits == 384) {
		hash->state->initial = sha512_384_initial;
	} else if ((digest_bits < 1) || (digest_bits > 512)) {
		free(hash->state);
		return -2;
	} else {
		static const UINT64 z = UINT64_INIT(0xA5A5A5A5u, 0xA5A5A5A5u);
		unsigned i;
		unsigned char buf[64];

		/* Temporarily configure the state as an SHA-512 implementation to
		 * hash the generation string. */
		create_gen_string(buf, digest_bits);
		for (i = 0; i < 8; i++)
			hash->state->hash[i] = UINT64_XOR(sha512_512_initial[i], z);
		hash->state->digest_bits = 512;
		hash->state->buffer_index = 0;
		hash->state->length = UINT64_MAKE(0, 0);
		sha2_512_process(hash, buf, strlen((const char *)buf));
		sha2_512_end(hash, buf);

		/* The resultant hash is the initialisation vector. */
		memcpy(hash->state->ivt, hash->state->hash, sizeof(hash->state->ivt));
		hash->state->digest_bits = digest_bits;
		hash->state->initial = hash->state->ivt;
	}

	return 0;
}



