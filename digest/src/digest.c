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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "hash/hash.h"
#include "hash/tiger.h"
#include "hash/sha1.h"
#include "hash/md5.h"
#include "hash/hashtree.h"

/* File reading buffer size */
#define BUFFER_SIZE (8192)

static
unsigned
print_lookup_digest(const unsigned char *data, unsigned data_bits, const char *lookup_table, unsigned lookup_bits)
{
	unsigned i = 0;
	while (i < data_bits) {
		unsigned c = 0;
		unsigned j;
		for (j = 0; (j < lookup_bits) && (i < data_bits); j++, i++)
			c = c | (((data[i/8] >> (7 - (i & 7))) & 1) << (lookup_bits - 1 - j));
		assert(c < (1 << lookup_bits));
		printf("%c", lookup_table[c]);
	}
	return data_bits % lookup_bits;
}

static
void
print_hex_digest(const unsigned char *digest, unsigned bits)
{
	static const char lookup[16] =
		{'0', '1', '2', '3' ,'4', '5', '6', '7'
		,'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
		};
	(void)print_lookup_digest(digest, bits, lookup, 4);
}

static
void
print_base32_digest(const unsigned char *digest, unsigned bits)
{
	static const char lookup[32] =
		{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'
		,'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P'
		,'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X'
		,'Y', 'Z', '2', '3', '4', '5', '6', '7'
		};
	unsigned i = print_lookup_digest(digest, bits, lookup, 5);
	/* TODO: add tests (there are vectors in RFC4648). This is probably
	 * wrong. */
	while (i > 0) {
		printf("=");
		i--;
	}
}

static
void
print_base64_digest(const unsigned char *digest, unsigned bits)
{
	static const char lookup[64] =
		{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'
		,'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P'
		,'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X'
		,'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f'
		,'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n'
		,'o', 'p', 'q', 'r', 's', 't', 'u', 'v'
		,'w', 'x', 'y', 'z', '0', '1', '2', '3'
		,'4', '5', '6', '7', '8', '9', '+', '/'
		};
	unsigned i = print_lookup_digest(digest, bits, lookup, 6) / 2;
	/* TODO: add tests (there are vectors in RFC4648). This is probably
	 * wrong. */
	while (i > 0) {
		printf("=");
		i--;
	}
}

typedef void (*digest_output_func)(const unsigned char *digest, unsigned bits);

struct output_fmt {
	const char         *name;
	digest_output_func  output;
};

static const struct output_fmt output_formats[] =
{	{"hex", print_hex_digest}
,	{"base32", print_base32_digest}
,	{"base64", print_base64_digest}
};

static
int
is_digit(char c)
{
	return (c >= '0') && (c <= '9');
}

/* Read an unsigned integer from the given string. The return value is the
 * position of the first character after the integer was parsed. If the
 * parse failed, the return value is NULL. */
static
const char *
parse_unsigned(const char *s, unsigned *x)
{
	*x = 0;
	do
	{
		if (!is_digit(*s)) {
			fprintf(stderr, "parse error: expected numerical digit\n");
			return NULL;
		}
		*x = (*x * 10) + (*s - '0');
		s++;
	} while (is_digit(*s));
	return s;
}

struct hash_step {
	struct hash_s      tree;
	struct hash_s      hash;
	int                tree_initialized;
	digest_output_func output;
	struct hash_step  *next;
};

static
int
tiger_setup(struct hash_step *step, const char *cfg_str)
{
	if (cfg_str) {
		fprintf(stderr, "cannot configure tiger with '%s'\n", cfg_str);
		return -1;
	}
	if (tiger_create(&step->hash)) {
		fprintf(stderr, "could not create tiger hash object\n");
		return -2;
	}
	return 0;
}

static
int
sha1_setup(struct hash_step *step, const char *cfg_str)
{
	if (cfg_str) {
		fprintf(stderr, "cannot configure SHA1 with '%s'\n", cfg_str);
		return -1;
	}
	if (sha1_create(&step->hash)) {
		fprintf(stderr, "could not create SHA1 hash object\n");
		return -2;
	}
	return 0;
}

static
int
md5_setup(struct hash_step *step, const char *cfg_str)
{
	if (cfg_str) {
		fprintf(stderr, "cannot configure MD5 with '%s'\n", cfg_str);
		return -1;
	}
	if (md5_create(&step->hash)) {
		fprintf(stderr, "could not create MD5 hash object\n");
		return -2;
	}
	return 0;
}

typedef int (*hash_cfg_func)(struct hash_step *step, const char *cfg_str);

struct hash_alg {
	const char    *name;
	hash_cfg_func  setup;
};

/* Searches the given string until the next separator ('.' or ':') or the end
 * of string is encountered for a matching supported hash algorithm. Returns
 * the position the scan finished on success. Returns NULL on fail. */
static
const char *
find_hashalg(const char *s, const struct hash_alg **alg)
{
	unsigned l = 0;
	unsigned i;
	static const struct hash_alg supported[] =
	{	{"tiger", tiger_setup}
	,	{"sha1", sha1_setup}
	,	{"md5", md5_setup}
	};

	while ((s[l] != ':') && (s[l] != '.') && (s[l] != '\0'))
		l++;

	*alg = NULL;
	for (i = 0; (*alg == NULL) && (i < (sizeof(supported) / sizeof(supported[0]))); i++)
		if ((strlen(supported[i].name) == l) && (strncmp(s, supported[i].name, l) == 0))
			*alg = supported + i;

	return (*alg != NULL) ? (s + l) : NULL;
}


static
const char *
parse_algorithm(const char *s, struct hash_step *step)
{
	const struct hash_alg *alg;
	char *p_args = NULL;

	s = find_hashalg(s, &alg);
	if (s == NULL) {
		fprintf(stderr, "parse error: expected hash algorithm name\n");
		return NULL;
	}

	/* Deal with arguments if they were given */
	if (*s == '.') {
		unsigned l = 0;
		s++;
		while ((s[l] != ':') && s[l] != '\0')
			l++;
		p_args = malloc(l + 1);
		if (!p_args) {
			fprintf(stderr, "oom\n");
			return NULL;
		}
		strncpy(p_args, s, l);
		p_args[l] = '\0';
		s += l;
	}

	/* Configure the step */
	if (alg->setup(step, p_args))
		s = NULL;

	/* Free the arguments */
	if (p_args)
		free(p_args);

	return s;
}

static const char *parse_output_format(const char *s, struct hash_step *step)
{
	const struct output_fmt *fmt = NULL;
	unsigned l = 0;
	unsigned i;
	assert(s);
	while ((s[l] != '.') && (s[l] != '\0'))
		l++;
	for (i = 0; (fmt == NULL) && (i < (sizeof(output_formats) / sizeof(output_formats[0]))); i++) {
		if ((strlen(output_formats[i].name) == l) && (strncmp(s, output_formats[i].name, l) == 0)) {
			fmt = output_formats + i;
		}
	}
	if (!fmt) {
		fprintf(stderr, "parse error: unsupported output formatter '%s'\n", s);
		return NULL;
	}
	step->output = fmt->output;
	return s + l;
}

unsigned parse_merkle_spec(const char *s, struct hash_step *step)
{
	unsigned hash_size     = 1024;
	step->tree_initialized = (strncmp(s, "tree", 4) == 0);
	if (step->tree_initialized) {
		s += 4;
		if (*s == '.') {
			s++;
			s = parse_unsigned(s, &hash_size);
		}
		if (s) {
			if (*s != ':') {
				fprintf(stderr, "parse error: expected ':' but got '%c'\n", *s);
				s = NULL;
			} else {
				s++;
			}
		}
	}
	if (s) {
		s = parse_algorithm(s, step);
	}
	if (s) {
		if (step->tree_initialized) {
			if (hashtree_create(&step->tree, &step->hash, hash_size, 0)) {
				fprintf(stderr, "failed to create hash tree\n");
				step->hash.destroy(&step->hash);
				s = 0;
			} else {
				step->tree.begin(&step->tree);
			}
		} else {
			step->hash.begin(&step->hash);
		}
	}
	if (s && *s != '\0') {
		assert(*s == ':');
		s = parse_output_format(s + 1, step);
		if ((s == NULL) || (*s != '\0')) {
			if (step->tree_initialized) {
				step->tree.destroy(&step->tree);
			}
			step->hash.destroy(&step->hash);
			s = NULL;
		}
	} else {
		step->output = print_hex_digest;
	}
	return (s) ? 0 : -1;
}

struct hash_step *str_to_spec(const char *s)
{
	struct hash_step *step = malloc(sizeof(struct hash_step));
	if (!step) {
		fprintf(stderr, "oom\n");
		return NULL;
	}
	step->next = 0;
	if (parse_merkle_spec(s, step)) {
		free(step);
		step = NULL;
	}
	return step;
}

static void process_file(FILE *f, struct hash_step *steps)
{
	unsigned char buffer[BUFFER_SIZE];
	size_t read;
	while ((read = fread(buffer, 1, sizeof(buffer), f))) {
		struct hash_step *t;
		for (t = steps; t != NULL; t = t->next) {
			if (t->tree_initialized)
				t->tree.process(&t->tree, buffer, read);
			else
				t->hash.process(&t->hash, buffer, read);
		}
	}
}

static int open_and_process(const char *filename, struct hash_step *steps)
{
	FILE *f = fopen(filename, "rb");
	if (!f) {
		fprintf(stderr, "could not open '%s'\n", filename);
		return -1;
	}
	process_file(f, steps);
	fclose(f);
	return 0;
}

void step_unlink(struct hash_step **n)
{
	struct hash_step *step;
	assert(n);
	step = *n;
	assert(step);
	if (step->tree_initialized)
		step->tree.destroy(&step->tree);
	step->hash.destroy(&step->hash);
	*n = step->next;
	free(step);
}

/* For all of the given steps, call the end method and print the digest in the
 * requested format. */
static
int
steps_finish_and_print(struct hash_step *steps)
{
	struct hash_step *t;
	for (t = steps; t != NULL; t = t->next) {
		struct hash_s *h = (t->tree_initialized) ? &t->tree : &t->hash;
		unsigned       dsize = h->query_digest_size(h);
		unsigned char *digest = malloc((dsize + 7) / 8);
		if (digest) {
			h->end(h, digest);
			t->output(digest, dsize);
			printf(" ");
			free(digest);
		} else {
			fprintf(stderr, "oom\n");
			return -1;
		}
	}
	printf("\n");
	return 0;
}

int
main(int argc, char *argv[])
{
	unsigned i = 1;
	int error = 0;
	struct hash_step *steps = NULL;
	struct hash_step **insert_pos = &steps;
	const char *filename = NULL;

	if (argc < 2) {
		/* FIXME: this is crap - come up with something else that makes a bit
		 * more sense... and it should be able to read from stdin. */
		printf("usage:\n");
		printf("  %s\n"
		       "     ( { [\"tree\", [\".\", block size], \":\"],\n"
		       "         algorithm,\n"
		       "         [\":\", format]\n"
		       "       }\n"
		       "     | ( \"-f\", filename )\n"
		       "     )\n\n", argv[0]);
		printf("Produces a set of hashes for data given through stdin or a file.\n\n");
		printf("A set of (optional) specifiers are given followed by either a single hyphen\n");
		printf("(signifying that input should be taken from stdin) or a filename. Each\n");
		printf("specifier can consist of an optional 'tree' prefix indicating that the root\n");
		printf("hash of a merkle tree should be used as the output. The tree prefix can take\n");
		printf("an optional integer argument (specified following a colon) to specify the block\n");
		printf("size. If the argument is not specified, it will default to 1024.\n\n");
		printf("The algorithm parameter specifies the name of the hash algorithm.\n\n");
		printf("The optional format specifier can be used to specify the display format of the\n");
		printf("output. If it is not given, it will default to hex.\n");
		exit(-1);
	}

	while ((i < argc) && !error) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'f':
				i++;
				if (filename) {
					fprintf(stderr, "filename already specified\n"); error = 1;
				} else if (i >= argc) {
					fprintf(stderr, "expected filename\n"); error = 1;
				} else {
					filename = argv[i];
				}
				break;
			default:
				fprintf(stderr, "unknown switch '%s'\n", &argv[i][1]); error = 1;
				break;
			}
		} else {
			*insert_pos = str_to_spec(argv[i]);
			error = (*insert_pos == NULL);
			insert_pos = &((*insert_pos)->next);
		}
		i++;
	}

	assert(steps);

	if ((steps != NULL) && !error) {
		if (filename) {
			error = open_and_process(filename, steps);
		} else {
			process_file(stdin, steps);
		}
	}

	if (!error)
		error = steps_finish_and_print(steps);

	while (steps)
		step_unlink(&steps);

	exit(error);

}


