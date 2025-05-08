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

#define _GNU_SOURCE /* For pthread_setname_mp() */

#include "osal.h"
/* #include "options.h" */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <limits.h>

#include <pthread.h>

#include <assert.h>
#include <errno.h>
#include <unistd.h>

#include <sys/syscall.h>

/* Priority of timer callback thread (if USE_SCHED_FIFO is set) */
#define TIMER_PRIO 30

#define USECS_PER_SEC (1 * 1000 * 1000)
#define NSECS_PER_SEC (1 * 1000 * 1000 * 1000)

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
   int result;
   pthread_t * thread;
   pthread_attr_t attr;

   thread = malloc (sizeof (*thread));
   CC_ASSERT (thread != 0);

   pthread_attr_init (&attr);
   pthread_attr_setstacksize (&attr, PTHREAD_STACK_MIN + stacksize);

#if defined(USE_SCHED_FIFO)
   CC_STATIC_ASSERT (_POSIX_THREAD_PRIORITY_SCHEDULING > 0);
   struct sched_param param = {.sched_priority = priority};
   pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED);
   pthread_attr_setschedpolicy (&attr, SCHED_FIFO);
   pthread_attr_setschedparam (&attr, &param);
#endif

   result = pthread_create (thread, &attr, (void *)entry, arg);
   CC_ASSERT (result == 0);

   pthread_setname_np (*thread, name);
   return thread;
}

os_mutex_t * os_mutex_create (void)
{
   int result;
   pthread_mutex_t * mutex;
   pthread_mutexattr_t mattr;

   mutex = malloc (sizeof (*mutex));
   CC_ASSERT (mutex != NULL);

   CC_STATIC_ASSERT (_POSIX_THREAD_PRIO_INHERIT > 0);
   pthread_mutexattr_init (&mattr);
   pthread_mutexattr_setprotocol (&mattr, PTHREAD_PRIO_INHERIT);
   pthread_mutexattr_settype (&mattr, PTHREAD_MUTEX_RECURSIVE);

   result = pthread_mutex_init (mutex, &mattr);
   CC_ASSERT (result == 0);

   return mutex;
}

void os_mutex_lock (os_mutex_t * _mutex)
{
   pthread_mutex_t * mutex = _mutex;
   pthread_mutex_lock (mutex);
}

void os_mutex_unlock (os_mutex_t * _mutex)
{
   pthread_mutex_t * mutex = _mutex;
   pthread_mutex_unlock (mutex);
}

void os_mutex_destroy (os_mutex_t * _mutex)
{
   pthread_mutex_t * mutex = _mutex;
   pthread_mutex_destroy (mutex);
   free (mutex);
}

os_sem_t * os_sem_create (size_t count)
{
   os_sem_t * sem;
   pthread_mutexattr_t mattr;
   pthread_condattr_t cattr;

   sem = malloc (sizeof (*sem));
   CC_ASSERT (sem != NULL);

   pthread_condattr_init (&cattr);
   pthread_condattr_setclock (&cattr, CLOCK_MONOTONIC);
   pthread_cond_init (&sem->cond, &cattr);
   pthread_mutexattr_init (&mattr);
   pthread_mutexattr_setprotocol (&mattr, PTHREAD_PRIO_INHERIT);
   pthread_mutex_init (&sem->mutex, &mattr);
   sem->count = count;

   return sem;
}

bool os_sem_wait (os_sem_t * sem, uint32_t time)
{
   struct timespec ts;
   int error     = 0;
   uint64_t nsec = (uint64_t)time * 1000 * 1000;

   clock_gettime (CLOCK_MONOTONIC, &ts);
   nsec += ts.tv_nsec;
   if (nsec > NSECS_PER_SEC)
   {
      ts.tv_sec += nsec / NSECS_PER_SEC;
      nsec %= NSECS_PER_SEC;
   }
   ts.tv_nsec = nsec;

   pthread_mutex_lock (&sem->mutex);
   while (sem->count == 0)
   {
      if (time != OS_WAIT_FOREVER)
      {
         error = pthread_cond_timedwait (&sem->cond, &sem->mutex, &ts);
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

void os_usleep (uint32_t usec)
{
   struct timespec ts;
   struct timespec remain;

   ts.tv_sec  = usec / USECS_PER_SEC;
   ts.tv_nsec = (usec % USECS_PER_SEC) * 1000;
   while (clock_nanosleep (CLOCK_MONOTONIC, 0, &ts, &remain) != 0)
   {
      ts = remain;
   }
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
   os_tick_t       tick;

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
   struct timespec ts;
   struct timespec remain;

   ts.tv_sec  = tick / NSECS_PER_SEC;
   ts.tv_nsec = tick % NSECS_PER_SEC;
   while (clock_nanosleep (CLOCK_MONOTONIC, 0, &ts, &remain) != 0)
   {
      ts = remain;
   }
}

os_event_t * os_event_create (void)
{
   os_event_t * event;
   pthread_mutexattr_t mattr;
   pthread_condattr_t cattr;

   event = (os_event_t *)malloc (sizeof (*event));
   CC_ASSERT (event != NULL);

   pthread_condattr_init (&cattr);
   pthread_condattr_setclock (&cattr, CLOCK_MONOTONIC);
   pthread_cond_init (&event->cond, &cattr);
   pthread_mutexattr_init (&mattr);
   pthread_mutexattr_setprotocol (&mattr, PTHREAD_PRIO_INHERIT);
   pthread_mutex_init (&event->mutex, &mattr);
   event->flags = 0;

   return event;
}

bool os_event_wait (os_event_t * event, uint32_t mask, uint32_t * value, uint32_t time)
{
   struct timespec ts;
   int error     = 0;
   uint64_t nsec = (uint64_t)time * 1000 * 1000;

   if (time != OS_WAIT_FOREVER)
   {
      clock_gettime (CLOCK_MONOTONIC, &ts);
      nsec += ts.tv_nsec;

      ts.tv_sec += nsec / NSECS_PER_SEC;
      ts.tv_nsec = nsec % NSECS_PER_SEC;
   }

   pthread_mutex_lock (&event->mutex);

   while ((event->flags & mask) == 0)
   {
      if (time != OS_WAIT_FOREVER)
      {
         error = pthread_cond_timedwait (&event->cond, &event->mutex, &ts);
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
   pthread_mutexattr_t mattr;
   pthread_condattr_t cattr;

   mbox = (os_mbox_t *)malloc (sizeof (*mbox) + size * sizeof (void *));
   CC_ASSERT (mbox != NULL);

   pthread_condattr_init (&cattr);
   pthread_condattr_setclock (&cattr, CLOCK_MONOTONIC);
   pthread_cond_init (&mbox->cond, &cattr);
   pthread_mutexattr_init (&mattr);
   pthread_mutexattr_setprotocol (&mattr, PTHREAD_PRIO_INHERIT);
   pthread_mutex_init (&mbox->mutex, &mattr);

   mbox->r     = 0;
   mbox->w     = 0;
   mbox->count = 0;
   mbox->size  = size;

   return mbox;
}

bool os_mbox_fetch (os_mbox_t * mbox, void ** msg, uint32_t time)
{
   struct timespec ts;
   int error     = 0;
   uint64_t nsec = (uint64_t)time * 1000 * 1000;

   if (time != OS_WAIT_FOREVER)
   {
      clock_gettime (CLOCK_MONOTONIC, &ts);
      nsec += ts.tv_nsec;

      ts.tv_sec += nsec / NSECS_PER_SEC;
      ts.tv_nsec = nsec % NSECS_PER_SEC;
   }

   pthread_mutex_lock (&mbox->mutex);

   while (mbox->count == 0)
   {
      if (time != OS_WAIT_FOREVER)
      {
         error = pthread_cond_timedwait (&mbox->cond, &mbox->mutex, &ts);
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
   int error     = 0;
   uint64_t nsec = (uint64_t)time * 1000 * 1000;

   if (time != OS_WAIT_FOREVER)
   {
      clock_gettime (CLOCK_MONOTONIC, &ts);
      nsec += ts.tv_nsec;

      ts.tv_sec += nsec / NSECS_PER_SEC;
      ts.tv_nsec = nsec % NSECS_PER_SEC;
   }

   pthread_mutex_lock (&mbox->mutex);

   while (mbox->count == mbox->size)
   {
      if (time != OS_WAIT_FOREVER)
      {
         error = pthread_cond_timedwait (&mbox->cond, &mbox->mutex, &ts);
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

static void os_timer_thread (void * arg)
{
   os_timer_t * timer = arg;
   sigset_t sigset;
   siginfo_t si;
   struct timespec tmo;

   timer->thread_id = (pid_t)syscall (SYS_gettid);

   /* Add SIGALRM */
   sigemptyset (&sigset);
   sigprocmask (SIG_BLOCK, &sigset, NULL);
   sigaddset (&sigset, SIGALRM);

   tmo.tv_sec  = 0;
   tmo.tv_nsec = 500 * 1000 * 1000;

   while (!timer->exit)
   {
      int sig = sigtimedwait (&sigset, &si, &tmo);
      if (sig == SIGALRM)
      {
         if (timer->fn)
            timer->fn (timer, timer->arg);
      }
   }
}

os_timer_t * os_timer_create (
   uint32_t us,
   void (*fn) (os_timer_t *, void * arg),
   void * arg,
   bool oneshot)
{
   os_timer_t * timer;
   struct sigevent sev;
   sigset_t sigset;
   int res;

   /* Block SIGALRM in calling thread */
   sigemptyset (&sigset);
   sigaddset (&sigset, SIGALRM);
   sigprocmask (SIG_BLOCK, &sigset, NULL);

   timer = (os_timer_t *)malloc (sizeof (*timer));
   CC_ASSERT (timer != NULL);

   timer->exit      = false;
   timer->thread_id = 0;
   timer->fn        = fn;
   timer->arg       = arg;
   timer->us        = us;
   timer->oneshot   = oneshot;

   /* Create timer thread */
   timer->thread =
      os_thread_create ("os_timer", TIMER_PRIO, 1024, os_timer_thread, timer);
   CC_ASSERT (timer->thread != NULL);


   /* Wait until timer thread sets its (kernel) thread id */
   do
   {
      sched_yield();
   } while (timer->thread_id == 0);

   /* Create timer */
   sev.sigev_notify            = SIGEV_THREAD_ID;
   sev.sigev_value.sival_ptr   = timer;
   sev._sigev_un._tid          = timer->thread_id;
   sev.sigev_signo             = SIGALRM;
   sev.sigev_notify_attributes = NULL;

   res = timer_create (CLOCK_MONOTONIC, &sev, &timer->timerid);
   CC_ASSERT (res != -1);

   return timer;
}

void os_timer_set (os_timer_t * timer, uint32_t us)
{
   timer->us = us;
}

void os_timer_start (os_timer_t * timer)
{
   struct itimerspec its;
   uint32_t sec = timer->us / (1000 * 1000);
   uint32_t us = timer->us - sec * 1000 * 1000;

   /* Start timer */
   its.it_value.tv_sec     = sec;
   its.it_value.tv_nsec    = 1000 * us;
   its.it_interval.tv_sec  = (timer->oneshot) ? 0 : its.it_value.tv_sec;
   its.it_interval.tv_nsec = (timer->oneshot) ? 0 : its.it_value.tv_nsec;
   timer_settime (timer->timerid, 0, &its, NULL);
}

void os_timer_stop (os_timer_t * timer)
{
   struct itimerspec its;

   /* Stop timer */
   its.it_value.tv_sec     = 0;
   its.it_value.tv_nsec    = 0;
   its.it_interval.tv_sec  = 0;
   its.it_interval.tv_nsec = 0;
   timer_settime (timer->timerid, 0, &its, NULL);
}

void os_timer_destroy (os_timer_t * timer)
{
   timer->exit = true;
   pthread_join (*timer->thread, NULL);
   timer_delete (timer->timerid);
   free (timer);
}
