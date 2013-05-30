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

#include "simple_hash_test.h"
#include <string.h>

static
char
tohex(int x)
{
	return (x > 9) ? ('A' + x - 10) : ('0' + x);
}

static
void
hashtest_data_test
	(struct unittest_manager *manager
	,struct hash_s           *dut
	,const unsigned char     *data
	,size_t                   data_size
	,unsigned                 repetitions
	,const char              *hex_reference
	)
{
	const unsigned digest_bits = dut->query_digest_size(dut);
	const unsigned reference_bits = strlen(hex_reference) * 4;
	char digest[129];
	unsigned char result[64];
	unsigned i;

	if (reference_bits != ((digest_bits + 3u) & (~3u))) {
		unittest_fail
			(manager
			,"query_digest_size() returned %u expected between %u and %u bits\n"
			,digest_bits
			,reference_bits - 3u
			,reference_bits
			);
		return;
	}

	dut->begin(dut);
	for (i = 0; i < repetitions; i++)
		dut->process(dut, data, data_size);
	dut->end(dut, result);

	for (i = 0; i < reference_bits / 8; i++) {
		digest[2*i+0] = tohex((result[i] & 0xF0) >> 4);
		digest[2*i+1] = tohex( result[i] & 0x0F );
	}
	digest[2*i] = '\0';

	if (strcmp(digest, hex_reference))
		unittest_fail
			(manager
			,"reference: '%s'\n"
			 "dut:       '%s'\n"
			,hex_reference
			,digest
			);
}

void
hashtest_string_test
	(struct unittest_manager *manager
	,struct hash_s           *dut
	,const char              *input_string
	,unsigned                 repetitions
	,const char              *hex_reference
	)
{
	hashtest_data_test
		(manager
		,dut
		,(const unsigned char *)input_string
		,strlen(input_string)
		,repetitions
		,hex_reference
		);
}


