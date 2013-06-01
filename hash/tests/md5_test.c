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

#include "hash/md5.h"
#include "simple_hash_test.h"

struct simple_md5_test {
	const char                   *input;
	unsigned                      input_repeats;
	const char                   *hash;
};

static const struct simple_md5_test simple_tests[] =
	/* Test vectors taken from RFC 1321 */
{	{"", 1
	,"D41D8CD98F00B204E9800998ECF8427E"
	}
,	{"a", 1
	,"0CC175B9C0F1B6A831C399E269772661"
	}
,	{"abc", 1
	,"900150983CD24FB0D6963F7D28E17F72"
	}
,	{"message digest", 1
	,"F96B697D7CB7938D525A2F31AAF161D0"
	}
,	{"abcdefghijklmnopqrstuvwxyz", 1
	,"C3FCD3D76192E4007DFB496CCA67E13B"
	}
,	{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 1
	,"D174AB98D277D9F5A5611C2C9F419D9F"
	}
,	{"12345678901234567890123456789012345678901234567890123456789012345678901234567890", 1
	,"57EDF4A22BE3C955AC49DA2E2107B67A"
	}
};

static
void run_simple_md5(struct unittest_manager *manager, const void *parameter)
{
	const struct simple_md5_test *p_test = parameter;
	struct hash_s md5;

	if (md5_create(&md5)) {
		unittest_fail(manager, "failed to get hash context\n");
		return;
	}

	hashtest_string_test
		(manager
		,&md5
		,p_test->input
		,p_test->input_repeats
		,p_test->hash
		);

	md5.destroy(&md5);
}

static const struct unittest md5_internal_tests[] =
{	{"test1", NULL, run_simple_md5, &simple_tests[0], NULL}
,	{"test2", NULL, run_simple_md5, &simple_tests[1], NULL}
,	{"test3", NULL, run_simple_md5, &simple_tests[2], NULL}
,	{"test4", NULL, run_simple_md5, &simple_tests[3], NULL}
,	{"test5", NULL, run_simple_md5, &simple_tests[4], NULL}
,	{"test6", NULL, run_simple_md5, &simple_tests[5], NULL}
,	{"test7", NULL, run_simple_md5, &simple_tests[6], NULL}
};

static const struct unittest *md5_subtests[] =
{	&md5_internal_tests[0]
,	&md5_internal_tests[1]
,	&md5_internal_tests[2]
,	&md5_internal_tests[3]
,	&md5_internal_tests[4]
,	&md5_internal_tests[5]
,	&md5_internal_tests[6]
,	NULL
};

struct unittest md5_tests =
{	"md5"
,	"MD5 Tests"
,	NULL
,	NULL
,	md5_subtests
};




