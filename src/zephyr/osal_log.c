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

#include <zephyr/logging/log.h>
#include <stdarg.h>
#include <stdio.h>

LOG_MODULE_REGISTER (osal, LOG_LEVEL_DBG);

void os_log_impl (uint8_t type, const char * fmt, ...)
{
   va_list list;
   char msg[128];

   va_start (list, fmt);
   vsnprintf (msg, sizeof (msg), fmt, list);
   va_end (list);

   switch (LOG_LEVEL_GET (type))
   {
   case LOG_LEVEL_DEBUG:
      LOG_DBG ("%s", msg);
      break;
   case LOG_LEVEL_INFO:
      LOG_INF ("%s", msg);
      break;
   case LOG_LEVEL_WARNING:
      LOG_WRN ("%s", msg);
      break;
   case LOG_LEVEL_ERROR:
   case LOG_LEVEL_FATAL:
      LOG_ERR ("%s", msg);
      break;
   default:
      break;
   }
}

os_log_t os_log = os_log_impl;
