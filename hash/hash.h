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

#ifndef HASH_H_
#define HASH_H_

#include <stddef.h>

/* Using this API:
 *
 * A hash object can be in one of two states: the "uninitialised state" and
 * the "initialised state". When a hash object is in the uninitialised state,
 * the only valid object calls are:
 *
 *     begin(), destroy() or query_digest_size()
 *
 * When in the initialised state, the following functions become available
 * in addition to all of the previous functions:
 *
 *     process() and end()
 *
 * Calling functions available in the initialised state on a hash object which
 * is in the uninitialised state is completely undefined and could cause your
 * computer to become self-aware.
 *
 * A newly instantiated hash object is ALWAYS in the uninitialised state.
 *
 * query_digest_size() returns the number of BITS the digest will supply. This
 * dictates the minimum size of the buffer required to be passed to the end()
 * call as: (n+7)/8.
 *
 * The begin() function can be called at ANY time to reset the state and place
 * the hash object in the initialised state. begin() is the only function
 * which places the hash object in the initialised state.
 *
 * process() feeds the hash function with a given array of octets. It must be
 * called while in the initialised state. Using process in the uninitialised
 * state is completely undefined. process() need not be called for the case of
 * hashing zero octets. i.e. calling begin() followed by end() is a valid use-
 * case of a hash object.
 *
 * The end() function completes the digest computation, stores the result in
 * the "result" argument and returns the hash object to the uninitialised
 * state. The result buffer should be at least large enough to contain the
 * number of bits returned by query_digest_size() where each character will
 * always contain an octet. Bits in the resultant buffer should always be
 * left aligned (i.e. for a single bit digest, the result should be in the
 * MSB of octet[0]).
 *
 * For clarification: because end() returns the hash object to the
 * uninitialised state, it can not be called again to retrieve the computed
 * hash.
 *
 * The destroy() function is responsible for cleaning up any memory which was
 * allocated during the creation of the hash function (i.e. the hash.state
 * member). destroy() can be called at any time and has no obligation to
 * change or nullify any members of the hash object passed to it.
 * Once destroy() has been called, using any of the hash object API is
 * undefined behaviour. */

struct hash_pvt_s;

struct hash_s {
	struct hash_pvt_s *state;

	void        (*begin)(struct hash_s *hash);
	void        (*process)(struct hash_s *hash, const unsigned char *data, size_t size);
	void        (*end)(struct hash_s *hash, unsigned char *result);
	void        (*destroy)(struct hash_s *hash);
	unsigned    (*query_digest_size)(const struct hash_s *hash);
};


#endif /* HASH_H_ */
