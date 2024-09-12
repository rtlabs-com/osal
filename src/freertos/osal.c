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
 * This software is licensed under the terms of the BSD 3-clause
 * license. See the file LICENSE distributed with this software for
 * full license information.
 ********************************************************************/

#include "osal.h"

#include <stdlib.h>

#define TMO_TO_TICKS(ms) \
   ((ms == OS_WAIT_FOREVER) ? portMAX_DELAY : (ms) / portTICK_PERIOD_MS)

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
   TaskHandle_t xHandle = NULL;
   BaseType_t status;

   /* stacksize in freertos is not in bytes but in stack depth, it should be
    * divided by the stack width */
   configSTACK_DEPTH_TYPE stackdepth =
      stacksize / sizeof (StackType_t);

   status = xTaskCreate (entry, name, stackdepth, arg, priority, &xHandle);
   CC_UNUSED (status);
   CC_ASSERT (status == pdPASS);
   CC_ASSERT (xHandle != NULL);
   return (os_thread_t *)xHandle;
}

os_mutex_t * os_mutex_create (void)
{
   SemaphoreHandle_t handle = xSemaphoreCreateRecursiveMutex();
   CC_ASSERT (handle != NULL);
   return (os_mutex_t *)handle;
}

void os_mutex_lock (os_mutex_t * mutex)
{
   xSemaphoreTakeRecursive ((SemaphoreHandle_t)mutex, portMAX_DELAY);
}

void os_mutex_unlock (os_mutex_t * mutex)
{
   xSemaphoreGiveRecursive ((SemaphoreHandle_t)mutex);
}

void os_mutex_destroy (os_mutex_t * mutex)
{
   vSemaphoreDelete ((SemaphoreHandle_t)mutex);
}

void os_usleep (uint32_t us)
{
   vTaskDelay ((us / portTICK_PERIOD_MS) / 1000);
}

uint32_t os_get_current_time_us (void)
{
   return 1000 * (xTaskGetTickCount() / portTICK_PERIOD_MS);
}

os_tick_t os_tick_current (void)
{
   return xTaskGetTickCount();
}

os_tick_t os_tick_from_us (uint32_t us)
{
   return us / (1000u * portTICK_PERIOD_MS);
}

void os_tick_sleep (os_tick_t tick)
{
   vTaskDelay (tick);
}

os_sem_t * os_sem_create (size_t count)
{
   SemaphoreHandle_t handle = xSemaphoreCreateCounting (UINT32_MAX, count);
   CC_ASSERT (handle != NULL);
   return (os_sem_t *)handle;
}

bool os_sem_wait (os_sem_t * sem, uint32_t time)
{
   if (xSemaphoreTake ((SemaphoreHandle_t)sem, TMO_TO_TICKS (time)) == pdTRUE)
   {
      /* Did not timeout */
      return false;
   }

   /* Timed out */
   return true;
}

void os_sem_signal (os_sem_t * sem)
{
   xSemaphoreGive ((SemaphoreHandle_t)sem);
}

void os_sem_destroy (os_sem_t * sem)
{
   vSemaphoreDelete ((SemaphoreHandle_t)sem);
}

os_event_t * os_event_create (void)
{
   EventGroupHandle_t handle = xEventGroupCreate();
   CC_ASSERT (handle);
   return (os_event_t *)handle;
}

bool os_event_wait (os_event_t * event, uint32_t mask, uint32_t * value, uint32_t time)
{
   *value = xEventGroupWaitBits (
      (EventGroupHandle_t)event,
      mask,
      pdFALSE,
      pdFALSE,
      TMO_TO_TICKS (time));

   *value &= mask;
   return *value == 0;
}

void os_event_set (os_event_t * event, uint32_t value)
{
   xEventGroupSetBits ((EventGroupHandle_t)event, value);
}

void os_event_clr (os_event_t * event, uint32_t value)
{
   xEventGroupClearBits ((EventGroupHandle_t)event, value);
}

void os_event_destroy (os_event_t * event)
{
   vEventGroupDelete ((EventGroupHandle_t)event);
}

os_mbox_t * os_mbox_create (size_t size)
{
   QueueHandle_t handle = xQueueCreate (size, sizeof (void *));
   CC_ASSERT (handle);
   return (os_mbox_t *)handle;
}

bool os_mbox_fetch (os_mbox_t * mbox, void ** msg, uint32_t time)
{
   BaseType_t success;

   success = xQueueReceive ((QueueHandle_t)mbox, msg, TMO_TO_TICKS (time));
   return success != pdTRUE;
}

bool os_mbox_post (os_mbox_t * mbox, void * msg, uint32_t time)
{
   BaseType_t success;

   success = xQueueSendToBack ((QueueHandle_t)mbox, &msg, TMO_TO_TICKS (time));
   return success != pdTRUE;
}

void os_mbox_destroy (os_mbox_t * mbox)
{
   vQueueDelete ((QueueHandle_t)mbox);
}

static void os_timer_callback (TimerHandle_t xTimer)
{
   os_timer_t * timer = pvTimerGetTimerID (xTimer);

   if (timer->fn)
      timer->fn (timer, timer->arg);
}

os_timer_t * os_timer_create (
   uint32_t us,
   void (*fn) (os_timer_t *, void * arg),
   void * arg,
   bool oneshot)
{
   os_timer_t * timer;

   timer = malloc (sizeof (*timer));
   CC_ASSERT (timer != NULL);

   timer->fn  = fn;
   timer->arg = arg;
   timer->us  = us;

   timer->handle = xTimerCreate (
      "os_timer",
      (us / portTICK_PERIOD_MS) / 1000,
      oneshot ? pdFALSE : pdTRUE,
      timer,
      os_timer_callback);
   CC_ASSERT (timer->handle != NULL);
   return timer;
}

void os_timer_set (os_timer_t * timer, uint32_t us)
{
   timer->us = us;
}

void os_timer_start (os_timer_t * timer)
{
   /* Start timer by updating the period */
   BaseType_t status = xTimerChangePeriod (
         timer->handle,
         (timer->us / portTICK_PERIOD_MS) / 1000,
         portMAX_DELAY);

   CC_UNUSED (status);
   CC_ASSERT (status == pdPASS);
}

void os_timer_stop (os_timer_t * timer)
{
   BaseType_t status = xTimerStop (timer->handle, portMAX_DELAY);
   CC_UNUSED (status);
   CC_ASSERT (status == pdPASS);
}

void os_timer_destroy (os_timer_t * timer)
{
   BaseType_t status = xTimerDelete (timer->handle, portMAX_DELAY);
   CC_UNUSED (status);
   CC_ASSERT (status == pdPASS);
   free (timer);
}
