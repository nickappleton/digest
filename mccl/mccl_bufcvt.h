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

#ifndef MCCL_BUFCVT_H_
#define MCCL_BUFCVT_H_

#include <limits.h>
#include "mccl_inline.h"
#include "mccl_endian.h"
#include "mccl_fastints.h"
#include "mccl_op_uint64.h"

/* Set this to force the slow but safe buffer conversions */
#if 0
#  define MCCL_BUFCVT_SAFE 1
#endif

#if UCHAR_MAX == 0xFFu
#define MASK_CHAR8(x) (x)
#else
#define MASK_CHAR8(x) ((x) & 0xFFu)
#endif

/* Needed for memcpy*/
#if !MCCL_BUFCVT_SAFE
#include <string.h>
#endif

static INLINE void bufcvt_le32_to_uif32(mccl_uif32 *ele, const unsigned char *data, unsigned nb_elements)
{
#if defined(UIF32_SIZE) && (UIF32_SIZE == 4) && MCCL_ENDIAN_LITTLE && !MCCL_BUFCVT_SAFE
	memcpy(ele, data, 4 * nb_elements);
#else
	unsigned i;
	for (i = 0; i < nb_elements; i++, data += 4) {
		ele[i] =                 MASK_CHAR8(data[3]);
		ele[i] = (ele[i] << 8) | MASK_CHAR8(data[2]);
		ele[i] = (ele[i] << 8) | MASK_CHAR8(data[1]);
		ele[i] = (ele[i] << 8) | MASK_CHAR8(data[0]);
	}
#endif
}

static INLINE void bufcvt_be32_to_uif32(mccl_uif32 *ele, const unsigned char *data, unsigned nb_elements)
{
#if defined(UIF32_SIZE) && (UIF32_SIZE == 4) && MCCL_ENDIAN_BIG && !MCCL_BUFCVT_SAFE
	memcpy(ele, data, 4 * nb_elements);
#else
	unsigned i;
	for (i = 0; i < nb_elements; i++, data += 4) {
		ele[i] =                 MASK_CHAR8(data[0]);
		ele[i] = (ele[i] << 8) | MASK_CHAR8(data[1]);
		ele[i] = (ele[i] << 8) | MASK_CHAR8(data[2]);
		ele[i] = (ele[i] << 8) | MASK_CHAR8(data[3]);
	}
#endif
}

static INLINE void bufcvt_le64_to_UINT64(UINT64 *ele, const unsigned char *data, unsigned nb_elements)
{
#if defined(UIA64_SIZE) && (UIA64_SIZE == 8) && MCCL_ENDIAN_LITTLE && !MCCL_BUFCVT_SAFE
	memcpy(ele, data, 8 * nb_elements);
#else
	unsigned i;
	for (i = 0; i < nb_elements; i++, data += 8) {
		mccl_uif32 h, l;
		h =            MASK_CHAR8(data[7]);
		h = (h << 8) | MASK_CHAR8(data[6]);
		h = (h << 8) | MASK_CHAR8(data[5]);
		h = (h << 8) | MASK_CHAR8(data[4]);
		l =            MASK_CHAR8(data[3]);
		l = (l << 8) | MASK_CHAR8(data[2]);
		l = (l << 8) | MASK_CHAR8(data[1]);
		l = (l << 8) | MASK_CHAR8(data[0]);
		ele[i] = UINT64_MAKE(h, l);
	}
#endif
}

static INLINE void bufcvt_be64_to_UINT64(UINT64 *ele, const unsigned char *data, unsigned nb_elements)
{
#if defined(UIA64_SIZE) && (UIA64_SIZE == 8) && MCCL_ENDIAN_BIG && !MCCL_BUFCVT_SAFE
	memcpy(ele, data, 8 * nb_elements);
#else
	unsigned i;
	for (i = 0; i < nb_elements; i++, data += 8) {
		mccl_uif32 h, l;
		h =            MASK_CHAR8(data[0]);
		h = (h << 8) | MASK_CHAR8(data[1]);
		h = (h << 8) | MASK_CHAR8(data[2]);
		h = (h << 8) | MASK_CHAR8(data[3]);
		l =            MASK_CHAR8(data[4]);
		l = (l << 8) | MASK_CHAR8(data[5]);
		l = (l << 8) | MASK_CHAR8(data[6]);
		l = (l << 8) | MASK_CHAR8(data[7]);
		ele[i] = UINT64_MAKE(h, l);
	}
#endif
}

#endif /* BUFCVT_H_ */
