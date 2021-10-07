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

#include <assert.h>

#if defined(__clang__)
#if !defined(CLANG_ANALYZER_NORETURN)
#if __has_feature(attribute_analyzer_noreturn)
#define CLANG_ANALYZER_NORETURN __attribute__ ((analyzer_noreturn))
#else
#define CLANG_ANALYZER_NORETURN
#endif
#endif
#else
#define CLANG_ANALYZER_NORETURN
#endif

static inline void cc_assert (int exp) CLANG_ANALYZER_NORETURN
{
   assert (exp); // LCOV_EXCL_LINE
}

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

#ifdef __cplusplus
}
#endif

#endif /* CC_H */
