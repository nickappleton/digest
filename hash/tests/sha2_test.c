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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hash/sha2.h"
#include "simple_hash_test.h"

struct simple_test_s {
	const char *input;
	unsigned    input_repeats;
	int         force_512;
	const char *hash;
	unsigned    digest_bits;
};

#define TEST1    "abc"

#define TEST2_1  "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
#define TEST2_2a "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"
#define TEST2_2b "hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"
#define TEST2_2  TEST2_2a TEST2_2b
#define TEST3    "a"
#define TEST4a   "01234567012345670123456701234567"
#define TEST4b   "01234567012345670123456701234567"
#define TEST4    TEST4a TEST4b
static const struct simple_test_s sha2_test_data[] =
	/* Test vectors taken from RFC 4634 */
{	{TEST1, 1, 0
	,"DDAF35A193617ABACC417349AE20413112E6FA4E89A97EA20A9EEEE64B55D39A2192992A274FC1A836BA3C23A3FEEBBD454D4423643CE80E2A9AC94FA54CA49F"
	,512
	}
,	{TEST2_2, 1, 0
	,"8E959B75DAE313DA8CF4F72814FC143F8F7779C6EB9F7FA17299AEADB6889018501D289E4900F7E4331B99DEC4B5433AC7D329EEB6DD26545E96E55B874BE909"
	,512
	}
,	{TEST3, 1000000, 0
	,"E718483D0CE769644E2E42C7BC15B4638E1F98B13B2044285632A803AFA973EBDE0FF244877EA60A4CB0432CE577C31BEB009C5C2C49AA2E4EADB217AD8CC09B"
	,512
	}
,	{TEST4, 10, 0
	,"89D05BA632C699C31231DED4FFC127D5A894DAD412C0E024DB872D1ABD2BA8141A0F85072A9BE1E2AA04CF33C765CB510813A39CD5A84C4ACAA64D3F3FB7BAE9"
	,512
	}
,	{TEST1, 1, 0
	,"CB00753F45A35E8BB5A03D699AC65007272C32AB0EDED1631A8B605A43FF5BED8086072BA1E7CC2358BAECA134C825A7"
	,384
	}
,	{TEST2_2, 1, 0
	,"09330C33F71147E83D192FC782CD1B4753111B173B3B05D22FA08086E3B0F712FCC7C71A557E2DB966C3E9FA91746039"
	,384
	}
,	{TEST3, 1000000, 0
	,"9D0E1809716474CB086E834E310A4A1CED149E9C00F248527972CEC5704C2A5B07B8B3DC38ECC4EBAE97DDD87F3D8985"
	,384
	}
,	{TEST4, 10, 0
	,"2FC64A4F500DDB6828F6A3430B8DD72A368EB7F3A8322A70BC84275B9C0B3AB00D27A5CC3C2D224AA6B61A0D79FB4596"
	,384
	}
,	{TEST1, 1, 0
	,"BA7816BF8F01CFEA414140DE5DAE2223B00361A396177A9CB410FF61F20015AD"
	,256
	}
,	{TEST2_1, 1, 0
	,"248D6A61D20638B8E5C026930C3E6039A33CE45964FF2167F6ECEDD419DB06C1"
	,256
	}
,	{TEST3, 1000000, 0
	,"CDC76E5C9914FB9281A1C7E284D73E67F1809A48A497200E046D39CCC7112CD0"
	,256
	}
,	{TEST4, 10, 0
	,"594847328451BDFA85056225462CC1D867D877FB388DF0CE35F25AB5562BFBB5"
	,256
	}
,	{TEST1, 1, 0
	,"23097D223405D8228642A477BDA255B32AADBCE4BDA0B3F7E36C9DA7"
	,224
	}
,	{TEST2_1, 1, 0
	,"75388B16512776CC5DBA5DA1FD890150B0C6455CB4F58B1952522525"
	,224
	}
,	{TEST3, 1000000, 0
	,"20794655980C91D8BBB4C1EA97618A4BF03F42581948B2EE4EE7AD67"
	,224
	}
,	{TEST4, 10, 0
	,"567F69F168CD7844E65259CE658FE7AADFA25216E68ECA0EB7AB8262"
	,224
	}
/* http://csrc.nist.gov/groups/ST/toolkit/documents/Examples/SHA512_256.pdf */
,	{TEST1, 1, 1
	,"53048E2681941EF99B2E29B76B4C7DABE4C2D0C634FC6D46E0E2F13107E7AF23"
	,256
	}
,	{TEST2_2, 1, 1
	,"3928E184FB8690F840DA3988121D31BE65CB9D3EF83EE6146FEAC861E19B563A"
	,256
	}
/* http://csrc.nist.gov/groups/ST/toolkit/documents/Examples/SHA512_224.pdf */
,	{TEST1, 1, 1
	,"4634270F707B6A54DAAE7530460842E20E37ED265CEEE9A43E8924AA"
	,224
	}
,	{TEST2_2, 1, 1
	,"23FEC5BB94D60B23308192640B0C453335D664734FE40E7268674AF9"
	,224
	}
};

static
void run_simple_sha2(struct unittest_manager *manager, const void *parameter)
{
	const struct simple_test_s *p_test = parameter;
	struct hash_s sha2;

	if (sha2_create(&sha2, p_test->digest_bits, p_test->force_512)) {
		unittest_fail(manager, "failed to get hash context\n");
		return;
	}

	hashtest_string_test
		(manager
		,&sha2
		,p_test->input
		,p_test->input_repeats
		,p_test->hash
		);

	sha2.destroy(&sha2);
}

static const struct unittest sha2_512_internal_tests[] =
{	{"test1", NULL, run_simple_sha2, &sha2_test_data[0], NULL}
,	{"test2", NULL, run_simple_sha2, &sha2_test_data[1], NULL}
,	{"test3", NULL, run_simple_sha2, &sha2_test_data[2], NULL}
,	{"test4", NULL, run_simple_sha2, &sha2_test_data[3], NULL}
};

static const struct unittest sha2_384_internal_tests[] =
{	{"test1", NULL, run_simple_sha2, &sha2_test_data[4], NULL}
,	{"test2", NULL, run_simple_sha2, &sha2_test_data[5], NULL}
,	{"test3", NULL, run_simple_sha2, &sha2_test_data[6], NULL}
,	{"test4", NULL, run_simple_sha2, &sha2_test_data[7], NULL}
};

static const struct unittest sha2_256_internal_tests[] =
{	{"test1", NULL, run_simple_sha2, &sha2_test_data[8], NULL}
,	{"test2", NULL, run_simple_sha2, &sha2_test_data[9], NULL}
,	{"test3", NULL, run_simple_sha2, &sha2_test_data[10], NULL}
,	{"test4", NULL, run_simple_sha2, &sha2_test_data[11], NULL}
};

static const struct unittest sha2_224_internal_tests[] =
{	{"test1", NULL, run_simple_sha2, &sha2_test_data[12], NULL}
,	{"test2", NULL, run_simple_sha2, &sha2_test_data[13], NULL}
,	{"test3", NULL, run_simple_sha2, &sha2_test_data[14], NULL}
,	{"test4", NULL, run_simple_sha2, &sha2_test_data[15], NULL}
};

static const struct unittest sha2_512_256_internal_tests[] =
{	{"test1", NULL, run_simple_sha2, &sha2_test_data[16], NULL}
,	{"test2", NULL, run_simple_sha2, &sha2_test_data[17], NULL}
};

static const struct unittest sha2_512_224_internal_tests[] =
{	{"test1", NULL, run_simple_sha2, &sha2_test_data[18], NULL}
,	{"test2", NULL, run_simple_sha2, &sha2_test_data[19], NULL}
};

static const struct unittest *sha2_512_subtests[] =
{	&sha2_512_internal_tests[0]
,	&sha2_512_internal_tests[1]
,	&sha2_512_internal_tests[2]
,	&sha2_512_internal_tests[3]
,	NULL
};

static const struct unittest *sha2_384_subtests[] =
{	&sha2_384_internal_tests[0]
,	&sha2_384_internal_tests[1]
,	&sha2_384_internal_tests[2]
,	&sha2_384_internal_tests[3]
,	NULL
};

static const struct unittest *sha2_256_subtests[] =
{	&sha2_256_internal_tests[0]
,	&sha2_256_internal_tests[1]
,	&sha2_256_internal_tests[2]
,	&sha2_256_internal_tests[3]
,	NULL
};

static const struct unittest *sha2_224_subtests[] =
{	&sha2_224_internal_tests[0]
,	&sha2_224_internal_tests[1]
,	&sha2_224_internal_tests[2]
,	&sha2_224_internal_tests[3]
,	NULL
};

static const struct unittest *sha2_512_256_subtests[] =
{	&sha2_512_256_internal_tests[0]
,	&sha2_512_256_internal_tests[1]
,	NULL
};

static const struct unittest *sha2_512_224_subtests[] =
{	&sha2_512_224_internal_tests[0]
,	&sha2_512_224_internal_tests[1]
,	NULL
};

static const struct unittest sha2_512_tests =
{	"512"
,	"SHA-2 512 bit digest tests"
,	NULL
,	NULL
,	sha2_512_subtests
};

static const struct unittest sha2_384_tests =
{	"384"
,	"SHA-2 384 bit digest tests"
,	NULL
,	NULL
,	sha2_384_subtests
};

static const struct unittest sha2_256_tests =
{	"256"
,	"SHA-2 256 bit digest tests"
,	NULL
,	NULL
,	sha2_256_subtests
};

static const struct unittest sha2_224_tests =
{	"224"
,	"SHA-2 224 bit digest tests"
,	NULL
,	NULL
,	sha2_224_subtests
};

static const struct unittest sha2_512_256_tests =
{	"512/256"
,	"SHA-2 512/256 bit digest tests"
,	NULL
,	NULL
,	sha2_512_256_subtests
};

static const struct unittest sha2_512_224_tests =
{	"512/224"
,	"SHA-2 512/224 bit digest tests"
,	NULL
,	NULL
,	sha2_512_224_subtests
};

static const struct unittest *sha2_subtests[] =
{	&sha2_512_tests
,	&sha2_512_224_tests
,	&sha2_512_256_tests
,	&sha2_384_tests
,	&sha2_256_tests
,	&sha2_224_tests
,	NULL
};

const struct unittest sha2_tests =
{	"sha2"
,	"SHA-2 tests"
,	NULL
,	NULL
,	sha2_subtests
};

