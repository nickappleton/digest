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
#include "hash/tiger.h"
#include "simple_hash_test.h"

struct simple_tiger_test {
	const char *input;
	unsigned    input_repeats;
	const char *hash;
};

static const struct simple_tiger_test simple_tests[] =
{	{"", 1
	,"3293AC630C13F0245F92BBB1766E16167A4E58492DDE73F3"
	}
,	{"a", 1
	,"77BEFBEF2E7EF8AB2EC8F93BF587A7FC613E247F5F247809"
	}
,	{"abc", 1
	,"2AAB1484E8C158F2BFB8C5FF41B57A525129131C957B5F93"
	}
,	{"message digest", 1
	,"D981F8CB78201A950DCF3048751E441C517FCA1AA55A29F6"
	}
,	{"abcdefghijklmnopqrstuvwxyz", 1
	,"1714A472EEE57D30040412BFCC55032A0B11602FF37BEEE9"
	}
,	{"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 1
	,"0F7BF9A19B9C58F2B7610DF7E84F0AC3A71C631E7B53F78E"
	}
,	{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 1
	,"8DCEA680A17583EE502BA38A3C368651890FFBCCDC49A8CC"
	}
,	{"1234567890", 8
	,"1C14795529FD9F207A958F84C52F11E887FA0CABDFD91BFD"
	}
,	{"a", 1000000
	,"6DB0E2729CBEAD93D715C6A7D36302E9B3CEE0D2BC314B41"
	}
,	{"Tiger", 1
	,"DD00230799F5009FEC6DEBC838BB6A27DF2B9D6F110C7937"
	}
,	{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-", 1
	,"F71C8583902AFB879EDFE610F82C0D4786A3A534504486B5"
	}
,	{"ABCDEFGHIJKLMNOPQRSTUVWXYZ=abcdefghijklmnopqrstuvwxyz+0123456789", 1
	,"48CEEB6308B87D46E95D656112CDF18D97915F9765658957"
	}
,	{"Tiger - A Fast New Hash Function, by Ross Anderson and Eli Biham", 1
	,"8A866829040A410C729AD23F5ADA711603B3CDD357E4C15E"
	}
,	{"Tiger - A Fast New Hash Function, by Ross Anderson and Eli Biham, " \
	 "proceedings of Fast Software Encryption 3, Cambridge.", 1
	,"CE55A6AFD591F5EBAC547FF84F89227F9331DAB0B611C889"
	}
,	{"Tiger - A Fast New Hash Function, by Ross Anderson and Eli Biham, " \
	 "proceedings of Fast Software Encryption 3, Cambridge, 1996.", 1
	,"631ABDD103EB9A3D245B6DFD4D77B257FC7439501D1568DD"
	}
,	{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-ABCDE" \
	 "FGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-", 1
	,"C54034E5B43EB8005848A7E0AE6AAC76E4FF590AE715FD25"
	}
};

static
void run_simple_tiger(struct unittest_manager *manager, const void *parameter)
{
	const struct simple_tiger_test *p_test = parameter;
	struct hash_s tiger;

	if (tiger_create(&tiger)) {
		unittest_fail(manager, "failed to get hash context\n");
		return;
	}

	hashtest_string_test
		(manager
		,&tiger
		,p_test->input
		,p_test->input_repeats
		,p_test->hash
		);

	tiger.destroy(&tiger);
}

static const struct unittest tiger_internal_tests[] =
{	{"test1", NULL, run_simple_tiger, &simple_tests[0], NULL}
,	{"test2", NULL, run_simple_tiger, &simple_tests[1], NULL}
,	{"test3", NULL, run_simple_tiger, &simple_tests[2], NULL}
,	{"test4", NULL, run_simple_tiger, &simple_tests[3], NULL}
,	{"test5", NULL, run_simple_tiger, &simple_tests[4], NULL}
,	{"test6", NULL, run_simple_tiger, &simple_tests[5], NULL}
,	{"test7", NULL, run_simple_tiger, &simple_tests[6], NULL}
,	{"test8", NULL, run_simple_tiger, &simple_tests[7], NULL}
,	{"test9", NULL, run_simple_tiger, &simple_tests[8], NULL}
,	{"test10", NULL, run_simple_tiger, &simple_tests[9], NULL}
,	{"test11", NULL, run_simple_tiger, &simple_tests[10], NULL}
,	{"test12", NULL, run_simple_tiger, &simple_tests[11], NULL}
};

static const struct unittest *tiger_subtests[] =
{	&tiger_internal_tests[0]
,	&tiger_internal_tests[1]
,	&tiger_internal_tests[2]
,	&tiger_internal_tests[3]
,	&tiger_internal_tests[4]
,	&tiger_internal_tests[5]
,	&tiger_internal_tests[6]
,	&tiger_internal_tests[7]
,	&tiger_internal_tests[8]
,	&tiger_internal_tests[9]
,	&tiger_internal_tests[10]
,	&tiger_internal_tests[11]
,	NULL
};

const struct unittest tiger_tests =
{	"tiger"
,	"tiger tests"
,	NULL
,	NULL
,	tiger_subtests
};


