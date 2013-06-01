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

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include "hash/hashtree.h"

/* Hash tree key structure. */
struct htk_s {
	/* Specifies how many times the data has been hashed from other hashes.
	 * i.e. all new data which is added starts with a rank of zero. */
	unsigned       rank;

	/* The key for the hash (length is the key_size member of the private
	 * state object). */
	unsigned char *data;

	/* Doubly-linked list connectors. */
	struct htk_s  *next;
	struct htk_s  *prev;
};

struct hash_pvt_s {
	/* Input data buffering. */
	size_t         block_size;
	size_t         block_index;
	unsigned char *block_data;

	/* Tree layers to preserve. */
	unsigned       depth_bits;

	/* Cached size of the digest and hash function object. */
	size_t         key_size;
	struct hash_s *hash;

	/* Base memory for all htk_s structures. Simplifies freeing of memory
	 * later on as we move pointers all over the place. */
	void          *basemem;

	/* Pool of htk_s structures and pointers to the first and last elements
	 * of the node list. */
	struct htk_s  *pool;
	struct htk_s  *first;
	struct htk_s  *last;

	/* The current number of consecutive elements which have the same rank
	 * counting from the first element. Used to know when to collapse the
	 * tree down. */
	unsigned       rll;
};

/* This function unlinks key from the list and returns it to the pool.
 * This function is undefined when key is not already linked.  */
static
void
unlink_key(struct hash_pvt_s *tree, struct htk_s *key)
{
	if (key->prev)
		key->prev->next = key->next;
	else
		tree->first = key->next;
	if (key->next)
		key->next->prev = key->prev;
	else
		tree->last = key->prev;
	key->next = tree->pool;
	tree->pool = key;
}

/* This function combines key with following key. The following key is then
 * unlinked from the list.
 * This function is obviously undefined when key->next is NULL. */
static
void
combine_discard(struct hash_pvt_s *tree, struct htk_s *key)
{
	tree->hash->begin(tree->hash);
	tree->hash->process(tree->hash, key->data, tree->key_size);
	tree->hash->process(tree->hash, key->next->data, tree->key_size);
	tree->hash->end(tree->hash, key->data);
	unlink_key(tree, key->next);
}

/* Collapses the end of the list such to try and make the tail of at least
 * the rank of the root element.
 * This function is undefined when tree->last is NULL. */
static
void
compact_end(struct hash_pvt_s *tree)
{
	while
	    (   (tree->last->prev != NULL) /* There is a node before the last node */
	    &&  (tree->last->rank == tree->last->prev->rank) /* The previous node has the same rank */
	    &&  (tree->last->rank < tree->first->rank) /* The rank is less than the rank of the root node */
	    ) {
		combine_discard(tree, tree->last->prev); /* Combine the last two nodes */
		/* If the rank is the same as the root rank, we have reached the
		 * bottom of the tree and can increase the number of root shared rank
		 * nodes. */
		tree->last->rank++;
		if (tree->last->rank == tree->first->rank)
			tree->rll++;
	}
}

/* Collapses the start of the list by merging all consecutive keys of the same
 * rank.
 * This function is undefined when tree->first is NULL. */
static
void
compact_start(struct hash_pvt_s *tree)
{
	const unsigned pr = tree->first->rank;
	struct htk_s *key = tree->first;
	if ((key) && (key->rank == pr) && (key->next) && (key->next->rank == pr)) {
		tree->rll = 0;
		do {
			combine_discard(tree, key);
			key->rank++;
			tree->rll++;
			key = key->next;
		} while ((key) && (key->rank == pr) && (key->next) && (key->next->rank == pr));
	}
}

/* Appends a new hash into the tree. k only needs to have the data member
 * specified - all other values will be set by this function. */
static
void
tree_append(struct hash_pvt_s *tree, struct htk_s *k)
{
	/* If this is the first node insertion or the rank of the first node is
	 * still zero, increase the count of root level shared rank nodes. */
	if (!tree->first || !tree->first->rank)
		tree->rll++;

	/* Configure and insert the new node on to the end of the list. */
	k->rank = 0;
	k->prev = tree->last;
	k->next = 0;
	if (k->prev)
		k->prev->next = k;
	else
		tree->first = k;
	tree->last = k;

	/* Compact the end of the tree. If we have a root count larger than what
	 * we need to preserve given the tree depth, increase the rank of the main
	 * stem by compact the start of the tree. */
	compact_end(tree);
	if (tree->rll > (1u << tree->depth_bits))
		compact_start(tree);

#if 0 /* USEFUL FOR DEBUGGING */
	{
		struct htk_s *n = tree->first;
		printf("%u: ", tree->rll);
		while (n) {
			printf("%u ", n->rank);
			n = n->next;
		}
		printf("\n");
	}
#endif
}

/* Hash a block of data and add the hash into the tree. This function takes
 * both the data and size (even though the state contains what seems to be
 * the same information) because the last block may not be complete and this
 * also avoids copying memory to the temporary buffer when process is called
 * with large blocks of memory. */
static
void
run_block(struct hash_pvt_s *tree, const unsigned char *data, size_t size)
{
	/* Get a free node from the pool */
	struct htk_s *k = tree->pool;
	assert(k);
	tree->pool = tree->pool->next;

	/* Hash and append */
	tree->hash->begin(tree->hash);
	tree->hash->process(tree->hash, data, size);
	tree->hash->end(tree->hash, k->data);
	tree_append(tree, k);
}

static
void
hashtree_destroy(struct hash_s *tree)
{
	free(tree->state->basemem);
	free(tree->state);
}

static
void
hashtree_begin(struct hash_s *tree)
{
	tree->state->rll = 0;
	tree->state->block_index = 0;
	while (tree->state->first)
		unlink_key(tree->state, tree->state->first);
}

static
void
hashtree_end(struct hash_s *tree, unsigned char *raw_data)
{
	if (!tree->state->first || tree->state->block_index)
		run_block(tree->state, tree->state->block_data, tree->state->block_index);

	/* solve root hash and return */
	do {
		int solved_some = 0;
		struct htk_s *key = tree->state->first;
		while ((key) && (key->next) && (key->rank == key->next->rank)) {
			solved_some = 1;
			combine_discard(tree->state, key);
			key = key->next;
		}
		if (!solved_some)
			break;
	} while (1);

	/* reverse compact */
	while (tree->state->first != tree->state->last)
		combine_discard(tree->state, tree->state->last->prev);

	memcpy(raw_data, tree->state->first->data, tree->state->key_size);
}

static
void
hashtree_process(struct hash_s *tree, const unsigned char *data, size_t size)
{
	if (size && tree->state->block_index) {
		size_t cpy = tree->state->block_size - tree->state->block_index;
		if (cpy > size)
			cpy = size;
		memcpy
			(tree->state->block_data + tree->state->block_index
			,data
			,cpy
			);
		size -= cpy;
		data += cpy;
		tree->state->block_index += cpy;
		if (tree->state->block_index == tree->state->block_size) {
			run_block(tree->state, tree->state->block_data, tree->state->block_size);
			tree->state->block_index = 0;
		}
	}
	while (size >= tree->state->block_size) {
		run_block(tree->state, data, tree->state->block_size);
		data += tree->state->block_size;
		size -= tree->state->block_size;
	}
	if (size) {
		memcpy
			(tree->state->block_data
			,data
			,size
			);
		tree->state->block_index = size;
	}
}

/* 1)  0
 * 2)  1
 * 3)  1 0
 * 4)  2
 * 5)  2 0
 * 6)  2 1
 * 7)  2 1 0
 * 8)  3
 * 9)  3 0
 * 10) 3 1
 * 11) 3 1 0
 * 12) 3 2
 * 13) 3 2 0
 * 14) 3 2 1
 * 15) 3 2 1 0
 * 4
 * 4 0
 * 4 1
 * 4 1 0
 * 4 2
 * 4 2 0
 * 4 2 1
 * 4 2 1 0
 * 4 3
 * 4 3 0
 * 4 3 1
 * 4 3 1 0
 * 4 3 2
 * 4 3 2 0
 * 4 3 2 1
 * 4 3 2 1 0
 * 5
 *
 * 1-2   1 node
 * 3-6   2 nodes
 * 7-14  3 nodes
 * 15-30 4 nodes
 */

/* Returns the number of nodes required to process the given number of blocks.
 * Function will return UINT_MAX when max_blocks is UINT_MAX which is not
 * going to be what you want. */
unsigned req_nodes(unsigned max_blocks, unsigned max_storage_levels)
{
	unsigned p;
	max_blocks++;
	for (p = 0; max_blocks; p++) {
		max_blocks = max_blocks >> 1;
	}
	p--;
	p += (1u << max_storage_levels);
	return p;
}


static
unsigned
hashtree_query_digest_size(const struct hash_s *hash)
{
	return hash->state->hash->query_digest_size(hash->state->hash);
}

int
hashtree_create(struct hash_s *tree, struct hash_s *alg, size_t block_size, unsigned max_storage_levels)
{
	const unsigned key_bits = alg->query_digest_size(alg);
	const size_t key_size = key_bits / 8;
	struct hash_pvt_s *pvt = malloc(sizeof(*pvt) + block_size);
	const unsigned keys = req_nodes(UINT_MAX-1u, max_storage_levels) + 1;
	unsigned i;

	assert((key_bits & 7) == 0);

	if (!pvt) {
		return -1;
	}

	pvt->basemem = malloc((sizeof(struct htk_s) + key_size) * keys);
	if (!pvt->basemem) {
		free(pvt);
		return -2;
	}

	pvt->block_data = (unsigned char*)(pvt+1);

	pvt->pool = pvt->basemem;
	for (i = 1; i < keys; i++) {
		pvt->pool[i-1].next = &pvt->pool[i];
		pvt->pool[i-1].data = ((unsigned char*)(pvt->pool + keys)) + (i-1) * key_size;
	}
	pvt->pool[keys-1].next = NULL;

	pvt->first = NULL;
	pvt->rll = 0;
	pvt->last = NULL;
	pvt->key_size = key_size;
	pvt->hash = alg;
	pvt->depth_bits = max_storage_levels;
	pvt->block_size = block_size;
	pvt->block_index = 0;

	tree->state = pvt;
	tree->destroy = hashtree_destroy;
	tree->begin = hashtree_begin;
	tree->process = hashtree_process;
	tree->end = hashtree_end;
	tree->query_digest_size = hashtree_query_digest_size;

	return 0;
}


