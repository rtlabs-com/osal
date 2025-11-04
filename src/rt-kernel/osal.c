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
#include "sys/osal_sys.h"

void * os_malloc (size_t size)
{
   return malloc (size);
}

void os_free (void * ptr)
{
   free (ptr);
}

os_thread_t * os_thread_create (
   const char * name,
   uint32_t priority,
   size_t stacksize,
   void (*entry) (void * arg),
   void * arg)
{
   return (os_thread_t *)task_spawn (name, entry, priority, stacksize, arg);
}

os_mutex_t * os_mutex_create (void)
{
   return (os_mutex_t *)mtx_create();
}

void os_mutex_lock (os_mutex_t * mutex)
{
   mtx_lock ((mtx_t *)mutex);
}

void os_mutex_unlock (os_mutex_t * mutex)
{
   mtx_unlock ((mtx_t *)mutex);
}

void os_mutex_destroy (os_mutex_t * mutex)
{
   mtx_destroy ((mtx_t *)mutex);
}

void os_usleep (uint32_t us)
{
   task_delay (tick_from_ms (us / 1000));
}

uint32_t os_get_current_time_us (void)
{
   return 1000 * tick_to_ms (tick_get());
}

os_tick_t os_tick_current (void)
{
   return tick_get();
}

os_tick_t os_tick_from_us (uint32_t us)
{
   return tick_from_ms (us / 1000);
}

void os_tick_sleep (os_tick_t tick)
{
   task_delay (tick);
}

os_sem_t * os_sem_create (size_t count)
{
   return (os_sem_t *)sem_create (count);
}

bool os_sem_wait (os_sem_t * sem_, uint32_t time)
{
   int tmo = 0;
   sem_t * sem = (sem_t *)sem_;

   if (time != OS_WAIT_FOREVER)
   {
      tmo = sem_wait_tmo (sem, tick_from_ms (time));
   }
   else
   {
      sem_wait (sem);
   }

   return tmo;
}

void os_sem_signal (os_sem_t * sem_)
{
   sem_t * sem = (sem_t *)sem_;
   sem_signal (sem);
}

void os_sem_destroy (os_sem_t * sem_)
{
   sem_t * sem = (sem_t *)sem_;
   sem_destroy (sem);
}

os_event_t * os_event_create (void)
{
   return (os_event_t *)flags_create (0);
}

bool os_event_wait (os_event_t * event_, uint32_t mask, uint32_t * value, uint32_t time)
{
   int tmo = 0;
   flags_t * event = (flags_t *)event_;

   if (time != OS_WAIT_FOREVER)
   {
      tmo = flags_wait_any_tmo (event, mask, tick_from_ms (time), value);
   }
   else
   {
      flags_wait_any (event, mask, value);
   }

   *value = *value & mask;
   return tmo;
}

void os_event_set (os_event_t * event_, uint32_t value)
{
   flags_t * event = (flags_t *)event_;
   flags_set (event, value);
}

void os_event_clr (os_event_t * event_, uint32_t value)
{
   flags_t * event = (flags_t *)event_;
   flags_clr (event, value);
}

void os_event_destroy (os_event_t * event_)
{
   flags_t * event = (flags_t *)event_;
   flags_destroy (event);
}

os_mbox_t * os_mbox_create (size_t size)
{
   return (os_mbox_t *)mbox_create (size);
}

bool os_mbox_fetch (os_mbox_t * mbox_, void ** msg, uint32_t time)
{
   int tmo = 0;
   mbox_t * mbox = (mbox_t *)mbox_;

   if (time != OS_WAIT_FOREVER)
   {
      tmo = mbox_fetch_tmo (mbox, msg, tick_from_ms (time));
   }
   else
   {
      mbox_fetch (mbox, msg);
   }

   return tmo;
}

bool os_mbox_post (os_mbox_t * mbox_, void * msg, uint32_t time)
{
   int tmo = 0;
   mbox_t * mbox = (mbox_t *)mbox_;

   if (time != OS_WAIT_FOREVER)
   {
      tmo = mbox_post_tmo (mbox, msg, tick_from_ms (time));
   }
   else
   {
      mbox_post (mbox, msg);
   }

   return tmo;
}

void os_mbox_destroy (os_mbox_t * mbox_)
{
   mbox_t * mbox = (mbox_t *)mbox_;
   mbox_destroy (mbox);
}

os_timer_t * os_timer_create (
   uint32_t us,
   void (*fn_) (os_timer_t *, void * arg),
   void * arg,
   bool oneshot)
{
   void (*fn)(tmr_t *, void *) = (void (*)(tmr_t *, void *))fn_;
   return (os_timer_t *)tmr_create (
      tick_from_ms (us / 1000),
      fn,
      arg,
      (oneshot) ? TMR_ONCE : TMR_CYCL);
}

void os_timer_set (os_timer_t * timer_, uint32_t us)
{
   tmr_t * timer = (tmr_t *)timer_;
   tmr_set (timer, tick_from_ms (us / 1000));
}

void os_timer_start (os_timer_t * timer_)
{
   tmr_t * timer = (tmr_t *)timer_;
   tmr_start (timer);
}

void os_timer_stop (os_timer_t * timer_)
{
   tmr_t * timer = (tmr_t *)timer_;
   tmr_stop (timer);
}

void os_timer_destroy (os_timer_t * timer_)
{
   tmr_t * timer = (tmr_t *)timer_;
   tmr_destroy (timer);
}
