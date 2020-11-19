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

#include "osal_log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void os_log (uint8_t type, const char * fmt, ...)
{
   va_list list;

   switch (LOG_LEVEL_GET (type))
   {
   case LOG_LEVEL_DEBUG:
      printf ("[DEBUG] ");
      break;
   case LOG_LEVEL_INFO:
      printf ("[INFO ] ");
      break;
   case LOG_LEVEL_WARNING:
      printf ("[WARN ] ");
      break;
   case LOG_LEVEL_ERROR:
      printf ("[ERROR] ");
      break;
   case LOG_LEVEL_FATAL:
      printf ("[FATAL] ");
      break;
   default:
      break;
   }

   va_start (list, fmt);
   vprintf (fmt, list);
   va_end (list);
   fflush (stdout);
}
