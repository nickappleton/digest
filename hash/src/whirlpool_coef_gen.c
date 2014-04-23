/* Copyright (c) 2014, Nicholas Appleton (http://www.appletonaudio.com)
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

#include "whirlpool_coefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const unsigned sbox_data[] =
	{   0x18u, 0x23u, 0xC6u, 0xE8u, 0x87u, 0xB8u, 0x01u, 0x4Fu
	,   0x36u, 0xA6u, 0xD2u, 0xF5u, 0x79u, 0x6Fu, 0x91u, 0x52u
	,   0x60u, 0xBCu, 0x9Bu, 0x8Eu, 0xA3u, 0x0Cu, 0x7Bu, 0x35u
	,   0x1Du, 0xE0u, 0xD7u, 0xC2u, 0x2Eu, 0x4Bu, 0xFEu, 0x57u
	,   0x15u, 0x77u, 0x37u, 0xE5u, 0x9Fu, 0xF0u, 0x4Au, 0xDAu
	,   0x58u, 0xC9u, 0x29u, 0x0Au, 0xB1u, 0xA0u, 0x6Bu, 0x85u
	,   0xBDu, 0x5Du, 0x10u, 0xF4u, 0xCBu, 0x3Eu, 0x05u, 0x67u
	,   0xE4u, 0x27u, 0x41u, 0x8Bu, 0xA7u, 0x7Du, 0x95u, 0xD8u
	,   0xFBu, 0xEEu, 0x7Cu, 0x66u, 0xDDu, 0x17u, 0x47u, 0x9Eu
	,   0xCAu, 0x2Du, 0xBFu, 0x07u, 0xADu, 0x5Au, 0x83u, 0x33u
	,   0x63u, 0x02u, 0xAAu, 0x71u, 0xC8u, 0x19u, 0x49u, 0xD9u
	,   0xF2u, 0xE3u, 0x5Bu, 0x88u, 0x9Au, 0x26u, 0x32u, 0xB0u
	,   0xE9u, 0x0Fu, 0xD5u, 0x80u, 0xBEu, 0xCDu, 0x34u, 0x48u
	,   0xFFu, 0x7Au, 0x90u, 0x5Fu, 0x20u, 0x68u, 0x1Au, 0xAEu
	,   0xB4u, 0x54u, 0x93u, 0x22u, 0x64u, 0xF1u, 0x73u, 0x12u
	,   0x40u, 0x08u, 0xC3u, 0xECu, 0xDBu, 0xA1u, 0x8Du, 0x3Du
	,   0x97u, 0x00u, 0xCFu, 0x2Bu, 0x76u, 0x82u, 0xD6u, 0x1Bu
	,   0xB5u, 0xAFu, 0x6Au, 0x50u, 0x45u, 0xF3u, 0x30u, 0xEFu
	,   0x3Fu, 0x55u, 0xA2u, 0xEAu, 0x65u, 0xBAu, 0x2Fu, 0xC0u
	,   0xDEu, 0x1Cu, 0xFDu, 0x4Du, 0x92u, 0x75u, 0x06u, 0x8Au
	,   0xB2u, 0xE6u, 0x0Eu, 0x1Fu, 0x62u, 0xD4u, 0xA8u, 0x96u
	,   0xF9u, 0xC5u, 0x25u, 0x59u, 0x84u, 0x72u, 0x39u, 0x4Cu
	,   0x5Eu, 0x78u, 0x38u, 0x8Cu, 0xD1u, 0xA5u, 0xE2u, 0x61u
	,   0xB3u, 0x21u, 0x9Cu, 0x1Eu, 0x43u, 0xC7u, 0xFCu, 0x04u
	,   0x51u, 0x99u, 0x6Du, 0x0Du, 0xFAu, 0xDFu, 0x7Eu, 0x24u
	,   0x3Bu, 0xABu, 0xCEu, 0x11u, 0x8Fu, 0x4Eu, 0xB7u, 0xEBu
	,   0x3Cu, 0x81u, 0x94u, 0xF7u, 0xB9u, 0x13u, 0x2Cu, 0xD3u
	,   0xE7u, 0x6Eu, 0xC4u, 0x03u, 0x56u, 0x44u, 0x7Fu, 0xA9u
	,   0x2Au, 0xBBu, 0xC1u, 0x53u, 0xDCu, 0x0Bu, 0x9Du, 0x6Cu
	,   0x31u, 0x74u, 0xF6u, 0x46u, 0xACu, 0x89u, 0x14u, 0xE1u
	,   0x16u, 0x3Au, 0x69u, 0x09u, 0x70u, 0xB6u, 0xD0u, 0xEDu
	,   0xCCu, 0x42u, 0x98u, 0xA4u, 0x28u, 0x5Cu, 0xF8u, 0x86u
	};


UINT64 sbox[8][256];
UINT64 rc[WHIRLPOOL_NB_ROUNDS];

void
print_tables()
{
	unsigned sb;
	printf("#include \"whirlpool_coefs.h\"\n\n");
	printf("const UINT64 whirlpool_sboxes[8][256] =\n{   {   ");
	for (sb = 0; sb < 8; sb++) {
		unsigned i;
		for (i = 0; i < 256; i++) {
			printf
				("UINT64_INIT(0x%08lXu, 0x%08lXu) /* %4d */"
				,(unsigned long)UINT64_HIGH(sbox[sb][i])
				,(unsigned long)UINT64_LOW(sbox[sb][i])
				,i
				);
			if (i & 1)
				printf("\n    ");
			if (i < 255)
				printf(",   ");
			else
				printf("}\n");
		}
		if (sb < 7)
			printf(",   {   ");
		else
			printf("};\n");
	}

	printf("\n");
	printf("const UINT64 whirlpool_rounds[WHIRLPOOL_NB_ROUNDS] =\n{   ");
	for (sb = 0; sb < WHIRLPOOL_NB_ROUNDS; sb++) {
		printf
			("UINT64_INIT(0x%08lXu, 0x%08lXu) /* %4d */"
			,(unsigned long)UINT64_HIGH(rc[sb])
			,(unsigned long)UINT64_LOW(rc[sb])
			,sb
			);
		if (sb & 1)
			printf("\n");
		if ((sb+1) < WHIRLPOOL_NB_ROUNDS)
			printf(",   ");
		else
			printf("};\n");
	}

	printf("\n\n\n");
}

#define WF(x) (((x) & 0x80u) ? (((x) << 1) ^ 0x11Du) : ((x) << 1))

int main(int argc, char *argv[])
{
	unsigned i;

	/* Create sboxes */
	for (i = 0; i < 256; i++) {
		unsigned j;

		mccl_uif32 v1 = sbox_data[i];
		mccl_uif32 v2 = WF(v1);
		mccl_uif32 v4 = WF(v2);
		mccl_uif32 v5 = v4 ^ v1;
		mccl_uif32 v8 = WF(v4);
		mccl_uif32 v9 = v8 ^ v1;

		sbox[0][i] = UINT64_MAKE
			((v1 << 24) | (v1 << 16) | (v4 << 8) | v1
			,(v8 << 24) | (v5 << 16) | (v2 << 8) | v9
			);

		for (j = 1; j < 8; j++)
			sbox[j][i] = UINT64_ROR(sbox[0][i], j * 8);
	}

	/* Create round constants */
	for (i = 0; i < WHIRLPOOL_NB_ROUNDS; i++)
		rc[i] =
			UINT64_MAKE
				(   (   (UINT64_HIGH(sbox[0][8*i+0]) & 0xFF000000u)
				    |   (UINT64_HIGH(sbox[1][8*i+1]) & 0x00FF0000u)
				    |   (UINT64_HIGH(sbox[2][8*i+2]) & 0x0000FF00u)
				    |   (UINT64_HIGH(sbox[3][8*i+3]) & 0x000000FFu)
				    )
				,   (   (UINT64_LOW (sbox[4][8*i+4]) & 0xFF000000u)
				    |   (UINT64_LOW (sbox[5][8*i+5]) & 0x00FF0000u)
				    |   (UINT64_LOW (sbox[6][8*i+6]) & 0x0000FF00u)
				    |   (UINT64_LOW (sbox[7][8*i+7]) & 0x000000FFu)
				    )
				);

	print_tables();
}
