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

#ifndef OSAL_H
#define OSAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "sys/osal_cc.h"

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef BIT
#define BIT(n) (1U << (n))
#endif

#ifndef NELEMENTS
#define NELEMENTS(a) (sizeof (a) / sizeof ((a)[0]))
#endif

#define OS_WAIT_FOREVER 0xFFFFFFFF

#ifndef OS_MAIN
#define OS_MAIN int main
#endif

typedef struct os_mutex os_mutex_t;
typedef struct os_sem os_sem_t;
typedef struct os_thread os_thread_t;
typedef struct os_event os_event_t;
typedef struct os_mbox os_mbox_t;
typedef struct os_timer os_timer_t;
typedef uint64_t os_tick_t;

void * os_malloc (size_t size);
void os_free (void * ptr);

void os_usleep (uint32_t us);
uint32_t os_get_current_time_us (void);

os_tick_t os_tick_current (void);
os_tick_t os_tick_from_us (uint32_t us);
void      os_tick_sleep (os_tick_t tick);

os_thread_t * os_thread_create (
   const char * name,
   uint32_t priority,
   size_t stacksize,
   void (*entry) (void * arg),
   void * arg);

os_mutex_t * os_mutex_create (void);
void os_mutex_lock (os_mutex_t * mutex);
void os_mutex_unlock (os_mutex_t * mutex);
void os_mutex_destroy (os_mutex_t * mutex);

os_sem_t * os_sem_create (size_t count);
bool os_sem_wait (os_sem_t * sem, uint32_t time);
void os_sem_signal (os_sem_t * sem);
void os_sem_destroy (os_sem_t * sem);

os_event_t * os_event_create (void);
bool os_event_wait (
   os_event_t * event,
   uint32_t mask,
   uint32_t * value,
   uint32_t time);
void os_event_set (os_event_t * event, uint32_t value);
void os_event_clr (os_event_t * event, uint32_t value);
void os_event_destroy (os_event_t * event);

os_mbox_t * os_mbox_create (size_t size);
bool os_mbox_fetch (os_mbox_t * mbox, void ** msg, uint32_t time);
bool os_mbox_post (os_mbox_t * mbox, void * msg, uint32_t time);
void os_mbox_destroy (os_mbox_t * mbox);

os_timer_t * os_timer_create (
   uint32_t us,
   void (*fn) (os_timer_t * timer, void * arg),
   void * arg,
   bool oneshot);
void os_timer_set (os_timer_t * timer, uint32_t us);
void os_timer_start (os_timer_t * timer);
void os_timer_stop (os_timer_t * timer);
void os_timer_destroy (os_timer_t * timer);

#ifdef __cplusplus
}
#endif

#endif /* OSAL_H */
