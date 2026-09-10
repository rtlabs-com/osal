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

#include "osal.h"
#include "osal_sys.h"

#include <zephyr/kernel.h>

static k_timeout_t os_to_timeout (uint32_t time)
{
   return (time == OS_WAIT_FOREVER) ? K_FOREVER : K_MSEC (time);
}

void * os_malloc (size_t size)
{
   return k_malloc (size);
}

void os_free (void * ptr)
{
   k_free (ptr);
}

static void os_thread_trampoline (void * p1, void * p2, void * p3)
{
   void (*entry) (void * arg) = p1;

   ARG_UNUSED (p3);
   entry (p2);
}

os_thread_t * os_thread_create (
   const char * name,
   uint32_t priority,
   size_t stacksize,
   void (*entry) (void * arg),
   void * arg)
{
   os_thread_t * thread;
   k_tid_t tid;

   thread = k_malloc (sizeof (*thread));
   CC_ASSERT (thread != NULL);

   thread->stacksize = stacksize;
   thread->stack     = k_thread_stack_alloc (stacksize, 0);
   CC_ASSERT (thread->stack != NULL);

   tid = k_thread_create (
      &thread->thread,
      thread->stack,
      stacksize,
      os_thread_trampoline,
      (void *)entry,
      arg,
      NULL,
      priority,
      0,
      K_NO_WAIT);
   CC_ASSERT (tid != NULL);

   k_thread_name_set (tid, name);

   return thread;
}

os_mutex_t * os_mutex_create (void)
{
   os_mutex_t * mutex;

   mutex = k_malloc (sizeof (*mutex));
   CC_ASSERT (mutex != NULL);

   k_mutex_init (&mutex->mutex);

   return mutex;
}

void os_mutex_lock (os_mutex_t * mutex)
{
   k_mutex_lock (&mutex->mutex, K_FOREVER);
}

void os_mutex_unlock (os_mutex_t * mutex)
{
   k_mutex_unlock (&mutex->mutex);
}

void os_mutex_destroy (os_mutex_t * mutex)
{
   k_free (mutex);
}

void os_usleep (uint32_t us)
{
   k_sleep (K_USEC (us));
}

uint32_t os_get_current_time_us (void)
{
   return (uint32_t)k_ticks_to_us_floor64 (k_uptime_ticks());
}

os_tick_t os_tick_current (void)
{
   return (os_tick_t)k_uptime_ticks();
}

os_tick_t os_tick_from_us (uint32_t us)
{
   return (os_tick_t)k_us_to_ticks_ceil64 (us);
}

void os_tick_sleep (os_tick_t tick)
{
   k_sleep (K_TICKS ((k_ticks_t)tick));
}

os_sem_t * os_sem_create (size_t count)
{
   os_sem_t * sem;

   sem = k_malloc (sizeof (*sem));
   CC_ASSERT (sem != NULL);

   k_sem_init (&sem->sem, count, K_SEM_MAX_LIMIT);

   return sem;
}

bool os_sem_wait (os_sem_t * sem, uint32_t time)
{
   return k_sem_take (&sem->sem, os_to_timeout (time)) != 0;
}

void os_sem_signal (os_sem_t * sem)
{
   k_sem_give (&sem->sem);
}

void os_sem_destroy (os_sem_t * sem)
{
   k_free (sem);
}

os_event_t * os_event_create (void)
{
   os_event_t * event;

   event = k_malloc (sizeof (*event));
   CC_ASSERT (event != NULL);

   k_event_init (&event->event);

   return event;
}

bool os_event_wait (os_event_t * event, uint32_t mask, uint32_t * value, uint32_t time)
{
   *value = k_event_wait (&event->event, mask, false, os_to_timeout (time));
   return *value == 0;
}

void os_event_set (os_event_t * event, uint32_t value)
{
   k_event_post (&event->event, value);
}

void os_event_clr (os_event_t * event, uint32_t value)
{
   k_event_clear (&event->event, value);
}

void os_event_destroy (os_event_t * event)
{
   k_free (event);
}

os_mbox_t * os_mbox_create (size_t size)
{
   os_mbox_t * mbox;
   void ** buffer;

   mbox = k_malloc (sizeof (*mbox));
   CC_ASSERT (mbox != NULL);

   buffer = k_malloc (size * sizeof (void *));
   CC_ASSERT (buffer != NULL);

   k_msgq_init (&mbox->msgq, (char *)buffer, sizeof (void *), size);
   mbox->buffer = buffer;

   return mbox;
}

bool os_mbox_fetch (os_mbox_t * mbox, void ** msg, uint32_t time)
{
   return k_msgq_get (&mbox->msgq, msg, os_to_timeout (time)) != 0;
}

bool os_mbox_post (os_mbox_t * mbox, void * msg, uint32_t time)
{
   return k_msgq_put (&mbox->msgq, &msg, os_to_timeout (time)) != 0;
}

void os_mbox_destroy (os_mbox_t * mbox)
{
   k_msgq_purge (&mbox->msgq);
   k_free (mbox->buffer);
   k_free (mbox);
}

static void os_timer_expiry (struct k_timer * ztimer)
{
   os_timer_t * timer = k_timer_user_data_get (ztimer);

   if (timer->fn)
   {
      timer->fn (timer, timer->arg);
   }
}

os_timer_t * os_timer_create (
   uint32_t us,
   void (*fn) (os_timer_t *, void * arg),
   void * arg,
   bool oneshot)
{
   os_timer_t * timer;

   timer = k_malloc (sizeof (*timer));
   CC_ASSERT (timer != NULL);

   timer->fn      = fn;
   timer->arg     = arg;
   timer->us      = us;
   timer->oneshot = oneshot;

   k_timer_init (&timer->timer, os_timer_expiry, NULL);
   k_timer_user_data_set (&timer->timer, timer);

   return timer;
}

void os_timer_set (os_timer_t * timer, uint32_t us)
{
   timer->us = us;
}

void os_timer_start (os_timer_t * timer)
{
   k_timer_start (
      &timer->timer,
      K_USEC (timer->us),
      timer->oneshot ? K_NO_WAIT : K_USEC (timer->us));
}

void os_timer_stop (os_timer_t * timer)
{
   k_timer_stop (&timer->timer);
}

void os_timer_destroy (os_timer_t * timer)
{
   k_timer_stop (&timer->timer);
   k_free (timer);
}
