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

#include "tiger_internal.h"
#include "tiger_coefs.h"
#include "mccl/mccl_inline.h"
#include <string.h>

#define TIGER_PASSES (3)

#define tiger_round(a, b, c, x, mul) \
		do { \
		c = UINT64_XOR(c, x); \
		a = UINT64_SUB \
			(a \
			,UINT64_XOR \
				(UINT64_XOR \
					(tiger_sboxes[0][UINT64_LOW(c) & 0xFFu] \
					,tiger_sboxes[1][(UINT64_LOW(c) >> 16) & 0xFFu] \
					) \
				,UINT64_XOR \
					(tiger_sboxes[2][UINT64_HIGH(c) & 0xFFu] \
					,tiger_sboxes[3][(UINT64_HIGH(c) >> 16) & 0xFFu] \
					) \
				) \
			); \
		b = UINT64_ADD \
			(b \
			,UINT64_XOR \
				(UINT64_XOR \
					(tiger_sboxes[3][(UINT64_LOW(c) >> 8) & 0xFFu] \
					,tiger_sboxes[2][(UINT64_LOW(c) >> 24) & 0xFFu] \
					) \
				,UINT64_XOR \
					(tiger_sboxes[1][(UINT64_HIGH(c) >> 8) & 0xFFu] \
					,tiger_sboxes[0][(UINT64_HIGH(c) >> 24) & 0xFFu] \
					) \
				) \
			); \
		b = UINT64_MUL64x16(b, mul); } while (0)

#define tiger_pass(a, b, c, str, mul) \
		do { \
		tiger_round(a, b, c, str[0], mul); \
		tiger_round(b, c, a, str[1], mul); \
		tiger_round(c, a, b, str[2], mul); \
		tiger_round(a, b, c, str[3], mul); \
		tiger_round(b, c, a, str[4], mul); \
		tiger_round(c, a, b, str[5], mul); \
		tiger_round(a, b, c, str[6], mul); \
		tiger_round(b, c, a, str[7], mul); } while (0)

static
INLINE
void
tiger_schedule(UINT64 *str)
{
	static const UINT64 tiger_schedule_1 = UINT64_INIT(0xA5A5A5A5u, 0xA5A5A5A5u);
	static const UINT64 tiger_schedule_2 = UINT64_INIT(0x01234567u, 0x89ABCDEFu);
	str[0] = UINT64_SUB(str[0], UINT64_XOR(str[7], tiger_schedule_1));
	str[1] = UINT64_XOR(str[1], str[0]);
	str[2] = UINT64_ADD(str[2], str[1]);
	str[3] = UINT64_SUB(str[3], UINT64_XOR(str[2], UINT64_SHL(UINT64_COMP(str[1]), 19)));
	str[4] = UINT64_XOR(str[4], str[3]);
	str[5] = UINT64_ADD(str[5], str[4]);
	str[6] = UINT64_SUB(str[6], UINT64_XOR(str[5], UINT64_SHR(UINT64_COMP(str[4]), 23)));
	str[7] = UINT64_XOR(str[7], str[6]);
	str[0] = UINT64_ADD(str[0], str[7]);
	str[1] = UINT64_SUB(str[1], UINT64_XOR(str[0], UINT64_SHL(UINT64_COMP(str[7]), 19)));
	str[2] = UINT64_XOR(str[2], str[1]);
	str[3] = UINT64_ADD(str[3], str[2]);
	str[4] = UINT64_SUB(str[4], UINT64_XOR(str[3], UINT64_SHR(UINT64_COMP(str[2]), 23)));
	str[5] = UINT64_XOR(str[5], str[4]);
	str[6] = UINT64_ADD(str[6], str[5]);
	str[7] = UINT64_SUB(str[7], UINT64_XOR(str[6], tiger_schedule_2));
}

void
tiger_compress(UINT64 *aa, UINT64 *bb, UINT64 *cc, UINT64 *str)
{
	UINT64 a = *aa;
	UINT64 b = *bb;
	UINT64 c = *cc;
	unsigned i;

	tiger_pass(a, b, c, str, 5);
	tiger_schedule(str);
	tiger_pass(c, a, b, str, 7);
	tiger_schedule(str);
	tiger_pass(b, c, a, str, 9);

	for (i = 3u; i < TIGER_PASSES; i++) {
		UINT64 tmp;
		tiger_schedule(str);
		tiger_pass(a, b, c, str, 9);
		tmp = a;
		a = c;
		c = b;
		b = tmp;
	}

	*aa = UINT64_XOR(a, *aa);
	*bb = UINT64_SUB(b, *bb);
	*cc = UINT64_ADD(c, *cc);
}

