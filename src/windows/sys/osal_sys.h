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

#include <windows.h>

#define OS_WAIT_FOREVER INFINITE

#define OS_THREAD
#define OS_MUTEX
#define OS_SEM
#define OS_EVENT
#define OS_MBOX
#define OS_TIMER

typedef HANDLE os_thread_t;
typedef CRITICAL_SECTION os_mutex_t;

typedef struct os_sem
{
   CONDITION_VARIABLE condition;
   CRITICAL_SECTION lock;
   size_t count;
} os_sem_t;

typedef struct os_event
{
   CONDITION_VARIABLE condition;
   CRITICAL_SECTION lock;
   uint32_t flags;
} os_event_t;

typedef struct os_mbox
{
   CONDITION_VARIABLE condition;
   CRITICAL_SECTION lock;
   size_t r;
   size_t w;
   size_t count;
   size_t size;
   void * msg[];
} os_mbox_t;

typedef struct os_timer
{
   UINT timerID;
   void (*fn) (struct os_timer *, void * arg);
   void * arg;
   uint32_t us;
   bool oneshot;
} os_timer_t;

#ifdef __cplusplus
}
#endif

#endif /* OSAL_SYS_H */
