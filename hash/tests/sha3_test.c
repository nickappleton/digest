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
#include "hash/sha3.h"
#include "simple_hash_test.h"

struct simple_test_s {
	const char *input;
	unsigned    input_repeats;
	const char *hash;
	unsigned    digest_bits;
};

/* Forgive me for I have sinned; these references came from:
 *   http://en.wikipedia.org/wiki/Keccak
 *
 * I have run some of the long tests from the official Keccak "Known-answer
 * and Monte Carlo test results" from here:
 *   http://keccak.noekeon.org/files.html
 * They pass but they take an obscene amount of time to execute. I will add
 * some of the shorter tests later. */

static const struct simple_test_s sha3_512_test_data[] =
{	{"", 1
	,"0EAB42DE4C3CEB9235FC91ACFFE746B29C29A8C366B7C60E4E67C466F36A4304C00FA9CAF9D87976BA469BCBE06713B435F091EF2769FB160CDAB33D3670680E"
	,512
	}
,	{"The quick brown fox jumps over the lazy dog.", 1
	,"AB7192D2B11F51C7DD744E7B3441FEBF397CA07BF812CCEAE122CA4DED6387889064F8DB9230F173F6D1AB6E24B6E50F065B039F799F5592360A6558EB52D760"
	,512
	}
};

static const struct simple_test_s sha3_384_test_data[] =
{	{"", 1
	,"2C23146A63A29ACF99E73B88F8C24EAA7DC60AA771780CCC006AFBFA8FE2479B2DD2B21362337441AC12B515911957FF"
	,384
	}
,	{"The quick brown fox jumps over the lazy dog.", 1
	,"9AD8E17325408EDDB6EDEE6147F13856AD819BB7532668B605A24A2D958F88BD5C169E56DC4B2F89FFD325F6006D820B"
	,384
	}
};

static const struct simple_test_s sha3_256_test_data[] =
{	{"", 1
	,"C5D2460186F7233C927E7DB2DCC703C0E500B653CA82273B7BFAD8045D85A470"
	,256
	}
,	{"The quick brown fox jumps over the lazy dog.", 1
	,"578951E24EFD62A3D63A86F7CD19AAA53C898FE287D2552133220370240B572D"
	,256
	}
};

static const struct simple_test_s sha3_224_test_data[] =
{	{"", 1
	,"F71837502BA8E10837BDD8D365ADB85591895602FC552B48B7390ABD"
	,224
	}
,	{"The quick brown fox jumps over the lazy dog.", 1
	,"C59D4EAEAC728671C635FF645014E2AFA935BEBFFDB5FBD207FFDEAB"
	,224
	}
};

static
void run_simple_sha3(struct unittest_manager *manager, const void *parameter)
{
	const struct simple_test_s *p_test = parameter;
	struct hash_s sha3;

	if (sha3_create(&sha3, p_test->digest_bits)) {
		unittest_fail(manager, "failed to get hash context\n");
		return;
	}

	hashtest_string_test
		(manager
		,&sha3
		,p_test->input
		,p_test->input_repeats
		,p_test->hash
		);

	sha3.destroy(&sha3);
}

static const struct unittest sha3_512_internal_tests[] =
{	{"test1", NULL, run_simple_sha3, &sha3_512_test_data[0], NULL}
,	{"test2", NULL, run_simple_sha3, &sha3_512_test_data[1], NULL}
};

static const struct unittest sha3_384_internal_tests[] =
{	{"test1", NULL, run_simple_sha3, &sha3_384_test_data[0], NULL}
,	{"test2", NULL, run_simple_sha3, &sha3_384_test_data[1], NULL}
};

static const struct unittest sha3_256_internal_tests[] =
{	{"test1", NULL, run_simple_sha3, &sha3_256_test_data[0], NULL}
,	{"test2", NULL, run_simple_sha3, &sha3_256_test_data[1], NULL}
};

static const struct unittest sha3_224_internal_tests[] =
{	{"test1", NULL, run_simple_sha3, &sha3_224_test_data[0], NULL}
,	{"test2", NULL, run_simple_sha3, &sha3_224_test_data[1], NULL}
};

static const struct unittest *sha3_512_subtests[] =
{	&sha3_512_internal_tests[0]
,	&sha3_512_internal_tests[1]
,	NULL
};

static const struct unittest *sha3_384_subtests[] =
{	&sha3_384_internal_tests[0]
,	&sha3_384_internal_tests[1]
,	NULL
};

static const struct unittest *sha3_256_subtests[] =
{	&sha3_256_internal_tests[0]
,	&sha3_256_internal_tests[1]
,	NULL
};

static const struct unittest *sha3_224_subtests[] =
{	&sha3_224_internal_tests[0]
,	&sha3_224_internal_tests[1]
,	NULL
};

static const struct unittest sha3_512_tests =
{	"512"
,	"SHA-3 512 bit digest tests"
,	NULL
,	NULL
,	sha3_512_subtests
};

static const struct unittest sha3_384_tests =
{	"384"
,	"SHA-3 384 bit digest tests"
,	NULL
,	NULL
,	sha3_384_subtests
};

static const struct unittest sha3_256_tests =
{	"256"
,	"SHA-3 256 bit digest tests"
,	NULL
,	NULL
,	sha3_256_subtests
};

static const struct unittest sha3_224_tests =
{	"224"
,	"SHA-3 224 bit digest tests"
,	NULL
,	NULL
,	sha3_224_subtests
};

static const struct unittest *sha3_subtests[] =
{	&sha3_512_tests
,	&sha3_384_tests
,	&sha3_256_tests
,	&sha3_224_tests
,	NULL
};

const struct unittest sha3_tests =
{	"sha3"
,	"SHA-3 tests"
,	NULL
,	NULL
,	sha3_subtests
};

