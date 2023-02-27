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
#include "osal_log.h"

#ifdef __GNUC__
#define CC_THREAD_LOCAL __thread
#elif defined(_MSC_VER)
#define CC_THREAD_LOCAL __declspec (thread)
#elif __STDC_VERSION__ >= 201112L
#define CC_THREAD_LOCAL _Thread_local
#else
#error Cannot define CC_THREAD
#endif

#define SCALE_100NS_PER_S 10000000ULL
#define SCALE_100NS_PER_US 10ULL

typedef struct os_thread_state
{
   HANDLE timer;
   void (*entry) (void * arg);
   void * arg;
} os_thread_state_t;

static CC_THREAD_LOCAL os_thread_state_t * os_thread_state;

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
   CRITICAL_SECTION * handle;
   handle = CreateMutex (NULL, FALSE, NULL);
   CC_ASSERT (handle != INVALID_HANDLE_VALUE);
   return handle;
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

static os_thread_state_t * os_thread_state_init (void (*entry) (void *), void * arg)
{
   os_thread_state_t * state =
      (os_thread_state_t *)calloc (1, sizeof (os_thread_state_t));
   CC_ASSERT (state != NULL);

   state->entry = entry;
   state->arg   = arg;
   state->timer = CreateWaitableTimerExA (
      NULL,
      NULL,
      CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
      TIMER_ALL_ACCESS);
   CC_ASSERT (state->timer != INVALID_HANDLE_VALUE);

   return state;
}

static os_thread_state_t * os_thread_state_get (void)
{
   if (os_thread_state == NULL)
   {
      os_thread_state = os_thread_state_init (NULL, NULL);
   }
   return os_thread_state;
}

static void os_thread_state_free (os_thread_state_t * state)
{
   CloseHandle (state->timer);
   free (state);
}

static void os_internal_sleep (uint64_t delay_100_ns)
{
   LARGE_INTEGER ft;
   BOOL res;
   DWORD event;
   os_thread_state_t * state;

   state = os_thread_state_get();

   CC_ASSERT (delay_100_ns < INT64_MAX);

   ft.QuadPart = -(int64_t)delay_100_ns;

   res = SetWaitableTimer (state->timer, &ft, 0, NULL, NULL, FALSE);
   CC_ASSERT (res);

   event = WaitForSingleObject (state->timer, INFINITE);
   CC_ASSERT (event == WAIT_OBJECT_0);
}

void os_usleep (uint32_t usec)
{
   os_internal_sleep (SCALE_100NS_PER_US * usec);
}

static void os_thread_entry (void * arg)
{
   os_thread_state_t * state = arg;

   os_thread_state = state;
   os_thread_state->entry (os_thread_state->arg);
   os_thread_state = NULL;

   os_thread_state_free (state);
}

os_thread_t * os_thread_create (
   const char * name,
   uint32_t priority,
   size_t stacksize,
   void (*entry) (void * arg),
   void * arg)
{
   HANDLE handle;
   os_thread_state_t * state;

   state  = os_thread_state_init (entry, arg);
   handle = CreateThread (
      NULL,
      0,
      (LPTHREAD_START_ROUTINE)os_thread_entry,
      (LPVOID)state,
      0,
      NULL);
   CC_ASSERT (handle != INVALID_HANDLE_VALUE);

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

static uint64_t os_get_frequency_tick (void)
{
   static uint64_t frequency;
   if (frequency == 0)
   {
      LARGE_INTEGER performanceFrequency;
      QueryPerformanceFrequency (&performanceFrequency);
      frequency = performanceFrequency.QuadPart;
   }
   return frequency;
}

uint32_t os_get_current_time_us (void)
{
   LARGE_INTEGER currentCount;
   uint64_t currentTime;
   QueryPerformanceCounter (&currentCount);
   currentTime = 1000000 * currentCount.QuadPart / os_get_frequency_tick();
   return (uint32_t)(currentTime & UINT32_MAX);
}

os_tick_t os_tick_current (void)
{
   LARGE_INTEGER currentCount;
   QueryPerformanceCounter (&currentCount);
   return currentCount.QuadPart;
}

os_tick_t os_tick_from_us (uint32_t us)
{
   return os_get_frequency_tick() * us / 1000000;
}

void os_tick_sleep (os_tick_t tick)
{
   uint64_t delay;
   delay = SCALE_100NS_PER_S;
   delay *= tick;
   delay /= os_get_frequency_tick();
   os_internal_sleep (delay);
}

os_sem_t * os_sem_create (size_t count)
{
   os_sem_t * sem;

   sem = (os_sem_t *)malloc (sizeof (*sem));
   CC_ASSERT (sem != NULL);

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
      CC_ASSERT (success);
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
   CC_ASSERT (event != NULL);

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
   CC_ASSERT (mbox != NULL);

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
      CC_ASSERT (success);
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
      CC_ASSERT (success);
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

static VOID CALLBACK timer_thread (void * arg)
{
   os_timer_t * timer = (os_timer_t *)arg;
   BOOL res;

   while (timer->run)
   {
      res = WaitForSingleObject (timer->timer, INFINITE);
      CC_ASSERT (res == WAIT_OBJECT_0);

      if (!timer->oneshot)
      {
         res = SetWaitableTimer (timer->timer, &timer->time, 0, NULL, NULL, 0);
         CC_ASSERT (res == TRUE);
      }

      if (timer->fn)
      {
         timer->fn (timer, timer->arg);
      }
   }

   CloseHandle (timer->timer);
   free (timer);
}

os_timer_t * os_timer_create (
   uint32_t us,
   void (*fn) (os_timer_t *, void * arg),
   void * arg,
   bool oneshot)
{
   os_timer_t * timer;

   timer = (os_timer_t *)calloc (1, sizeof (os_timer_t));

   timer->fn      = fn;
   timer->arg     = arg;
   timer->oneshot = oneshot;
   timer->run     = true;

   os_timer_set (timer, us);

   timer->timer = CreateWaitableTimerExA (
      NULL,
      NULL,
      CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
      TIMER_ALL_ACCESS);
   CC_ASSERT (timer->timer != INVALID_HANDLE_VALUE);

   HANDLE thread = CreateThread (
      NULL,
      0,
      (LPTHREAD_START_ROUTINE)timer_thread,
      (LPVOID)timer,
      0,
      NULL);
   CC_ASSERT (thread != INVALID_HANDLE_VALUE);

   SetThreadPriority (thread, THREAD_PRIORITY_TIME_CRITICAL);

   /* Thread will clean itself up,
      we don't need this handle */
   CloseHandle (thread);

   return timer;
}

void os_timer_set (os_timer_t * timer, uint32_t us)
{
   uint64_t delay = SCALE_100NS_PER_US * (uint64_t)us;
   CC_ASSERT (delay < INT64_MAX);
   timer->time.QuadPart = -(int64_t)(delay);
}

void os_timer_start (os_timer_t * timer)
{
   BOOL res;
   res = SetWaitableTimer (timer->timer, &timer->time, 0, NULL, NULL, FALSE);
   CC_ASSERT (res == TRUE);
}

void os_timer_stop (os_timer_t * timer)
{
   CancelWaitableTimer (timer->timer);
}

void os_timer_destroy (os_timer_t * timer)
{
   CancelWaitableTimer (timer->timer);
   timer->run = false;
}
