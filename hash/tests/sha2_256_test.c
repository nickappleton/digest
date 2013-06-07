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
#include "hash/src/sha2_256.h"
#include "simple_hash_test.h"

struct simple_test_s {
	const char *input;
	unsigned    input_repeats;
	const char *hash;
	unsigned    digest_bits;
};

/* Test vectors taken from RFC 4634 */
#define TEST1    "abc"
#define TEST2_1  "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
#define TEST3    "a"
#define TEST4a   "01234567012345670123456701234567"
#define TEST4b   "01234567012345670123456701234567"
#define TEST4    TEST4a TEST4b
static const struct simple_test_s sha2_256_test_data[] =
{	{TEST1, 1
	,"BA7816BF8F01CFEA414140DE5DAE2223B00361A396177A9CB410FF61F20015AD"
	,256
	}
,	{TEST2_1, 1
	,"248D6A61D20638B8E5C026930C3E6039A33CE45964FF2167F6ECEDD419DB06C1"
	,256
	}
,	{TEST3, 1000000
	,"CDC76E5C9914FB9281A1C7E284D73E67F1809A48A497200E046D39CCC7112CD0"
	,256
	}
,	{TEST4, 10
	,"594847328451BDFA85056225462CC1D867D877FB388DF0CE35F25AB5562BFBB5"
	,256
	}
,	{TEST1, 1
	,"23097D223405D8228642A477BDA255B32AADBCE4BDA0B3F7E36C9DA7"
	,224
	}
,	{TEST2_1, 1
	,"75388B16512776CC5DBA5DA1FD890150B0C6455CB4F58B1952522525"
	,224
	}
,	{TEST3, 1000000
	,"20794655980C91D8BBB4C1EA97618A4BF03F42581948B2EE4EE7AD67"
	,224
	}
,	{TEST4, 10
	,"567F69F168CD7844E65259CE658FE7AADFA25216E68ECA0EB7AB8262"
	,224
	}
};

static
void run_simple_sha2_256(struct unittest_manager *manager, const void *parameter)
{
	const struct simple_test_s *p_test = parameter;
	struct hash_s sha2;

	if (sha2_256_create(&sha2, p_test->digest_bits)) {
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

static const struct unittest sha2_256_256_internal_tests[] =
{	{"test1", NULL, run_simple_sha2_256, &sha2_256_test_data[0], NULL}
,	{"test2", NULL, run_simple_sha2_256, &sha2_256_test_data[1], NULL}
,	{"test3", NULL, run_simple_sha2_256, &sha2_256_test_data[2], NULL}
,	{"test4", NULL, run_simple_sha2_256, &sha2_256_test_data[3], NULL}
};

static const struct unittest *sha2_256_256_subtests[] =
{	&sha2_256_256_internal_tests[0]
,	&sha2_256_256_internal_tests[1]
,	&sha2_256_256_internal_tests[2]
,	&sha2_256_256_internal_tests[3]
,	NULL
};

static const struct unittest sha2_256_256_tests =
{	"256-bit"
,	"SHA-2-256 256 bit digest tests"
,	NULL
,	NULL
,	sha2_256_256_subtests
};

static const struct unittest sha2_256_224_internal_tests[] =
{	{"test1", NULL, run_simple_sha2_256, &sha2_256_test_data[4], NULL}
,	{"test2", NULL, run_simple_sha2_256, &sha2_256_test_data[5], NULL}
,	{"test3", NULL, run_simple_sha2_256, &sha2_256_test_data[6], NULL}
,	{"test4", NULL, run_simple_sha2_256, &sha2_256_test_data[7], NULL}
};

static const struct unittest *sha2_256_224_subtests[] =
{	&sha2_256_224_internal_tests[0]
,	&sha2_256_224_internal_tests[1]
,	&sha2_256_224_internal_tests[2]
,	&sha2_256_224_internal_tests[3]
,	NULL
};

static const struct unittest sha2_256_224_tests =
{	"224-bit"
,	"SHA-2-256 224 bit digest tests"
,	NULL
,	NULL
,	sha2_256_224_subtests
};

static const struct unittest *sha2_256_subtests[] =
{	&sha2_256_224_tests
,	&sha2_256_256_tests
,	NULL
};

const struct unittest sha2_256_tests =
{	"sha2-256"
,	"SHA-2-256 tests"
,	NULL
,	NULL
,	sha2_256_subtests
};

