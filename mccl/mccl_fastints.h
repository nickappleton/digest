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

#ifndef MCCL_FASTINTS_H_
#define MCCL_FASTINTS_H_

/* Supplies the following type:
 *   mccl_uif32 - fast minimum 32-bit width unsigned integer
 *   UIF32_MAX  - the maximum value the mccl_uif32 type can hold
 *   UIF32_SIZE - the size in chars of the mccl_uif32 type
 * May supply the following type:
 *   mccl_uif64 - fast minimum 64-bit width unsigned integer
 *   UIF32_MAX  - the maximum value the mccl_uif32 type can hold
 *   UIF32_SIZE - the size in chars of the mccl_uif32 type
 *
 * The UIFx_SIZE macros may not be defined if the size is not known.
 * The UIFx_MAX macros will always be defined if the type is defined (i.e.
 * the presence of the type can be determined by the existance of this
 * macro).
 */

#include <limits.h>

/* If this macro is set, prefer types with known sizes (in preference to fast
 * types with unknown sizes). This may be advantageous when converting buffers
 * of the types. */
#if 1
#  define PREFER_KNOWN_SIZE_TYPES (1)
#endif

/* Preprocessor-time bit counting thing. */
#define NUMBITS_2(x)   (((x) & 0x2u) ? 2u : 1u)                                                /* x 1..2 bits */
#define NUMBITS_4(x)   (((x) & 0xCu) ? (2u + NUMBITS_2((x) >> 2u)) : NUMBITS_2(x))             /* x 1..4 bits */
#define NUMBITS_8(x)   (((x) & 0xF0u) ? (4u + NUMBITS_4((x) >> 4u)) : NUMBITS_4(x))            /* x 1..8 bits */
#define NUMBITS_16(x)  (((x) & 0xFF00u) ? (8u + NUMBITS_8((x) >> 8u)) : NUMBITS_8(x))          /* x 1..16 bits */
#define NUMBITS_32(x)  (((x) & 0xFFFF0000u) ? (16u + NUMBITS_16((x) >> 16u)) : NUMBITS_16(x))  /* x 1..32 bits */
#define NUMBITS_64B(x) (((x) > 0xFFFFFFFFu) ? (32u + NUMBITS_32((x) >> 32u)) : NUMBITS_32(x))  /* x 1..64 bits */
#define NUMBITS_64A(x) (((x) > 0xFFFFFFFFu) ? (32u + NUMBITS_64B((x) >> 32u)) : NUMBITS_32(x)) /* x 1..96 bits */
#define NUMBITS(x)     (((x) > 0xFFFFFFFFu) ? (32u + NUMBITS_64A((x) >> 32u)) : NUMBITS_32(x)) /* x 1..128 bits */

#if __GNUC__

/* LP64 on GNUC means that longs are 64 bit and ints are 32 bit. */
#if (defined(__LP64__) || defined(_LP64))
#if !defined(UIF32_MAX)
typedef unsigned        mccl_uif32;
#define UIF32_MAX       UINT_MAX
#define UIF32_SIZE      __SIZEOF_INT__
#endif
#if !defined(UIF64_MAX)
typedef unsigned long   mccl_uif64;
#define UIF64_MAX       ULONG_MAX
#define UIF64_SIZE      __SIZEOF_LONG__
#endif
#endif

/* Use the GCC long long if we can get info on it */
#if !defined(UIF64_MAX) && __SIZEOF_LONG_LONG__ && defined(__UINT64_MAX__)
__extension__ typedef unsigned long long mccl_uif64;
#define UIF64_MAX       __UINT64_MAX__
#define UIF64_SIZE      __SIZEOF_LONG_LONG__
#endif

#endif /* __GNUC__ */

/* If we didn't define our types, fall back on the c99 types. */
#if defined(__STDC__) && (__STDC_VERSION__ >= 199901L) && (!defined(UIF32_MAX) || !defined(UIF64_MAX))

#include <stdint.h>

#if !defined(UIF32_MAX)
#if defined(UINT32_MAX) && PREFER_KNOWN_SIZE_TYPES
#if (NUMBITS(UINT32_MAX) != 32) || (NUMBITS(UINT32_MAX) % CHAR_BIT != 0)
#error sanity check failed - either your compiler is broken or I made some bad assumptions
#endif
typedef uint32_t        mccl_uif32;
#define UIF32_MAX       UINT32_MAX
#define UIF32_SIZE      (NUMBITS(UIF32_MAX) / CHAR_BIT)
#define UIF32_UNPADDED  (1)
#else
typedef uint_fast32_t   mccl_uif32;
#define UIF32_MAX       UINT_FAST32_MAX
#endif
#endif /* !defined(UIF32_MAX) */

#if !defined(UIF64_MAX)
#if defined(UINT64_MAX) && PREFER_KNOWN_SIZE_TYPES
#if (NUMBITS(UINT64_MAX) != 64) || (NUMBITS(UINT64_MAX) % CHAR_BIT != 0)
#error sanity check failed - either your compiler is broken or I made some bad assumptions
#endif
typedef uint64_t        mccl_uif64;
#define UIF64_MAX       UINT64_MAX
#define UIF64_SIZE      (NUMBITS(UIF64_MAX) / CHAR_BIT)
#define UIF64_UNPADDED  (1)
#else
typedef uint_fast64_t   mccl_uif64;
#define UIF64_MAX       UINT_FAST64_MAX
#endif
#endif /* !defined(UIF64_MAX) */

#endif /* C99 */

/* Fallback on long for 32 bit type if still not-existant */
#if !defined(UIF32_MAX)
/* This seems completely reasonable to prefer the integer type over the long
 * integer type. Flame me if I am wrong. */
#if NUMBITS(UINT_MAX) >= 32
typedef unsigned        mccl_uif32;
#define UIF32_MAX       UINT_MAX
#else
typedef unsigned long   mccl_uif32;
#define UIF32_MAX       ULONG_MAX
#endif
#endif

/* Say how many bits our types have */
#if !defined(UIF32_NUMBITS)
#define UIF32_NUMBITS  NUMBITS(UIF32_MAX)
#endif
#if defined(UIF64_MAX) && !defined(UIF64_NUMBITS)
#define UIF64_NUMBITS  NUMBITS(UIF64_MAX)
#endif

/* If we know the exact size of the type and we know it's maximum value, we
 * can probe to figure out if the native type contains any padding. Hack at
 * your own peril. */
#if !defined(UIF32_UNPADDED) && defined(UIF32_MAX) && defined(UIF32_SIZE) && (UIF32_NUMBITS == CHAR_BIT * UIF32_SIZE)
#define UIF32_UNPADDED (1)
#endif
#if !defined(UIF64_UNPADDED) && defined(UIF64_MAX) && defined(UIF64_SIZE) && (UIF64_NUMBITS == CHAR_BIT * UIF64_SIZE)
#define UIF64_UNPADDED (1)
#endif

#endif /* FASTINTS_H_ */

