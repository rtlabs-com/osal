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

#define WIN32_LEAN_AND_MEAN /* Do not include <winsock.h> */
#include <windows.h>

/* Libraries excluded by WIN32_LEAN_AND_MEAN */
#include <malloc.h>
#include <timeapi.h>
#include <mmsystem.h>

#define OS_WAIT_FOREVER INFINITE

#define OS_THREAD
#define OS_MUTEX
#define OS_SEM
#define OS_EVENT
#define OS_MBOX
#define OS_TIMER
#define OS_TICK

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
   HANDLE timer;
   void (*fn) (struct os_timer *, void * arg);
   void * arg;
   LARGE_INTEGER time;
   bool oneshot;
   bool run;
} os_timer_t;

typedef uint64_t os_tick_t;

#ifdef __cplusplus
}
#endif

#endif /* OSAL_SYS_H */
