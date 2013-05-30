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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hash/src/sha2_512.h"
#include "simple_hash_test.h"

struct simple_test_s {
	const char *input;
	unsigned    input_repeats;
	const char *hash;
	unsigned    digest_bits;
};

/* Test vectors taken from RFC 4634 */
#define TEST1    "abc"
#define TEST2_2a "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"
#define TEST2_2b "hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"
#define TEST2_2  TEST2_2a TEST2_2b
#define TEST3    "a"
#define TEST4a   "01234567012345670123456701234567"
#define TEST4b   "01234567012345670123456701234567"
#define TEST4    TEST4a TEST4b
static const struct simple_test_s sha2_512_test_data[] =
{	{TEST1, 1
	,"DDAF35A193617ABACC417349AE20413112E6FA4E89A97EA20A9EEEE64B55D39A2192992A274FC1A836BA3C23A3FEEBBD454D4423643CE80E2A9AC94FA54CA49F"
	,512
	}
,	{TEST2_2, 1
	,"8E959B75DAE313DA8CF4F72814FC143F8F7779C6EB9F7FA17299AEADB6889018501D289E4900F7E4331B99DEC4B5433AC7D329EEB6DD26545E96E55B874BE909"
	,512
	}
,	{TEST3, 1000000
	,"E718483D0CE769644E2E42C7BC15B4638E1F98B13B2044285632A803AFA973EBDE0FF244877EA60A4CB0432CE577C31BEB009C5C2C49AA2E4EADB217AD8CC09B"
	,512
	}
,	{TEST4, 10
	,"89D05BA632C699C31231DED4FFC127D5A894DAD412C0E024DB872D1ABD2BA8141A0F85072A9BE1E2AA04CF33C765CB510813A39CD5A84C4ACAA64D3F3FB7BAE9"
	,512
	}
,	{TEST1, 1
	,"CB00753F45A35E8BB5A03D699AC65007272C32AB0EDED1631A8B605A43FF5BED8086072BA1E7CC2358BAECA134C825A7"
	,384
	}
,	{TEST2_2, 1
	,"09330C33F71147E83D192FC782CD1B4753111B173B3B05D22FA08086E3B0F712FCC7C71A557E2DB966C3E9FA91746039"
	,384
	}
,	{TEST3, 1000000
	,"9D0E1809716474CB086E834E310A4A1CED149E9C00F248527972CEC5704C2A5B07B8B3DC38ECC4EBAE97DDD87F3D8985"
	,384
	}
,	{TEST4, 10
	,"2FC64A4F500DDB6828F6A3430B8DD72A368EB7F3A8322A70BC84275B9C0B3AB00D27A5CC3C2D224AA6B61A0D79FB4596"
	,384
	}
};

static
void run_simple_sha2_512(struct unittest_manager *manager, const void *parameter)
{
	const struct simple_test_s *p_test = parameter;
	struct hash_s sha1;

	if (sha2_512_create(&sha1, p_test->digest_bits)) {
		unittest_fail(manager, "failed to get hash context\n");
		return;
	}

	hashtest_string_test
		(manager
		,&sha1
		,p_test->input
		,p_test->input_repeats
		,p_test->hash
		);

	sha1.destroy(&sha1);
}

static const struct unittest sha2_512_internal_tests[] =
{	{"test1", NULL, run_simple_sha2_512, &sha2_512_test_data[0], NULL}
,	{"test2", NULL, run_simple_sha2_512, &sha2_512_test_data[1], NULL}
,	{"test3", NULL, run_simple_sha2_512, &sha2_512_test_data[2], NULL}
,	{"test4", NULL, run_simple_sha2_512, &sha2_512_test_data[3], NULL}
,	{"test5", NULL, run_simple_sha2_512, &sha2_512_test_data[4], NULL}
,	{"test6", NULL, run_simple_sha2_512, &sha2_512_test_data[5], NULL}
,	{"test7", NULL, run_simple_sha2_512, &sha2_512_test_data[6], NULL}
,	{"test8", NULL, run_simple_sha2_512, &sha2_512_test_data[7], NULL}
};

static const struct unittest *sha2_512_subtests[] =
{	&sha2_512_internal_tests[0]
,	&sha2_512_internal_tests[1]
,	&sha2_512_internal_tests[2]
,	&sha2_512_internal_tests[3]
,	&sha2_512_internal_tests[4]
,	&sha2_512_internal_tests[5]
,	&sha2_512_internal_tests[6]
,	&sha2_512_internal_tests[7]
,	NULL
};

struct unittest sha2_512_tests =
{	"sha2-512-512"
,	"SHA-2-512 tests"
,	NULL
,	NULL
,	sha2_512_subtests
};

