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
#include "mccl/mccl_bufcvt.h"
#include <assert.h>
#include <stdlib.h>

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
#define SSIG0(x)     (ROR32(x, 7) ^  ROR32(x, 18) ^ (((x) & 0xFFFFFFFFu) >> 3))
#define SSIG1(x)     (ROR32(x, 17) ^ ROR32(x, 19) ^ (((x) & 0xFFFFFFFFu) >> 10))

void sha2_256_process_block(mccl_uif32 *state, const unsigned char *words)
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


