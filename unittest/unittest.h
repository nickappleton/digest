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

#ifndef UNITTEST_H_
#define UNITTEST_H_

/* Using this API:
 *
 * Your unit test application should build unittest_main.c which implements
 * main(). The frontend will search for a unittest symbol named "root"
 * which you should create for your application. The descriptive name of this
 * object does not need to be root (i.e. the name member of the unittest
 * structure) - just the exported symbol.
 *
 * Unit tests are stored as a tree where each test may contain optional sub-
 * tests. The unittest structure contains the following members:
 *   name            - A short name for your unit test. This will be printed
 *                     to stdout when your test executes.
 *   description     - A description that describes what this unit test is
 *                     actually doing.
 *   run_fn          - A pointer to a unittest_fn to execute. This is what
 *                     will actually contain your unit test code. If this
 *                     member is NULL, test will be executed. It is useful if
 *                     the subtests member is non-NULL.
 *   run_fn_argument - A pointer to anything or nothing. It will be supplied
 *                     to the run_fn when the test is executed.
 *   subtests        - A NULL-terminated list of other unittest structures
 *                     which should be run as children of this unit test. This
 *                     field can be NULL to signify that the test has no sub-
 *                     tests.
 *
 * The unittest_fn execution functions take a pointer to a unittest_manager
 * and a void pointer as specified by the unittest's run_fn_argument. The
 * unittest_manager is how you inform the test harness of a failure. At
 * present there is only one available call: unittest_fail() which informs
 * the test harness the test failed. This function takes printf style
 * arguments to give a description of the failure.
 *
 * If the unittest_fn does not call unittest_fail(), the test is assumed to
 * have passed. */

struct unittest_manager;

typedef void (*unittest_fn)(struct unittest_manager *manager, const void *parameter);

struct unittest {
	const char       *name;
	const char       *description;
	unittest_fn       run_fn;
	const void       *run_fn_argument;
	const struct unittest **subtests;
};

void unittest_fail(struct unittest_manager *manager, const char *format, ...);

#endif /* UNITTEST_H_ */

