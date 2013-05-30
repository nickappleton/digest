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
#include <stdarg.h>
#include <string.h>
#include "unittest/unittest.h"

extern struct unittest root;

struct unittest_manager {
	int      current_has_failed;
	unsigned level;
	unsigned executed;
	unsigned failed;
};

#define MAX_LINE_BUF_SIZE (2048)

void unittest_fail(struct unittest_manager *manager, const char *format, ...)
{
	if (!manager->current_has_failed) {
		unsigned i;
		va_list ap;
		char *buffer = malloc(MAX_LINE_BUF_SIZE);
		char *p;
		printf("failed\n");
		va_start(ap, format);
		vsnprintf(buffer, MAX_LINE_BUF_SIZE, format, ap);
		va_end(ap);

		p = buffer;
		do {
			char *np = strchr(p, '\n');
			if (np)
				*np = '\0';

			if (*p) {
				for (i = 0; i < manager->level; i++) {
					printf("  ");
				}
				printf("%s\n", p);
			}

			p = (np) ? (np + 1) : NULL;
		} while (p);

		manager->current_has_failed = 1;
	}
}

static void run_test(struct unittest_manager *manager, const struct unittest *test)
{
	unsigned i;
	for (i = 0; i < manager->level; i++)
		printf("  ");

	if (test->run_fn)
		printf("%s... ", test->name);
	else
		printf("%s\n", test->name);

	manager->level++;
	if (test->run_fn) {
		manager->current_has_failed = 0;
		test->run_fn(manager, test->run_fn_argument);
		manager->executed++;
		if (manager->current_has_failed)
			manager->failed++;
		else
			printf("passed\n");
	}

	if (test->subtests) {
		const struct unittest **s = test->subtests;
		while (*s)
			run_test(manager, *s++);
	}
	manager->level--;
}

int main(int argc, char *argv[])
{
	struct unittest_manager mgr;
	mgr.current_has_failed = 0;
	mgr.executed = 0;
	mgr.failed = 0;
	mgr.level = 0;
	run_test(&mgr, &root);

	if (mgr.failed)
		printf("%u/%u tests failed\n", mgr.failed, mgr.executed);
	else
		printf("%u tests passed\n", mgr.executed);

	exit(mgr.failed);
}
