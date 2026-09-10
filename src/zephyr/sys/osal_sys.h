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

#ifndef OSAL_SYS_H
#define OSAL_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/kernel.h>

typedef struct os_thread
{
   struct k_thread thread;
   k_thread_stack_t * stack;
   size_t stacksize;
} os_thread_t;

typedef struct os_mutex
{
   struct k_mutex mutex;
} os_mutex_t;

typedef struct os_sem
{
   struct k_sem sem;
} os_sem_t;

typedef struct os_event
{
   struct k_event event;
} os_event_t;

typedef struct os_mbox
{
   struct k_msgq msgq;
   void ** buffer;
} os_mbox_t;

typedef struct os_timer
{
   struct k_timer timer;
   void (*fn) (struct os_timer *, void * arg);
   void * arg;
   uint32_t us;
   bool oneshot;
} os_timer_t;

#ifdef __cplusplus
}
#endif

#endif /* OSAL_SYS_H */
