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
#define UIF32_SIZE      __SIZEOF_LONG_LONG__
#endif

#endif /* __GNUC__ */

#if defined(__STDC__) && (__STDC_VERSION__ >= 199901L) && (!defined(UIF32_MAX) || !defined(UIF64_MAX))

#include <stdint.h>

#if !defined(UIF32_MAX)
typedef uint_fast32_t   mccl_uif32;
#define UIF32_MAX       UINT_FAST32_MAX
#endif

#if !defined(UIF64_MAX)
typedef uint_fast64_t   mccl_uif64;
#define UIF64_MAX       UINT_FAST64_MAX
#endif

#endif /* C99 */

/* Fallback on long for 32 bit type if still not-existant */
#if !defined(UIF32_MAX)
typedef unsigned long   mccl_uif32;
#define UIF32_MAX       ULONG_MAX
#endif

#endif /* FASTINTS_H_ */
