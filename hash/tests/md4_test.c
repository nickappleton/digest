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

#include "hash/md4.h"
#include "simple_hash_test.h"

struct simple_md4_test {
	const char                   *input;
	unsigned                      input_repeats;
	const char                   *hash;
};

static const struct simple_md4_test simple_tests[] =
	/* Test vectors taken from RFC 1320 */
{	{"", 1
	,"31D6CFE0D16AE931B73C59D7E0C089C0"
	}
,	{"a", 1
	,"BDE52CB31DE33E46245E05FBDBD6FB24"
	}
,	{"abc", 1
	,"A448017AAF21D8525FC10AE87AA6729D"
	}
,	{"message digest", 1
	,"D9130A8164549FE818874806E1C7014B"
	}
,	{"abcdefghijklmnopqrstuvwxyz", 1
	,"D79E1C308AA5BBCDEEA8ED63DF412DA9"
	}
,	{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 1
	,"043F8582F241DB351CE627E153E7F0E4"
	}
,	{"12345678901234567890123456789012345678901234567890123456789012345678901234567890", 1
	,"E33B4DDC9C38F2199C3E7B164FCC0536"
	}
};

static
void run_simple_md4(struct unittest_manager *manager, const void *parameter)
{
	const struct simple_md4_test *p_test = parameter;
	struct hash_s md4;

	if (md4_create(&md4)) {
		unittest_fail(manager, "failed to get hash context\n");
		return;
	}

	hashtest_string_test
		(manager
		,&md4
		,p_test->input
		,p_test->input_repeats
		,p_test->hash
		);

	md4.destroy(&md4);
}

static const struct unittest md4_internal_tests[] =
{	{"test1", NULL, run_simple_md4, &simple_tests[0], NULL}
,	{"test2", NULL, run_simple_md4, &simple_tests[1], NULL}
,	{"test3", NULL, run_simple_md4, &simple_tests[2], NULL}
,	{"test4", NULL, run_simple_md4, &simple_tests[3], NULL}
,	{"test5", NULL, run_simple_md4, &simple_tests[4], NULL}
,	{"test6", NULL, run_simple_md4, &simple_tests[5], NULL}
,	{"test7", NULL, run_simple_md4, &simple_tests[6], NULL}
};

static const struct unittest *md4_subtests[] =
{	&md4_internal_tests[0]
,	&md4_internal_tests[1]
,	&md4_internal_tests[2]
,	&md4_internal_tests[3]
,	&md4_internal_tests[4]
,	&md4_internal_tests[5]
,	&md4_internal_tests[6]
,	NULL
};

struct unittest md4_tests =
{	"md4"
,	"MD4 Tests"
,	NULL
,	NULL
,	md4_subtests
};


