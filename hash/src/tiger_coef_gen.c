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

#include "tiger_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UINT64 tiger_sboxes[4][256];

void
gen(const unsigned char str[64], unsigned passes)
{
	static const UINT64 initial_state[3] = {
			UINT64_INIT(0x01234567u, 0x89ABCDEFu),
			UINT64_INIT(0xFEDCBA98u, 0x76543210u),
			UINT64_INIT(0xF096A5B4u, 0xC3B2E187u) };

	UINT64 state[3];
	UINT64 tempstr[8];
	unsigned i;
	unsigned cnt;
	unsigned abc = 2;

	state[0] = initial_state[0];
	state[1] = initial_state[1];
	state[2] = initial_state[2];

	for (i = 0; i < 8; i++) {
		unsigned j;
		tempstr[i] = UINT64_MAKE(0, 0);
		for(j = 0; j < 8; j++) {
			tempstr[i] = UINT64_OR(UINT64_SHL(tempstr[i], 8), UINT64_MAKE(0, str[(i*8)+(j^7)]));
		}
	}

	for (i = 0; i < 256; i++) {
		unsigned j;
		UINT64 val = UINT64_MAKE(0, i);
		val = UINT64_OR(val, UINT64_SHL(val, 32));
		val = UINT64_OR(val, UINT64_SHL(val, 16));
		val = UINT64_OR(val, UINT64_SHL(val, 8));
		for (j = 0; j < 4; j++) {
			tiger_sboxes[j][i] = val;
		}
	}

	for (cnt = 0; cnt < passes; cnt++) {
		for (i = 0; i < 256; i++) {
			unsigned sb;
			for (sb = 0; sb < 4; sb++) {
				unsigned col;
				abc++;
				if (abc == 3) {
					UINT64 cpy[8];
					memcpy(cpy, tempstr, sizeof(cpy));
					abc = 0;
					tiger_compress(state, state+1, state+2, cpy);
				}
				for (col = 0; col < 8; col++) {
					static const UINT64 MASK0 = UINT64_INIT(0, 0xFFu);
					const UINT64 mask = UINT64_SHL(MASK0, col * 8);
					const UINT64 amask = UINT64_COMP(mask);
					const unsigned o2 = UINT64_LOW(UINT64_SHR(state[abc], col * 8)) & 0xFFu;
					UINT64 a = UINT64_OR(UINT64_AND(tiger_sboxes[sb][i], amask), UINT64_AND(tiger_sboxes[sb][o2], mask));
					UINT64 b = UINT64_OR(UINT64_AND(tiger_sboxes[sb][o2], amask), UINT64_AND(tiger_sboxes[sb][i], mask));
					tiger_sboxes[sb][i] = a;
					tiger_sboxes[sb][o2] = b;
				}
			}
		}
	}

}

void
print_table()
{
	unsigned sb;
	printf("#include \"tiger_coefs.h\"\n\n");
	printf("const UINT64 tiger_sboxes[4][256] =\n{   {   ");
	for (sb = 0; sb < 4; sb++) {
		unsigned i;
		for (i = 0; i < 256; i++) {
			printf
				("UINT64_INIT(0x%08lXu, 0x%08lXu) /* %4d */"
				,(unsigned long)UINT64_HIGH(tiger_sboxes[sb][i])
				,(unsigned long)UINT64_LOW(tiger_sboxes[sb][i])
				,i
				);
			if (i & 1)
				printf("\n    ");
			if (i < 255)
				printf(",   ");
			else
				printf("}\n");
		}
		if (sb < 3)
			printf(",   {   ");
		else
			printf("};\n");
	}
	printf("\n\n\n");
}

int
main(int argc, char *argv[])
{
	gen((const unsigned char *)"Tiger - A Fast New Hash Function, by Ross Anderson and Eli Biham", 5);
	print_table();
	exit(0);
}

