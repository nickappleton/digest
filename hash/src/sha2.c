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

/* This work is based on:
 * http://csrc.nist.gov/publications/fips/fips180-4/fips-180-4.pdf */

#include "hash/sha2.h"
#include "mccl/mccl_op_uint64.h"
#include "mccl/mccl_bufcvt.h"
#include "sha2_512.h"
#include "sha2_256.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static const UINT64 sha512_224_initial[8] =
{	UINT64_INIT(0x8C3D37C8u, 0x19544DA2u), UINT64_INIT(0x73E19966u, 0x89DCD4D6u)
,	UINT64_INIT(0x1DFAB7AEu, 0x32FF9C82u), UINT64_INIT(0x679DD514u, 0x582F9FCFu)
,	UINT64_INIT(0x0F6D2B69u, 0x7BD44DA8u), UINT64_INIT(0x77E36F73u, 0x04C48942u)
,	UINT64_INIT(0x3F9D85A8u, 0x6A1D36C8u), UINT64_INIT(0x1112E6ADu, 0x91D692A1u)
};

static const UINT64 sha512_256_initial[8] =
{	UINT64_INIT(0x22312194u, 0xFC2BF72Cu), UINT64_INIT(0x9F555FA3u, 0xC84C64C2u)
,	UINT64_INIT(0x2393B86Bu, 0x6F53B151u), UINT64_INIT(0x96387719u, 0x5940EABDu)
,	UINT64_INIT(0x96283EE2u, 0xA88EFFE3u), UINT64_INIT(0xBE5E1E25u, 0x53863992u)
,	UINT64_INIT(0x2B0199FCu, 0x2C85B8AAu), UINT64_INIT(0x0EB72DDCu, 0x81C52CA2u)
};

static const UINT64 sha512_384_initial[8] =
{	UINT64_INIT(0xCBBB9D5Du, 0xC1059ED8u), UINT64_INIT(0x629A292Au, 0x367CD507u)
,	UINT64_INIT(0x9159015Au, 0x3070DD17u), UINT64_INIT(0x152FECD8u, 0xF70E5939u)
,	UINT64_INIT(0x67332667u, 0xFFC00B31u), UINT64_INIT(0x8EB44A87u, 0x68581511u)
,	UINT64_INIT(0xDB0C2E0Du, 0x64F98FA7u), UINT64_INIT(0x47B5481Du, 0xBEFA4FA4u)
};

static const UINT64 sha512_512_initial[8] =
{	UINT64_INIT(0x6A09E667u, 0xF3BCC908u), UINT64_INIT(0xBB67AE85u, 0x84CAA73Bu)
,	UINT64_INIT(0x3C6EF372u, 0xFE94F82Bu), UINT64_INIT(0xA54FF53Au, 0x5F1D36F1u)
,	UINT64_INIT(0x510E527Fu, 0xADE682D1u), UINT64_INIT(0x9B05688Cu, 0x2B3E6C1Fu)
,	UINT64_INIT(0x1F83D9ABu, 0xFB41BD6Bu), UINT64_INIT(0x5BE0CD19u, 0x137E2179u)
};

static const mccl_uif32 sha256_224_initial[8] =
{	0xC1059ED8u, 0x367CD507u, 0x3070DD17u, 0xF70E5939u
,	0xFFC00B31u, 0x68581511u, 0x64F98FA7u, 0xBEFA4FA4u
};

static const mccl_uif32 sha256_256_initial[8] =
{	0x6A09E667u, 0xBB67AE85u, 0x3C6EF372u, 0xA54FF53Au
,	0x510E527Fu, 0x9B05688Cu, 0x1F83D9ABu, 0x5BE0CD19u
};

struct hash_pvt_s {
	unsigned      buffer_length; /* 128 for sha2-512, 64 for sha2-256 */
	unsigned      buffer_index;
	unsigned char buffer_data[128];
	unsigned      digest_bits;
	UINT64        length;

	union {
		const UINT64     *h512;
		const mccl_uif32 *h256;
	} initial;

	union {
		UINT64            h512[8];
		mccl_uif32        h256[8];
	} hash;

	/* Storage for the arbitrary bit length initial vector for 512 */
	UINT64        ivt[8];
};

static
void
sha2_begin(struct hash_s *hash)
{
	struct hash_pvt_s *ctx = hash->state;
	if (ctx->buffer_length == 128)
		memcpy(ctx->hash.h512, ctx->initial.h512, sizeof(ctx->hash.h512));
	else
		memcpy(ctx->hash.h256, ctx->initial.h256, sizeof(ctx->hash.h256));
	ctx->length = UINT64_MAKE(0, 0);
	ctx->buffer_index = 0;
}

static
void
sha2_process(struct hash_s *hash, const unsigned char *data, size_t size)
{
	struct hash_pvt_s *context = hash->state;
	const UINT64 incr = UINT64_MAKE(0, 8 * context->buffer_length);

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
			if (context->buffer_length == 128)
				sha2_512_process_block(context->hash.h512, context->buffer_data);
			else
				sha2_256_process_block(context->hash.h256, context->buffer_data);
			context->length = UINT64_ADD(context->length, incr);
			assert(UINT64_HIGH(context->length) || UINT64_LOW(context->length));
			context->buffer_index = 0;
		}
	}
	while (size >= context->buffer_length) {
		if (context->buffer_length == 128)
			sha2_512_process_block(context->hash.h512, data);
		else
			sha2_256_process_block(context->hash.h256, data);
		context->length = UINT64_ADD(context->length, incr);
		assert(UINT64_HIGH(context->length) || UINT64_LOW(context->length));
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
sha2_end(struct hash_s *hash, unsigned char *result)
{
	unsigned i;
	struct hash_pvt_s *context = hash->state;
	const UINT64 incr = UINT64_MAKE(0, 8*context->buffer_index);

	context->length = UINT64_ADD(context->length, incr);
	assert(UINT64_HIGH(context->length) || UINT64_LOW(context->length));

	context->buffer_data[context->buffer_index++] = 0x80;

	if (context->buffer_index > context->buffer_length - 16) {
		while (context->buffer_index < context->buffer_length)
			context->buffer_data[context->buffer_index++] = 0;
		if (context->buffer_length == 128)
			sha2_512_process_block(context->hash.h512, context->buffer_data);
		else
			sha2_256_process_block(context->hash.h256, context->buffer_data);
		context->buffer_index = 0;
	}

	while (context->buffer_index < context->buffer_length - 16)
		context->buffer_data[context->buffer_index++] = 0;

	context->buffer_index = context->buffer_length - 1;
	while (context->buffer_index >= context->buffer_length - 16) {
		context->buffer_data[context->buffer_index--] = (unsigned char)(UINT64_LOW(context->length) & 0xFFu);
		context->length = UINT64_SHR(context->length, 8);
	}

	if (context->buffer_length == 128) {
		sha2_512_process_block(context->hash.h512, context->buffer_data);
		for (i = 0; i < context->digest_bits / 8; i++)
			result[i] = (unsigned char)(UINT64_LOW(UINT64_SHR(context->hash.h512[i/8], 8u * (7u - (i & 0x07u)))) & 0xFFu);
	} else {
		sha2_256_process_block(context->hash.h256, context->buffer_data);
		for (i = 0; i < context->digest_bits / 8; i++)
			result[i] = (unsigned char)((context->hash.h256[i/4] >> 8u * (3u - (i & 0x03u))) & 0xFFu);
	}
}

static
unsigned
sha2_query_digest_size(const struct hash_s *hash)
{
	return hash->state->digest_bits;
}

static
void
sha2_destroy(struct hash_s *hash)
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

int sha2_create(struct hash_s *hash, unsigned digest_bits, int force_512)
{
	struct hash_pvt_s *ctx = malloc(sizeof(struct hash_pvt_s));
	if (ctx == NULL)
		return -1;
	if ((digest_bits < 1) || (digest_bits > 512))
		return -2;

	hash->state = ctx;
	hash->destroy = sha2_destroy;
	hash->query_digest_size = sha2_query_digest_size;
	hash->begin = sha2_begin;
	hash->process = sha2_process;
	hash->end = sha2_end;

	ctx->digest_bits = digest_bits;
	ctx->buffer_length = 64;

	if ((digest_bits == 256) && (!force_512))
		ctx->initial.h256 = sha256_256_initial;
	else if ((digest_bits == 224) && (!force_512))
		ctx->initial.h256 = sha256_224_initial;
	else {
		ctx->buffer_length = 128;
		switch (digest_bits) {
		case 512:
			ctx->initial.h512 = sha512_512_initial;
			break;
		case 384:
			ctx->initial.h512 = sha512_384_initial;
			break;
		case 256:
			ctx->initial.h512 = sha512_256_initial;
			break;
		case 224:
			ctx->initial.h512 = sha512_224_initial;
			break;
		default:
			{
				static const UINT64 z = UINT64_INIT(0xA5A5A5A5u, 0xA5A5A5A5u);
				unsigned i;
				unsigned char buf[64];

				create_gen_string(buf, digest_bits);
				for (i = 0; i < 8; i++)
					hash->state->hash.h512[i] = UINT64_XOR(sha512_512_initial[i], z);
				ctx->digest_bits = 512;
				ctx->buffer_index = 0;
				ctx->length = UINT64_MAKE(0, 0);
				sha2_process(hash, buf, strlen((const char *)buf));
				sha2_end(hash, buf);

				/* The resultant hash is the initialisation vector. */
				memcpy(ctx->ivt, ctx->hash.h512, sizeof(ctx->ivt));
				ctx->digest_bits = digest_bits;
				ctx->initial.h512 = ctx->ivt;
			}
			break;
		}
	}

	return 0;
}



