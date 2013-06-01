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
#include "hash/sha1.h"
#include "simple_hash_test.h"

struct simple_sha1_test {
	const char                   *input;
	unsigned                      input_repeats;
	const char                   *hash;
};

/* Test vectors taken from RFC 3174 */
#define TEST1   "abc"
#define TEST2a  "abcdbcdecdefdefgefghfghighijhi"
#define TEST2b  "jkijkljklmklmnlmnomnopnopq"
#define TEST2   TEST2a TEST2b
#define TEST3   "a"
#define TEST4a  "01234567012345670123456701234567"
#define TEST4b  "01234567012345670123456701234567"
#define TEST4   TEST4a TEST4b

static const struct simple_sha1_test simple_tests[] =
{	{TEST1, 1
	,"A9993E364706816ABA3E25717850C26C9CD0D89D"
	}
,	{TEST2, 1
	,"84983E441C3BD26EBAAE4AA1F95129E5E54670F1"
	}
,	{TEST3, 1000000
	,"34AA973CD4C4DAA4F61EEB2BDBAD27316534016F"
	}
,	{TEST4, 10
	,"DEA356A2CDDD90C7A7ECEDC5EBB563934F460452"
	}
};

static
void run_simple_sha1(struct unittest_manager *manager, const void *parameter)
{
	const struct simple_sha1_test *p_test = parameter;
	struct hash_s sha1;

	if (sha1_create(&sha1)) {
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

static const struct unittest sha1_internal_tests[] =
{	{"test1", NULL, run_simple_sha1, &simple_tests[0], NULL}
,	{"test2", NULL, run_simple_sha1, &simple_tests[1], NULL}
,	{"test3", NULL, run_simple_sha1, &simple_tests[2], NULL}
,	{"test4", NULL, run_simple_sha1, &simple_tests[3], NULL}
};

static const struct unittest *sha1_subtests[] =
{	&sha1_internal_tests[0]
,	&sha1_internal_tests[1]
,	&sha1_internal_tests[2]
,	&sha1_internal_tests[3]
,	NULL
};

struct unittest sha1_tests =
{	"sha1"
,	"SHA-1 Tests"
,	NULL
,	NULL
,	sha1_subtests
};

