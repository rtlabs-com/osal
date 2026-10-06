/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2026 rt-labs AB, Sweden.
 *
 * This software is licensed under the terms of the BSD 3-clause
 * license. See the file LICENSE distributed with this software for
 * full license information.
 ********************************************************************/

#ifndef OSAL_SYS_H
#define OSAL_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "osal.h"

#include <dispatch/dispatch.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>

struct os_thread {
   pthread_t thread;
};

struct os_mutex {
   pthread_mutex_t mutex;
};

struct os_sem
{
   pthread_cond_t cond;
   pthread_mutex_t mutex;
   size_t count;
};

struct os_event
{
   pthread_cond_t cond;
   pthread_mutex_t mutex;
   uint32_t flags;
};

struct os_mbox
{
   pthread_cond_t cond;
   pthread_mutex_t mutex;
   size_t r;
   size_t w;
   size_t count;
   size_t size;
   void * msg[];
};

struct os_timer
{
   dispatch_queue_t queue;
   dispatch_source_t source;
   void (*fn) (struct os_timer *, void * arg);
   void * arg;
   uint32_t us;
   bool oneshot;
};

#ifdef __cplusplus
}
#endif

#endif /* OSAL_SYS_H */
