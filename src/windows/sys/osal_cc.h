/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2017 rt-labs AB, Sweden.
 *
 * This software is licensed under the terms of the BSD 3-clause
 * license. See the file LICENSE distributed with this software for
 * full license information.
 ********************************************************************/

#ifndef CC_H
#define CC_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) || defined(__GNUG__)

#include <assert.h>

#define CC_PACKED_BEGIN
#define CC_PACKED_END
#define CC_PACKED __attribute__ ((packed))

#define CC_FORMAT(str, arg) __attribute__ ((format (printf, str, arg)))

#if BYTE_ORDER == LITTLE_ENDIAN
#define CC_TO_LE16(x)   ((uint16_t)(x))
#define CC_TO_LE32(x)   ((uint32_t)(x))
#define CC_TO_LE64(x)   ((uint64_t)(x))
#define CC_FROM_LE16(x) ((uint16_t)(x))
#define CC_FROM_LE32(x) ((uint32_t)(x))
#define CC_FROM_LE64(x) ((uint64_t)(x))
#define CC_TO_BE16(x)   ((uint16_t)__builtin_bswap16 (x))
#define CC_TO_BE32(x)   ((uint32_t)__builtin_bswap32 (x))
#define CC_TO_BE64(x)   ((uint64_t)__builtin_bswap64 (x))
#define CC_FROM_BE16(x) ((uint16_t)__builtin_bswap16 (x))
#define CC_FROM_BE32(x) ((uint32_t)__builtin_bswap32 (x))
#define CC_FROM_BE64(x) ((uint64_t)__builtin_bswap64 (x))
#else
#define CC_TO_LE16(x)   ((uint16_t)__builtin_bswap16 (x))
#define CC_TO_LE32(x)   ((uint32_t)__builtin_bswap32 (x))
#define CC_TO_LE64(x)   ((uint64_t)__builtin_bswap64 (x))
#define CC_FROM_LE16(x) ((uint16_t)__builtin_bswap16 (x))
#define CC_FROM_LE32(x) ((uint32_t)__builtin_bswap32 (x))
#define CC_FROM_LE64(x) ((uint64_t)__builtin_bswap64 (x))
#define CC_TO_BE16(x)   ((uint16_t)(x))
#define CC_TO_BE32(x)   ((uint32_t)(x))
#define CC_TO_BE64(x)   ((uint64_t)(x))
#define CC_FROM_BE16(x) ((uint16_t)(x))
#define CC_FROM_BE32(x) ((uint32_t)(x))
#define CC_FROM_BE64(x) ((uint64_t)(x))
#endif

#define CC_ATOMIC_GET8(p)  __atomic_load_n ((p), __ATOMIC_SEQ_CST)
#define CC_ATOMIC_GET16(p) __atomic_load_n ((p), __ATOMIC_SEQ_CST)
#define CC_ATOMIC_GET32(p) __atomic_load_n ((p), __ATOMIC_SEQ_CST)
#define CC_ATOMIC_GET64(p) __atomic_load_n ((p), __ATOMIC_SEQ_CST)

#define CC_ATOMIC_SET8(p, v)  __atomic_store_n ((p), (v), __ATOMIC_SEQ_CST)
#define CC_ATOMIC_SET16(p, v) __atomic_store_n ((p), (v), __ATOMIC_SEQ_CST)
#define CC_ATOMIC_SET32(p, v) __atomic_store_n ((p), (v), __ATOMIC_SEQ_CST)
#define CC_ATOMIC_SET64(p, v) __atomic_store_n ((p), (v), __ATOMIC_SEQ_CST)

#define CC_ASSERT(exp) cc_assert (exp)

#ifdef __cplusplus
#define CC_STATIC_ASSERT(exp) static_assert (exp, "")
#else
#define CC_STATIC_ASSERT(exp) _Static_assert(exp, "")
#endif

#define CC_UNUSED(var) (void)(var)

static inline void cc_assert (int exp)
{
   assert (exp); // LCOV_EXCL_LINE
}

#elif defined(_MSC_VER)

#include <assert.h>

#define CC_PACKED_BEGIN __pragma (pack (push, 1))
#define CC_PACKED_END   __pragma (pack (pop))
#define CC_PACKED

#define CC_FORMAT(str, arg)

#define CC_TO_LE16(x)   ((uint16_t)(x))
#define CC_TO_LE32(x)   ((uint32_t)(x))
#define CC_TO_LE64(x)   ((uint64_t)(x))
#define CC_FROM_LE16(x) ((uint16_t)(x))
#define CC_FROM_LE32(x) ((uint32_t)(x))
#define CC_FROM_LE64(x) ((uint64_t)(x))
#define CC_TO_BE16(x)   ((uint16_t)_byteswap_ushort (x))
#define CC_TO_BE32(x)   ((uint32_t)_byteswap_ulong (x))
#define CC_TO_BE64(x)   ((uint64_t)_byteswap_uint64 (x))
#define CC_FROM_BE16(x) ((uint16_t)_byteswap_ushort (x))
#define CC_FROM_BE32(x) ((uint32_t)_byteswap_ulong (x))
#define CC_FROM_BE64(x) ((uint64_t)_byteswap_uint64 (x))

/* TODO */
#define CC_ATOMIC_GET8(p)  (*p)
#define CC_ATOMIC_GET16(p) (*p)
#define CC_ATOMIC_GET32(p) (*p)
#define CC_ATOMIC_GET64(p) (*p)

/* TODO */
#define CC_ATOMIC_SET8(p, v)  ((*p) = (v))
#define CC_ATOMIC_SET16(p, v) ((*p) = (v))
#define CC_ATOMIC_SET32(p, v) ((*p) = (v))
#define CC_ATOMIC_SET64(p, v) ((*p) = (v))

static uint8_t __inline cc_ctz (uint32_t x)
{
   DWORD n = 0;
   _BitScanForward (&n, x);
   return (uint8_t)n;
}

#define __builtin_ctz(x) cc_ctz (x)

#define CC_ASSERT(exp)        assert (exp)
#define CC_STATIC_ASSERT(exp) static_assert ((exp), "")

#endif

#ifdef __cplusplus
}
#endif

#endif /* CC_H */
