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

#define URESOLUTION  10

void * os_malloc (size_t size)
{
   return malloc (size);
}

void os_free (void * ptr)
{
   free (ptr);
}

os_mutex_t * os_mutex_create (void)
{
   return CreateMutex (NULL, FALSE, NULL);
}

void os_mutex_lock (os_mutex_t * mutex)
{
   WaitForSingleObject (mutex, INFINITE);
}

void os_mutex_unlock (os_mutex_t * mutex)
{
   ReleaseMutex (mutex);
}

void os_mutex_destroy (os_mutex_t * mutex)
{
   CloseHandle (mutex);
}

void os_usleep (uint32_t usec)
{
   Sleep (usec / 1000);
}

os_thread_t * os_thread_create (
   const char * name,
   uint32_t priority,
   size_t stacksize,
   void (*entry) (void * arg),
   void * arg)
{
   HANDLE handle;
   handle =
      CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)entry, (LPVOID)arg, 0, NULL);

   if (priority < 5)
   {
      SetThreadPriority (handle, THREAD_PRIORITY_BELOW_NORMAL);
   }
   else if (priority >= 15)
   {
      SetThreadPriority (handle, THREAD_PRIORITY_TIME_CRITICAL);
   }
   return handle;
}

uint32_t os_get_current_time_us (void)
{
   static LARGE_INTEGER performanceFrequency = {0};
   LARGE_INTEGER currentCount;
   uint64_t currentTime;

   if (performanceFrequency.QuadPart == 0)
   {
      timeBeginPeriod (URESOLUTION);
      QueryPerformanceFrequency (&performanceFrequency);
      performanceFrequency.QuadPart = performanceFrequency.QuadPart / (1000 * 1000);
   }

   QueryPerformanceCounter (&currentCount);
   currentTime = currentCount.QuadPart / performanceFrequency.QuadPart;
   return (uint32_t)(currentTime & UINT32_MAX);
}

os_sem_t * os_sem_create (size_t count)
{
   os_sem_t * sem;

   sem = (os_sem_t *)malloc (sizeof (*sem));

   InitializeConditionVariable (&sem->condition);
   InitializeCriticalSection (&sem->lock);
   sem->count = count;

   return sem;
}

bool os_sem_wait (os_sem_t * sem, uint32_t time)
{
   BOOL success = TRUE;

   EnterCriticalSection (&sem->lock);
   while (sem->count == 0)
   {
      /* FIXME - decrease timeout if woken early */
      success = SleepConditionVariableCS (&sem->condition, &sem->lock, time);
      if (!success && GetLastError() == ERROR_TIMEOUT)
      {
         goto timeout;
      }
      assert (success);
   }

   sem->count--;

timeout:
   LeaveCriticalSection (&sem->lock);
   return !success;
}

void os_sem_signal (os_sem_t * sem)
{
   EnterCriticalSection (&sem->lock);
   sem->count++;
   LeaveCriticalSection (&sem->lock);
   WakeAllConditionVariable (&sem->condition);
}

void os_sem_destroy (os_sem_t * sem)
{
   EnterCriticalSection (&sem->lock);
   free (sem);
}

os_event_t * os_event_create (void)
{
   os_event_t * event;

   event = (os_event_t *)malloc (sizeof (*event));

   InitializeConditionVariable (&event->condition);
   InitializeCriticalSection (&event->lock);
   event->flags = 0;

   return event;
}

bool os_event_wait (os_event_t * event, uint32_t mask, uint32_t * value, uint32_t time)
{
   BOOL success = TRUE;

   EnterCriticalSection (&event->lock);
   while ((event->flags & mask) == 0)
   {
      /* FIXME - decrease timeout if woken early */
      success = SleepConditionVariableCS (&event->condition, &event->lock, time);
      if (!success && GetLastError() == ERROR_TIMEOUT)
      {
         break;
      }
   }
   *value = event->flags & mask;
   LeaveCriticalSection (&event->lock);
   return !success;
}

void os_event_set (os_event_t * event, uint32_t value)
{
   EnterCriticalSection (&event->lock);
   event->flags |= value;
   LeaveCriticalSection (&event->lock);
   WakeAllConditionVariable (&event->condition);
}

void os_event_clr (os_event_t * event, uint32_t value)
{
   EnterCriticalSection (&event->lock);
   event->flags &= ~value;
   LeaveCriticalSection (&event->lock);
}

void os_event_destroy (os_event_t * event)
{
   EnterCriticalSection (&event->lock);
   free (event);
}

os_mbox_t * os_mbox_create (size_t size)
{
   os_mbox_t * mbox;

   mbox = (os_mbox_t *)malloc (sizeof (*mbox) + size * sizeof (void *));

   InitializeConditionVariable (&mbox->condition);
   InitializeCriticalSection (&mbox->lock);

   mbox->r     = 0;
   mbox->w     = 0;
   mbox->count = 0;
   mbox->size  = size;

   return mbox;
}

bool os_mbox_fetch (os_mbox_t * mbox, void ** msg, uint32_t time)
{
   BOOL success = TRUE;

   EnterCriticalSection (&mbox->lock);
   while (mbox->count == 0)
   {
      /* FIXME - decrease timeout if woken early */
      success = SleepConditionVariableCS (&mbox->condition, &mbox->lock, time);
      if (!success && GetLastError() == ERROR_TIMEOUT)
      {
         goto timeout;
      }
      assert (success);
   }

   *msg = mbox->msg[mbox->r++];
   if (mbox->r == mbox->size)
      mbox->r = 0;

   mbox->count--;

timeout:
   LeaveCriticalSection (&mbox->lock);
   WakeAllConditionVariable (&mbox->condition);

   return !success;
}

bool os_mbox_post (os_mbox_t * mbox, void * msg, uint32_t time)
{
   BOOL success = TRUE;

   EnterCriticalSection (&mbox->lock);
   while (mbox->count == mbox->size)
   {
      /* FIXME - decrease timeout if woken early */
      success = SleepConditionVariableCS (&mbox->condition, &mbox->lock, time);
      if (!success && GetLastError() == ERROR_TIMEOUT)
      {
         goto timeout;
      }
      assert (success);
   }

   mbox->msg[mbox->w++] = msg;
   if (mbox->w == mbox->size)
      mbox->w = 0;

   mbox->count++;

timeout:
   LeaveCriticalSection (&mbox->lock);
   WakeAllConditionVariable (&mbox->condition);

   return !success;
}

void os_mbox_destroy (os_mbox_t * mbox)
{
   EnterCriticalSection (&mbox->lock);
   free (mbox);
}

static VOID CALLBACK
timer_callback (UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
   os_timer_t * timer = (os_timer_t *)dwUser;

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

   timer = (os_timer_t *)malloc (sizeof (*timer));

   timer->fn      = fn;
   timer->arg     = arg;
   timer->us      = us;
   timer->oneshot = oneshot;

   return timer;
}

void os_timer_set (os_timer_t * timer, uint32_t us)
{
   timer->us = us;
}

void os_timer_start (os_timer_t * timer)
{
   timeBeginPeriod (URESOLUTION);

   /****************************************************************
    * N.B. The function timeSetEvent is obsolete.                  *
    *      The reason for still using it here is that it gives     *
    *      much better resolution (15 ms -> 1 ms) than when        *
    *      using the recommended function CreateTimerQueueTimer.   *
    ****************************************************************/
   timer->timerID = timeSetEvent (
      timer->us / 1000,
      URESOLUTION,
      timer_callback,
      (DWORD_PTR)timer,
      (timer->oneshot) ? TIME_ONESHOT : TIME_PERIODIC);
}

void os_timer_stop (os_timer_t * timer)
{
   timeKillEvent (timer->timerID);

   timeEndPeriod (URESOLUTION);
}

void os_timer_destroy (os_timer_t * timer)
{
   free (timer);
}
