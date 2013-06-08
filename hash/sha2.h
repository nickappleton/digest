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

#ifndef SHA2_H_
#define SHA2_H_

#include "hash.h"

/* Creates a SHA-2 context with the length specified by digest_bits. The value
 * of digest_bits can be anywhere from 1 to 512. Values outside this range
 * produce undefined results.
 *
 * |   digest_bits   | force_512 |  NIST SHA-2  |
 * | 224             | 0         | SHA-224      |
 * | 224             | 1         | SHA-512/224  |
 * | 256             | 0         | SHA-256      |
 * | 256             | 1         | SHA-512/256  |
 * | 384             | X         | SHA-384      |
 * | 512             | X         | SHA-512      |
 *
 * For all other values of digest bits, the value of force_512 is ignored and
 * the resulting digest will be computed as per SHA-512/t as proposed here:
 *   http://eprint.iacr.org/2010/548.pdf */
int sha2_create(struct hash_s *hash, unsigned digest_bits, int force_512);

#endif /* SHA2_H_ */
