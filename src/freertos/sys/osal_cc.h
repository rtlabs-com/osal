/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2021 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/

#ifndef CC_H
#define CC_H

#include <assert.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define CC_PACKED_BEGIN
#define CC_PACKED_END
#define CC_PACKED             __attribute__((packed))

#define CC_FORMAT(str,arg)    __attribute__((format (printf, str, arg)))


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

#define CC_ATOMIC_GET8(p)     (*p)
#define CC_ATOMIC_GET16(p)    (*p)
#define CC_ATOMIC_GET32(p)    (*p)
#define CC_ATOMIC_GET64(p)                      \
({                                              \
   uint64_t v;                                  \
   /*int_lock();*/                              \
   v = *p;                                      \
   /*int_unlock();*/                            \
   v;                                           \
})

#define CC_ATOMIC_SET8(p, v)  ((*p) = (v))
#define CC_ATOMIC_SET16(p, v) ((*p) = (v))
#define CC_ATOMIC_SET32(p, v) ((*p) = (v))
#define CC_ATOMIC_SET64(p, v)                   \
({                                              \
   /*int_lock();*/                              \
   *p = v;                                      \
   /*int_unlock();*/                            \
})

#define CC_ASSERT(exp) assert (exp)
#define CC_STATIC_ASSERT(exp) _Static_assert (exp, "")

#ifdef __cplusplus
}
#endif

#endif /* CC_H */
