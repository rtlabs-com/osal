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

#ifndef OSAL_LOG_H
#define OSAL_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "osal.h"

/* Log levels */
#define LOG_LEVEL_DEBUG   0x00
#define LOG_LEVEL_INFO    0x01
#define LOG_LEVEL_WARNING 0x02
#define LOG_LEVEL_ERROR   0x03
#define LOG_LEVEL_FATAL   0x04
#define LOG_LEVEL_MASK    0x07
#define LOG_LEVEL_GET(t)  (t & LOG_LEVEL_MASK)

/* Log states */
#define LOG_STATE_ON  0x80
#define LOG_STATE_OFF 0x00

#define LOG_ENABLED(type)                                                      \
   ((LOG_LEVEL_GET (type) >= LOG_LEVEL) && (type & LOG_STATE_ON))

/** Log a message if it is enabled */
#define LOG(type, ...)                                                         \
   do                                                                          \
   {                                                                           \
      if (LOG_ENABLED (type))                                                  \
      {                                                                        \
         os_log (type, __VA_ARGS__);                                           \
      }                                                                        \
   } while (0)

/** Log debug messages */
#define LOG_DEBUG(type, ...) LOG ((LOG_LEVEL_DEBUG | type), __VA_ARGS__)

/** Log informational messages */
#define LOG_INFO(type, ...) LOG ((LOG_LEVEL_INFO | type), __VA_ARGS__)

/** Log warning messages */
#define LOG_WARNING(type, ...) LOG ((LOG_LEVEL_WARNING | type), __VA_ARGS__)

/** Log error messages */
#define LOG_ERROR(type, ...) LOG ((LOG_LEVEL_ERROR | type), __VA_ARGS__)

/** Log fatal messages */
#define LOG_FATAL(type, ...) LOG ((LOG_LEVEL_FATAL | type), __VA_ARGS__)

#define LOG_DEBUG_ENABLED(type)   LOG_ENABLED (LOG_LEVEL_DEBUG | type)
#define LOG_INFO_ENABLED(type)    LOG_ENABLED (LOG_LEVEL_INFO | type)
#define LOG_WARNING_ENABLED(type) LOG_ENABLED (LOG_LEVEL_WARNING | type)
#define LOG_ERROR_ENABLED(type)   LOG_ENABLED (LOG_LEVEL_ERROR | type)
#define LOG_FATAL_ENABLED(type)   LOG_ENABLED (LOG_LEVEL_FATAL | type)

void os_log (uint8_t type, const char * fmt, ...) CC_FORMAT (2, 3);

#ifdef __cplusplus
}
#endif

#endif /* OSAL_LOG_H */
