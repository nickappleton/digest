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

#ifndef MCCL_OP_UINT64_H_
#define MCCL_OP_UINT64_H_

#include "mccl_endian.h"
#include "mccl_fastints.h"

/* Setting the following macro will cause the the UINT64 type to be hidden in
 * a structure to ensure that the operators have been used correctly. */
#if 0
#  define TYPE_DEBUG 1
#endif

/* Setting the following macro will enable the 32-bit only operations. This
 * is really only useful for testing. */
#if 0
#  define FORCE_32BIT 1
#endif

/* Can we pun the 64 bit type as a LE sequence of chars? */
#if FORCE_32BIT || !defined(UIF64_MAX)

#include "mccl_inline.h"

#if UIF32_SIZE
#define UIA64_SIZE (2 * UIF32_SIZE)
#endif

typedef struct {
	mccl_uif32 data[2];
} UINT64;

#if MCCL_ENDIAN_LITTLE
#define UINT64_INIT(high, low) { {low, high} }
#define UINT64_H(a) ((a).data[1])
#define UINT64_L(a) ((a).data[0])
#else
#define UINT64_INIT(high, low) { {high, low} }
#define UINT64_H(a) ((a).data[0])
#define UINT64_L(a) ((a).data[1])
#endif

#define UINT64_MAKE(high, low) makeu64(high, low)
#define UINT64_XOR(a, b)       makeu64(UINT64_H(a) ^ UINT64_H(b), UINT64_L(a) ^ UINT64_L(b))
#define UINT64_OR(a, b)        makeu64(UINT64_H(a) | UINT64_H(b), UINT64_L(a) | UINT64_L(b))
#define UINT64_AND(a, b)       makeu64(UINT64_H(a) & UINT64_H(b), UINT64_L(a) & UINT64_L(b))
#define UINT64_COMP(a)         makeu64(~UINT64_H(a), ~UINT64_L(a))
#define UINT64_ADD(a, b)       addu64(a, b)
#define UINT64_SUB(a, b)       addu64(a, twos64(b))
#define UINT64_SHL(a, b)       shlu64(a, b)
#define UINT64_SHR(a, b)       shru64(a, b)
#define UINT64_MUL64x16(a, b)  mulu64x16(a, b)
#define UINT64_LOW(a)          UINT64_L(a)
#define UINT64_HIGH(a)         UINT64_H(a)

static INLINE UINT64 makeu64(mccl_uif32 high, mccl_uif32 low)
{
	UINT64 ret;
	UINT64_H(ret) = high;
	UINT64_L(ret) = low;
	return ret;
}
static INLINE UINT64 addu64(UINT64 a, UINT64 b)
{
	UINT64 ret;
	UINT64_L(ret) = 0xFFFFFFFFu & (UINT64_L(a) + UINT64_L(b));
	UINT64_H(ret) = 0xFFFFFFFFu & (UINT64_H(a) + UINT64_H(b) + (((UINT64_L(ret) < UINT64_L(a)) || (UINT64_L(ret) < UINT64_L(b))) ? 1u : 0u));
	return ret;
}
static INLINE UINT64 twos64(UINT64 a)
{
	UINT64 ret;
	UINT64_L(ret) = 0xFFFFFFFFu & ((~UINT64_L(a)) + 1u);
	UINT64_H(ret) = 0xFFFFFFFFu & ((~UINT64_H(a)) + !UINT64_L(ret));
	return ret;
}
static INLINE UINT64 shlu64(UINT64 a, unsigned b)
{
	if (b /* necessary to avoid undefined behavior */) {
		UINT64 ret;
		UINT64_H(ret) = 0xFFFFFFFFu & ((b >= 32) ? (UINT64_L(a) << (b-32)) : ((UINT64_L(a) >> (32-b)) | (UINT64_H(a) << b)));
		UINT64_L(ret) = 0xFFFFFFFFu & ((b >= 32) ? 0                       :  (UINT64_L(a) << b));
		return ret;
	}
	return a;
}
static INLINE UINT64 shru64(UINT64 a, unsigned b)
{
	if (b /* necessary to avoid undefined behavior */) {
		UINT64 ret;
		UINT64_H(ret) = (b >= 32) ? 0                       :   (UINT64_H(a) >> b);
		UINT64_L(ret) = (b >= 32) ? (UINT64_H(a) >> (b-32)) : (((UINT64_H(a) << (32-b)) | (UINT64_L(a) >> b)) & 0xFFFFFFFFu);
		return ret;
	}
	return a;
}
static INLINE UINT64 mulu64x16(UINT64 a, unsigned b)
{
	UINT64 ret;
	UINT64 lap;
	UINT64_H(ret) = UINT64_H(a) * b;
	UINT64_L(ret) = (UINT64_L(a) & 0xFFFFu) * b;
	UINT64_L(lap) = (UINT64_L(a) >> 16) * b;
	UINT64_H(lap) = UINT64_L(lap) >> 16;
	UINT64_L(lap) = (UINT64_L(lap) & 0xFFFFu) << 16;
	return addu64(ret, lap);
}

#elif TYPE_DEBUG

#include "mccl_inline.h"

#if UIF64_SIZE
#define UIA64_SIZE UIF64_SIZE
#endif

typedef struct { mccl_uif64 val; } UINT64;

#define UINT64_INIT(high, low) { ((((mccl_uif64)((high) & 0xFFFFFFFFu)) << 32u) | (mccl_uif64)((low) & 0xFFFFFFFFu)) }
#define UINT64_MAKE(high, low) composeu64(high, low)
#define UINT64_MUL64x16(a, b)  makeu64((a).val * (b))
#define UINT64_SHL(a, b)       makeu64((a).val << (b))
#define UINT64_SHR(a, b)       makeu64((a).val >> (b))
#define UINT64_ADD(a, b)       makeu64((a).val + (b).val)
#define UINT64_SUB(a, b)       makeu64((a).val - (b).val)
#define UINT64_XOR(a, b)       makeu64((a).val ^ (b).val)
#define UINT64_OR(a, b)        makeu64((a).val | (b).val)
#define UINT64_AND(a, b)       makeu64((a).val & (b).val)
#define UINT64_COMP(a)         makeu64(~(a).val)
#define UINT64_LOW(a)          ((mccl_uif32)(((a).val) & 0xFFFFFFFFu))
#define UINT64_HIGH(a)         ((mccl_uif32)(((a).val >> 32u) & 0xFFFFFFFFu))

static INLINE UINT64 composeu64(mccl_uif32 high, mccl_uif32 low)
{
	UINT64 ret;
	ret.val = (((mccl_uif64)((high) & 0xFFFFFFFFu)) << 32u) | (mccl_uif64)((low) & 0xFFFFFFFFu);
	return ret;
}
static INLINE UINT64 makeu64(mccl_uif64 val)
{
	UINT64 ret;
	ret.val = val;
	return ret;
}

#else /* TYPE_DEBUG */

#if UIF64_SIZE
#define UIA64_SIZE UIF64_SIZE
#endif

typedef mccl_uif64 UINT64;

#define UINT64_INIT(high, low)  ((((UINT64)((high) & 0xFFFFFFFFu)) << 32u) | (UINT64)((low) & 0xFFFFFFFFu))
#define UINT64_MAKE(high, low)  ((((UINT64)((high) & 0xFFFFFFFFu)) << 32u) | (UINT64)((low) & 0xFFFFFFFFu))
#define UINT64_MUL64x16(a, b)   (((UINT64)(a)) * (b))
#define UINT64_SHL(a, b)        (((UINT64)(a)) << (b))
#define UINT64_SHR(a, b)        (((UINT64)(a)) >> (b))
#define UINT64_ADD(a, b)        (((UINT64)(a)) + (b))
#define UINT64_SUB(a, b)        (((UINT64)(a)) - (b))
#define UINT64_XOR(a, b)        (((UINT64)(a)) ^ (b))
#define UINT64_OR(a, b)         (((UINT64)(a)) | (b))
#define UINT64_AND(a, b)        (((UINT64)(a)) & (b))
#define UINT64_COMP(a)          (~((UINT64)(a)))
#define UINT64_LOW(a)           ((mccl_uif32)(((UINT64)(a)) & 0xFFFFFFFFu))
#define UINT64_HIGH(a)          ((mccl_uif32)((((UINT64)(a)) >> 32u) & 0xFFFFFFFFu))

#endif /* TYPE_DEBUG */

#ifndef UINT64_ROR
#define UINT64_ROR(a, b) UINT64_OR(UINT64_SHR(a, (b) & 63u), UINT64_SHL(a, (64u - (b)) & 63u))
#endif

#endif /* OP_UINT64_H_ */
