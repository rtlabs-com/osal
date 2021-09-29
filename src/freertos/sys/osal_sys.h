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

#ifndef OSAL_SYS_H
#define OSAL_SYS_H

#include <FreeRTOS.h>
#include <semphr.h>
#include <event_groups.h>
#include <queue.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OS_MAIN extern "C" int _main

#define OS_THREAD
#define OS_MUTEX
#define OS_SEM
#define OS_EVENT
#define OS_MBOX
#define OS_TIMER

typedef SemaphoreHandle_t os_mutex_t;
typedef TaskHandle_t os_thread_t;
typedef SemaphoreHandle_t os_sem_t;
typedef EventGroupHandle_t  os_event_t;
typedef QueueHandle_t os_mbox_t;

typedef struct os_timer
{
   TimerHandle_t handle;
   void(*fn) (struct os_timer *, void * arg);
   void * arg;
   uint32_t us;
} os_timer_t;

#ifdef __cplusplus
}
#endif

#endif /* OSAL_SYS_H */
