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

#include "hash/whirlpool.h"
#include "simple_hash_test.h"

struct simple_whirlpool_test {
    const char                   *input;
    unsigned                      input_repeats;
    const char                   *hash;
};

static const struct simple_whirlpool_test simple_tests[] =
    /* Test vectors taken from reference implementation */
{   {"", 1
    ,"19FA61D75522A4669B44E39C1D2E1726C530232130D407F89AFEE0964997F7A73E83BE698B288FEBCF88E3E03C4F0757EA8964E59B63D93708B138CC42A66EB3"
    }
,   {"a", 1
    ,"8ACA2602792AEC6F11A67206531FB7D7F0DFF59413145E6973C45001D0087B42D11BC645413AEFF63A42391A39145A591A92200D560195E53B478584FDAE231A"
    }
,   {"abc", 1
    ,"4E2448A4C6F486BB16B6562C73B4020BF3043E3A731BCE721AE1B303D97E6D4C7181EEBDB6C57E277D0E34957114CBD6C797FC9D95D8B582D225292076D4EEF5"
    }
,   {"message digest", 1
    ,"378C84A4126E2DC6E56DCC7458377AAC838D00032230F53CE1F5700C0FFB4D3B8421557659EF55C106B4B52AC5A4AAA692ED920052838F3362E86DBD37A8903E"
    }
,   {"abcdefghijklmnopqrstuvwxyz", 1
    ,"F1D754662636FFE92C82EBB9212A484A8D38631EAD4238F5442EE13B8054E41B08BF2A9251C30B6A0B8AAE86177AB4A6F68F673E7207865D5D9819A3DBA4EB3B"
    }
,   {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 1
    ,"DC37E008CF9EE69BF11F00ED9ABA26901DD7C28CDEC066CC6AF42E40F82F3A1E08EBA26629129D8FB7CB57211B9281A65517CC879D7B962142C65F5A7AF01467"
    }
,   {"1234567890", 8
    ,"466EF18BABB0154D25B9D38A6414F5C08784372BCCB204D6549C4AFADB6014294D5BD8DF2A6C44E538CD047B2681A51A2C60481E88C5A20B2C2A80CF3A9A083B"
    }
,   {"abcdbcdecdefdefgefghfghighijhijk", 1
    ,"2A987EA40F917061F5D6F0A0E4644F488A7A5A52DEEE656207C562F988E95C6916BDC8031BC5BE1B7B947639FE050B56939BAAA0ADFF9AE6745B7B181C3BE3FD"
    }
,   {"a", 1000000
    ,"0C99005BEB57EFF50A7CF005560DDF5D29057FD86B20BFD62DECA0F1CCEA4AF51FC15490EDDC47AF32BB2B66C34FF9AD8C6008AD677F77126953B226E4ED8B01"
    }
};

static
void run_simple_whirlpool(struct unittest_manager *manager, const void *parameter)
{
    const struct simple_whirlpool_test *p_test = parameter;
    struct hash_s whirlpool;

    if (whirlpool_create(&whirlpool)) {
        unittest_fail(manager, "failed to get hash context\n");
        return;
    }

    hashtest_string_test
        (manager
        ,&whirlpool
        ,p_test->input
        ,p_test->input_repeats
        ,p_test->hash
        );

    whirlpool.destroy(&whirlpool);
}

static const struct unittest whirlpool_internal_tests[] =
{   {"test1", NULL, run_simple_whirlpool, &simple_tests[0], NULL}
,   {"test2", NULL, run_simple_whirlpool, &simple_tests[1], NULL}
,   {"test3", NULL, run_simple_whirlpool, &simple_tests[2], NULL}
,   {"test4", NULL, run_simple_whirlpool, &simple_tests[3], NULL}
,   {"test5", NULL, run_simple_whirlpool, &simple_tests[4], NULL}
,   {"test6", NULL, run_simple_whirlpool, &simple_tests[5], NULL}
,   {"test7", NULL, run_simple_whirlpool, &simple_tests[6], NULL}
};

static const struct unittest *whirlpool_subtests[] =
{   &whirlpool_internal_tests[0]
,   &whirlpool_internal_tests[1]
,   &whirlpool_internal_tests[2]
,   &whirlpool_internal_tests[3]
,   &whirlpool_internal_tests[4]
,   &whirlpool_internal_tests[5]
,   &whirlpool_internal_tests[6]
,   NULL
};

const struct unittest whirlpool_tests =
{   "whirlpool"
,   "Whirlpool Tests"
,   NULL
,   NULL
,   whirlpool_subtests
};


