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
 * This software is licensed under the terms of the BSD 3-clause
 * license. See the file LICENSE distributed with this software for
 * full license information.
 ********************************************************************/

#include "osal_log.h"

#include <stdarg.h>
#include <stdio.h>

void os_log (uint8_t type, const char * fmt, ...)
{
   va_list list;

   switch (LOG_LEVEL_GET (type))
   {
   case LOG_LEVEL_DEBUG:
      printf ("%10ld [DEBUG] ", xTaskGetTickCount());
      break;
   case LOG_LEVEL_INFO:
      printf ("%10ld [INFO ] ", xTaskGetTickCount());
      break;
   case LOG_LEVEL_WARNING:
      printf ("%10ld [WARN ] ", xTaskGetTickCount());
      break;
   case LOG_LEVEL_ERROR:
      printf ("%10ld [ERROR] ", xTaskGetTickCount());
      break;
   case LOG_LEVEL_FATAL:
      printf ("%10ld [FATAL] ", xTaskGetTickCount());
      break;
   default:
      break;
   }

   va_start (list, fmt);
   vprintf (fmt, list);
   va_end (list);
}
