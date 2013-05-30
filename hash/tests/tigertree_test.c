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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash/hashtree.h"
#include "hash/tiger.h"
#include "simple_hash_test.h"

struct simple_test_s {
	const char *input;
	unsigned    input_repeats;
	const char *hash;
};

/* Test vectors obtained from here:
 * http://tigertree.cvs.sourceforge.net/viewvc/tigertree/tigertree/tigertree/TESTOUTPUT?revision=1.1&view=markup */
static const struct simple_test_s tiger_tree_tests[] =
{	{"", 1
	,"3293AC630C13F0245F92BBB1766E16167A4E58492DDE73F3"
	}
,	{"abc", 1
	,"2AAB1484E8C158F2BFB8C5FF41B57A525129131C957B5F93"
	}
,	{"Tiger", 1
	,"DD00230799F5009FEC6DEBC838BB6A27DF2B9D6F110C7937"
	}
,	{"1", 1
	,"1D573194A056EB3200F9D302900C843C3D41AB4ED06C03DF"
	}
,	{"TigerTree", 1
	,"A314CF48D2E154A60067690E34BFE2763C176F34EE99BEAC"
	}
,	{"a", 1024
	,"96CD77A3D4A04D0CC85EE0297E2984C76AB723F8C5447F4D"
	}
,	{"a", 1025
	,"D1593128F306DD1C2831459205F14CCB843E675F021F2EA2"
	}
,	{"b", 17408
	,"AD26CE236D7779B8F1CAF78DF147CCBC7620BABB8E42EDC1"
	}
,	{"b", 17409
	,"C2708C80DB97E655B4E1F0218AF53F7ADCAB06053CD104C2"
	}
};

static
void run_simple_tigertree(struct unittest_manager *manager, const void *parameter)
{
	const struct simple_test_s *p_test = parameter;
	struct hash_s tiger;
	struct hash_s tree;

	if (tiger_create(&tiger)) {
		unittest_fail(manager, "failed to get hash context\n");
		return;
	}

	if (hashtree_create(&tree, &tiger, 1024, 1)) {
		unittest_fail(manager, "failed to get tree context\n");
		tiger.destroy(&tiger);
		return;
	}

	hashtest_string_test
		(manager
		,&tree
		,p_test->input
		,p_test->input_repeats
		,p_test->hash
		);

	tree.destroy(&tree);
	tiger.destroy(&tiger);
}

static const struct unittest tigertree_internal_tests[] =
{	{"test1", NULL, run_simple_tigertree, &tiger_tree_tests[0], NULL}
,	{"test2", NULL, run_simple_tigertree, &tiger_tree_tests[1], NULL}
,	{"test3", NULL, run_simple_tigertree, &tiger_tree_tests[2], NULL}
,	{"test4", NULL, run_simple_tigertree, &tiger_tree_tests[3], NULL}
,	{"test5", NULL, run_simple_tigertree, &tiger_tree_tests[4], NULL}
,	{"test6", NULL, run_simple_tigertree, &tiger_tree_tests[5], NULL}
,	{"test7", NULL, run_simple_tigertree, &tiger_tree_tests[6], NULL}
,	{"test8", NULL, run_simple_tigertree, &tiger_tree_tests[7], NULL}
,	{"test9", NULL, run_simple_tigertree, &tiger_tree_tests[8], NULL}
};

static const struct unittest *tigertree_subtests[] =
{	&tigertree_internal_tests[0]
,	&tigertree_internal_tests[1]
,	&tigertree_internal_tests[2]
,	&tigertree_internal_tests[3]
,	&tigertree_internal_tests[4]
,	&tigertree_internal_tests[5]
,	&tigertree_internal_tests[6]
,	&tigertree_internal_tests[7]
,	&tigertree_internal_tests[8]
,	NULL
};

struct unittest tigertree_tests =
{	"tigertree"
,	"tigertree tests"
,	NULL
,	NULL
,	tigertree_subtests
};



