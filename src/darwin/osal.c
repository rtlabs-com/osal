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

/*
 * macOS port. Differs from the Linux port where macOS lacks the
 * corresponding POSIX APIs:
 *
 * - pthread_condattr_setclock is missing, so timed waits compute an
 *   absolute CLOCK_MONOTONIC deadline and wait with the relative
 *   pthread_cond_timedwait_relative_np.
 * - clock_nanosleep is missing, so sleeps use nanosleep. Note that macOS
 *   applies timer coalescing to threads without a realtime policy, so
 *   sleeps and timeouts may overshoot by a few percent.
 * - POSIX timers (timer_create) are missing, so timers use a GCD
 *   dispatch source on a private serial queue.
 * - pthread_setname_np can only name the calling thread, so threads
 *   name themselves from a start trampoline.
 */

#include "osal.h"
#include "osal_sys.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#include <pthread.h>

#include <errno.h>
#include <unistd.h>

#define USECS_PER_SEC (1 * 1000 * 1000)
#define NSECS_PER_SEC (1 * 1000 * 1000 * 1000)

/* Max thread name length on macOS, including terminator */
#define THREAD_NAME_SIZE 64

typedef struct os_thread_start
{
   void (*entry) (void * arg);
   void * arg;
   char name[THREAD_NAME_SIZE];
} os_thread_start_t;

void * os_malloc (size_t size)
{
   return malloc (size);
}

void os_free (void * ptr)
{
   free (ptr);
}

static void * os_thread_trampoline (void * arg)
{
   os_thread_start_t start = *(os_thread_start_t *)arg;

   free (arg);
   pthread_setname_np (start.name);
   start.entry (start.arg);
   return NULL;
}

os_thread_t * os_thread_create (
   const char * name,
   uint32_t priority,
   size_t stacksize,
   void (*entry) (void * arg),
   void * arg)
{
   int result;
   os_thread_t * thread;
   os_thread_start_t * start;
   pthread_attr_t attr;
   size_t pagesize = (size_t)sysconf (_SC_PAGESIZE);

   thread = malloc (sizeof (*thread));
   CC_ASSERT (thread != NULL);

   start = malloc (sizeof (*start));
   CC_ASSERT (start != NULL);
   start->entry = entry;
   start->arg   = arg;
   strlcpy (start->name, name, sizeof (start->name));

   /* macOS rejects stack sizes that are not a multiple of the page size */
   stacksize = PTHREAD_STACK_MIN + stacksize;
   stacksize = (stacksize + pagesize - 1) / pagesize * pagesize;

   pthread_attr_init (&attr);
   result = pthread_attr_setstacksize (&attr, stacksize);
   CC_ASSERT (result == 0);

   result = pthread_create (&thread->thread, &attr, os_thread_trampoline, start);
   CC_ASSERT (result == 0);

   pthread_attr_destroy (&attr);
   return thread;
}

os_mutex_t * os_mutex_create (void)
{
   int result;
   os_mutex_t * mutex;
   pthread_mutexattr_t mattr;

   mutex = malloc (sizeof (os_mutex_t));
   CC_ASSERT (mutex != NULL);

   pthread_mutexattr_init (&mattr);
   pthread_mutexattr_setprotocol (&mattr, PTHREAD_PRIO_INHERIT);
   pthread_mutexattr_settype (&mattr, PTHREAD_MUTEX_RECURSIVE);

   result = pthread_mutex_init (&mutex->mutex, &mattr);
   CC_ASSERT (result == 0);

   return mutex;
}

void os_mutex_lock (os_mutex_t * mutex)
{
   pthread_mutex_lock (&mutex->mutex);
}

void os_mutex_unlock (os_mutex_t * mutex)
{
   pthread_mutex_unlock (&mutex->mutex);
}

void os_mutex_destroy (os_mutex_t * mutex)
{
   pthread_mutex_destroy (&mutex->mutex);
   free (mutex);
}

/* Compute absolute CLOCK_MONOTONIC deadline ms milliseconds from now */
static void os_deadline_from_ms (uint32_t ms, struct timespec * deadline)
{
   uint64_t nsec = (uint64_t)ms * 1000 * 1000;

   clock_gettime (CLOCK_MONOTONIC, deadline);
   nsec += deadline->tv_nsec;
   deadline->tv_sec += nsec / NSECS_PER_SEC;
   deadline->tv_nsec = nsec % NSECS_PER_SEC;
}

/* Wait on cond until the absolute CLOCK_MONOTONIC deadline. Returns 0 if
 * signalled (or woken spuriously), ETIMEDOUT if the deadline has passed. */
static int os_cond_wait_until (
   pthread_cond_t * cond,
   pthread_mutex_t * mutex,
   const struct timespec * deadline)
{
   struct timespec now;
   struct timespec rel;

   clock_gettime (CLOCK_MONOTONIC, &now);
   rel.tv_sec  = deadline->tv_sec - now.tv_sec;
   rel.tv_nsec = deadline->tv_nsec - now.tv_nsec;
   if (rel.tv_nsec < 0)
   {
      rel.tv_sec--;
      rel.tv_nsec += NSECS_PER_SEC;
   }

   if (rel.tv_sec < 0 || (rel.tv_sec == 0 && rel.tv_nsec == 0))
   {
      return ETIMEDOUT;
   }

   return pthread_cond_timedwait_relative_np (cond, mutex, &rel);
}

static void os_cond_mutex_init (pthread_cond_t * cond, pthread_mutex_t * mutex)
{
   pthread_mutexattr_t mattr;

   pthread_cond_init (cond, NULL);
   pthread_mutexattr_init (&mattr);
   pthread_mutexattr_setprotocol (&mattr, PTHREAD_PRIO_INHERIT);
   pthread_mutex_init (mutex, &mattr);
   pthread_mutexattr_destroy (&mattr);
}

os_sem_t * os_sem_create (size_t count)
{
   os_sem_t * sem;

   sem = malloc (sizeof (*sem));
   CC_ASSERT (sem != NULL);

   os_cond_mutex_init (&sem->cond, &sem->mutex);
   sem->count = count;

   return sem;
}

bool os_sem_wait (os_sem_t * sem, uint32_t time)
{
   struct timespec ts;
   int error = 0;

   if (time != OS_WAIT_FOREVER)
   {
      os_deadline_from_ms (time, &ts);
   }

   pthread_mutex_lock (&sem->mutex);
   while (sem->count == 0)
   {
      if (time != OS_WAIT_FOREVER)
      {
         error = os_cond_wait_until (&sem->cond, &sem->mutex, &ts);
         CC_ASSERT (error != EINVAL);
         if (error)
         {
            goto timeout;
         }
      }
      else
      {
         error = pthread_cond_wait (&sem->cond, &sem->mutex);
         CC_ASSERT (error != EINVAL);
      }
   }

   sem->count--;

timeout:
   pthread_mutex_unlock (&sem->mutex);
   return (error != 0);
}

void os_sem_signal (os_sem_t * sem)
{
   pthread_mutex_lock (&sem->mutex);
   sem->count++;
   pthread_cond_signal (&sem->cond);
   pthread_mutex_unlock (&sem->mutex);
}

void os_sem_destroy (os_sem_t * sem)
{
   pthread_cond_destroy (&sem->cond);
   pthread_mutex_destroy (&sem->mutex);
   free (sem);
}

static void os_nanosleep (uint64_t nsec)
{
   struct timespec ts;
   struct timespec remain;

   ts.tv_sec  = nsec / NSECS_PER_SEC;
   ts.tv_nsec = nsec % NSECS_PER_SEC;
   while (nanosleep (&ts, &remain) != 0)
   {
      ts = remain;
   }
}

void os_usleep (uint32_t usec)
{
   os_nanosleep ((uint64_t)usec * 1000);
}

uint32_t os_get_current_time_us (void)
{
   struct timespec ts;

   clock_gettime (CLOCK_MONOTONIC, &ts);
   return ts.tv_sec * 1000 * 1000 + ts.tv_nsec / 1000;
}

os_tick_t os_tick_current (void)
{
   struct timespec ts;
   os_tick_t tick;

   clock_gettime (CLOCK_MONOTONIC, &ts);
   tick = ts.tv_sec;
   tick *= NSECS_PER_SEC;
   tick += ts.tv_nsec;
   return tick;
}

os_tick_t os_tick_from_us (uint32_t us)
{
   return (os_tick_t)us * 1000;
}

void os_tick_sleep (os_tick_t tick)
{
   os_nanosleep (tick);
}

os_event_t * os_event_create (void)
{
   os_event_t * event;

   event = (os_event_t *)malloc (sizeof (*event));
   CC_ASSERT (event != NULL);

   os_cond_mutex_init (&event->cond, &event->mutex);
   event->flags = 0;

   return event;
}

bool os_event_wait (os_event_t * event, uint32_t mask, uint32_t * value, uint32_t time)
{
   struct timespec ts;
   int error = 0;

   if (time != OS_WAIT_FOREVER)
   {
      os_deadline_from_ms (time, &ts);
   }

   pthread_mutex_lock (&event->mutex);

   while ((event->flags & mask) == 0)
   {
      if (time != OS_WAIT_FOREVER)
      {
         error = os_cond_wait_until (&event->cond, &event->mutex, &ts);
         CC_ASSERT (error != EINVAL);
         if (error)
         {
            goto timeout;
         }
      }
      else
      {
         error = pthread_cond_wait (&event->cond, &event->mutex);
         CC_ASSERT (error != EINVAL);
      }
   }

timeout:
   *value = event->flags & mask;
   pthread_mutex_unlock (&event->mutex);
   return (error != 0);
}

void os_event_set (os_event_t * event, uint32_t value)
{
   pthread_mutex_lock (&event->mutex);
   event->flags |= value;
   pthread_cond_signal (&event->cond);
   pthread_mutex_unlock (&event->mutex);
}

void os_event_clr (os_event_t * event, uint32_t value)
{
   pthread_mutex_lock (&event->mutex);
   event->flags &= ~value;
   pthread_mutex_unlock (&event->mutex);
}

void os_event_destroy (os_event_t * event)
{
   pthread_cond_destroy (&event->cond);
   pthread_mutex_destroy (&event->mutex);
   free (event);
}

os_mbox_t * os_mbox_create (size_t size)
{
   os_mbox_t * mbox;

   mbox = (os_mbox_t *)malloc (sizeof (*mbox) + size * sizeof (void *));
   CC_ASSERT (mbox != NULL);

   os_cond_mutex_init (&mbox->cond, &mbox->mutex);

   mbox->r     = 0;
   mbox->w     = 0;
   mbox->count = 0;
   mbox->size  = size;

   return mbox;
}

bool os_mbox_fetch (os_mbox_t * mbox, void ** msg, uint32_t time)
{
   struct timespec ts;
   int error = 0;

   if (time != OS_WAIT_FOREVER)
   {
      os_deadline_from_ms (time, &ts);
   }

   pthread_mutex_lock (&mbox->mutex);

   while (mbox->count == 0)
   {
      if (time != OS_WAIT_FOREVER)
      {
         error = os_cond_wait_until (&mbox->cond, &mbox->mutex, &ts);
         CC_ASSERT (error != EINVAL);
         if (error)
         {
            goto timeout;
         }
      }
      else
      {
         error = pthread_cond_wait (&mbox->cond, &mbox->mutex);
         CC_ASSERT (error != EINVAL);
      }
   }

   *msg = mbox->msg[mbox->r++];
   if (mbox->r == mbox->size)
      mbox->r = 0;

   mbox->count--;

timeout:
   pthread_mutex_unlock (&mbox->mutex);
   pthread_cond_signal (&mbox->cond);

   return (error != 0);
}

bool os_mbox_post (os_mbox_t * mbox, void * msg, uint32_t time)
{
   struct timespec ts;
   int error = 0;

   if (time != OS_WAIT_FOREVER)
   {
      os_deadline_from_ms (time, &ts);
   }

   pthread_mutex_lock (&mbox->mutex);

   while (mbox->count == mbox->size)
   {
      if (time != OS_WAIT_FOREVER)
      {
         error = os_cond_wait_until (&mbox->cond, &mbox->mutex, &ts);
         CC_ASSERT (error != EINVAL);
         if (error)
         {
            goto timeout;
         }
      }
      else
      {
         error = pthread_cond_wait (&mbox->cond, &mbox->mutex);
         CC_ASSERT (error != EINVAL);
      }
   }

   mbox->msg[mbox->w++] = msg;
   if (mbox->w == mbox->size)
      mbox->w = 0;

   mbox->count++;

timeout:
   pthread_mutex_unlock (&mbox->mutex);
   pthread_cond_signal (&mbox->cond);

   return (error != 0);
}

void os_mbox_destroy (os_mbox_t * mbox)
{
   pthread_cond_destroy (&mbox->cond);
   pthread_mutex_destroy (&mbox->mutex);
   free (mbox);
}

static void os_timer_disarm (os_timer_t * timer)
{
   dispatch_source_set_timer (timer->source, DISPATCH_TIME_FOREVER, 0, 0);
}

static void os_timer_handler (void * arg)
{
   os_timer_t * timer = arg;

   if (timer->oneshot)
   {
      os_timer_disarm (timer);
   }

   if (timer->fn)
      timer->fn (timer, timer->arg);
}

static void os_timer_noop (void * arg)
{
}

os_timer_t * os_timer_create (
   uint32_t us,
   void (*fn) (os_timer_t *, void * arg),
   void * arg,
   bool oneshot)
{
   os_timer_t * timer;
   dispatch_queue_attr_t attr;

   timer = (os_timer_t *)malloc (sizeof (*timer));
   CC_ASSERT (timer != NULL);

   timer->fn      = fn;
   timer->arg     = arg;
   timer->us      = us;
   timer->oneshot = oneshot;

   /* Callbacks run serially on a private high priority queue */
   attr = dispatch_queue_attr_make_with_qos_class (
      DISPATCH_QUEUE_SERIAL,
      QOS_CLASS_USER_INTERACTIVE,
      0);
   timer->queue = dispatch_queue_create ("os_timer", attr);
   CC_ASSERT (timer->queue != NULL);

   timer->source = dispatch_source_create (
      DISPATCH_SOURCE_TYPE_TIMER,
      0,
      DISPATCH_TIMER_STRICT,
      timer->queue);
   CC_ASSERT (timer->source != NULL);

   dispatch_set_context (timer->source, timer);
   dispatch_source_set_event_handler_f (timer->source, os_timer_handler);
   os_timer_disarm (timer);
   dispatch_activate (timer->source);

   return timer;
}

void os_timer_set (os_timer_t * timer, uint32_t us)
{
   timer->us = us;
}

void os_timer_start (os_timer_t * timer)
{
   uint64_t interval = (uint64_t)timer->us * 1000;

   dispatch_source_set_timer (
      timer->source,
      dispatch_time (DISPATCH_TIME_NOW, interval),
      (timer->oneshot) ? DISPATCH_TIME_FOREVER : interval,
      0);
}

void os_timer_stop (os_timer_t * timer)
{
   os_timer_disarm (timer);
}

void os_timer_destroy (os_timer_t * timer)
{
   dispatch_source_cancel (timer->source);

   /* Wait for any callback in progress to finish */
   dispatch_sync_f (timer->queue, NULL, os_timer_noop);

   dispatch_release (timer->source);
   dispatch_release (timer->queue);
   free (timer);
}
