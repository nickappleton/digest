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

#include <stdlib.h>
#include <string.h>
#include "mccl/mccl_op_uint64.h"
#include "mccl/mccl_bufcvt.h"
#include "hash/hash.h"
#include "hash/tiger.h"
#include "tiger_coefs.h"
#include "tiger_internal.h"

struct hash_pvt_s {
	UINT64          hash[3];
	UINT64          length;
	unsigned char   work[64];
	unsigned        bufsz;
};

static
void
tiger_begin(struct hash_s *hash)
{
	static const UINT64 initial_hash[3] = {
			UINT64_INIT(0x01234567u, 0x89ABCDEFu),
			UINT64_INIT(0xFEDCBA98u, 0x76543210u),
			UINT64_INIT(0xF096A5B4u, 0xC3B2E187u) };
	hash->state->hash[0] = initial_hash[0];
	hash->state->hash[1] = initial_hash[1];
	hash->state->hash[2] = initial_hash[2];
	hash->state->bufsz   = 0;
	hash->state->length  = UINT64_MAKE(0, 0);
}

static
void
tiger_run_work
	(struct hash_pvt_s   *pvt
	,const unsigned char *data
	)
{
	UINT64 tmp[8];
	bufcvt_le64_to_UINT64(tmp, data, 8);
	tiger_compress(pvt->hash, pvt->hash+1, pvt->hash+2, tmp);
}

static
void
tiger_process(struct hash_s *tree, const unsigned char *data, size_t size)
{
	tree->state->length = UINT64_ADD(tree->state->length, UINT64_MAKE(0, size));

	if (size && tree->state->bufsz) {
		size_t cpy = 64 - tree->state->bufsz;
		if (cpy > size)
			cpy = size;
		memcpy
			(tree->state->work + tree->state->bufsz
			,data
			,cpy
			);
		size -= cpy;
		data += cpy;
		tree->state->bufsz += cpy;
		if (tree->state->bufsz == 64) {
			tiger_run_work(tree->state, tree->state->work);
			tree->state->bufsz = 0;
		}
	}
	while (size >= 64) {
		tiger_run_work(tree->state, data);
		data += 64;
		size -= 64;
	}
	if (size) {
		memcpy
			(tree->state->work
			,data
			,size
			);
		tree->state->bufsz = size;
	}
}

static
void
tiger_end(struct hash_s *hash, unsigned char *raw_data)
{
	unsigned i;
	hash->state->work[hash->state->bufsz++] = 1;
	for (; hash->state->bufsz & 0x7; hash->state->bufsz++)
		hash->state->work[hash->state->bufsz] = 0;

	if (hash->state->bufsz > 56) {
		for (; hash->state->bufsz < 64; hash->state->bufsz++)
			hash->state->work[hash->state->bufsz] = 0;
		tiger_run_work(hash->state, hash->state->work);
		hash->state->bufsz = 0;
	}

	for (; hash->state->bufsz < 56; hash->state->bufsz++)
		hash->state->work[hash->state->bufsz] = 0;

	hash->state->length = UINT64_SHL(hash->state->length, 3);
	for (; hash->state->bufsz < 64; hash->state->bufsz++) {
		hash->state->work[hash->state->bufsz] = UINT64_LOW(hash->state->length) & 0xFFu;
		hash->state->length = UINT64_SHR(hash->state->length, 8);
	}
	tiger_run_work(hash->state, hash->state->work);

	for (i = 0; i < 3; i++) {
		unsigned j;
		for (j = 0; j < 8; j++) {
			unsigned u8                     = UINT64_LOW(hash->state->hash[i]) & 0xFFu;
			raw_data[i*8+j]      = u8;
			hash->state->hash[i]            = UINT64_SHR(hash->state->hash[i], 8);
		}
	}
}

static
void
tiger_destroy(struct hash_s *tiger)
{
	free(tiger->state);
}

static
unsigned
tiger_query_digest_size(const struct hash_s *tiger)
{
	return 24*8;
}

int
tiger_create(struct hash_s *tiger)
{
	tiger->begin = tiger_begin;
	tiger->end = tiger_end;
	tiger->process = tiger_process;
	tiger->destroy = tiger_destroy;
	tiger->query_digest_size = tiger_query_digest_size;
	tiger->state = malloc(sizeof(struct hash_pvt_s));
	if (!tiger->state)
		return 1;
	memset(tiger->state, 0, sizeof(struct hash_pvt_s));
	return 0;
}



