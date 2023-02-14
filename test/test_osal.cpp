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
#include <gtest/gtest.h>

static int expired_calls;
static void * expired_arg;
static void expired (os_timer_t * timer, void * arg)
{
   expired_calls++;
   expired_arg = arg;
}

class Osal : public ::testing::Test
{
 protected:
   virtual void SetUp()
   {
      expired_calls = 0;
   }
};

TEST_F (Osal, SemShouldTimeoutWhenCountIsZero)
{
   os_sem_t * sem = os_sem_create (2);
   bool tmo;

   // Count 2
   tmo = os_sem_wait (sem, 100);
   EXPECT_FALSE (tmo);

   // Count 1
   tmo = os_sem_wait (sem, 100);
   EXPECT_FALSE (tmo);

   // Count 0 - timeout
   tmo = os_sem_wait (sem, 100);
   EXPECT_TRUE (tmo);

   // Count 0 - timeout
   tmo = os_sem_wait (sem, 100);
   EXPECT_TRUE (tmo);

   // Increase count
   os_sem_signal (sem);

   // Count 1
   tmo = os_sem_wait (sem, 100);
   EXPECT_FALSE (tmo);

   // Count 0 - timeout
   tmo = os_sem_wait (sem, 100);
   EXPECT_TRUE (tmo);

   os_sem_destroy (sem);
}

TEST_F (Osal, EventShouldNotTimeout)
{
   os_event_t * event = os_event_create();
   uint32_t value     = 99;
   bool tmo;

   os_event_set (event, 1);
   tmo = os_event_wait (event, 1, &value, OS_WAIT_FOREVER);
   EXPECT_FALSE (tmo);
   EXPECT_EQ (1u, value);

   os_event_destroy (event);
}

TEST_F (Osal, EventShouldTimeout)
{
   os_event_t * event = os_event_create();
   uint32_t value     = 99;
   bool tmo;

   os_event_set (event, 2);
   tmo = os_event_wait (event, 1, &value, 100);
   EXPECT_TRUE (tmo);
   EXPECT_EQ (0u, value);

   os_event_destroy (event);
}

TEST_F (Osal, MboxShouldNotTimeout)
{
   os_mbox_t * mbox = os_mbox_create (2);
   void * msg;
   bool tmo;

   os_mbox_post (mbox, (void *)1, OS_WAIT_FOREVER);
   tmo = os_mbox_fetch (mbox, &msg, OS_WAIT_FOREVER);

   EXPECT_FALSE (tmo);
   EXPECT_EQ (1, (intptr_t)msg);

   os_mbox_destroy (mbox);
}

TEST_F (Osal, FetchFromEmptyMboxShouldTimeout)
{
   os_mbox_t * mbox = os_mbox_create (2);
   void * msg;
   bool tmo;

   tmo = os_mbox_fetch (mbox, &msg, 100);
   EXPECT_TRUE (tmo);

   os_mbox_destroy (mbox);
}

TEST_F (Osal, PostToFullMBoxShouldTimeout)
{
   os_mbox_t * mbox = os_mbox_create (2);
   bool tmo;

   os_mbox_post (mbox, (void *)1, 100);
   os_mbox_post (mbox, (void *)2, 100);
   tmo = os_mbox_post (mbox, (void *)3, 100);
   EXPECT_TRUE (tmo);

   os_mbox_destroy (mbox);
}

#define TEN_MS (10 * 1000)
#define CALLS   39

TEST_F (Osal, CyclicTimer)
{
   int t0, t1;
   os_timer_t * timer;

   timer = os_timer_create (TEN_MS, expired, (void *)0x42, false);
   os_timer_start (timer);

   t0 = os_get_current_time_us();
   os_usleep (CALLS * TEN_MS);
   t1 = os_get_current_time_us();
   os_timer_stop (timer);

   EXPECT_NEAR (CALLS * TEN_MS, t1 - t0, 1000);
   EXPECT_NEAR (CALLS, expired_calls, 2);

   EXPECT_EQ ((void *)0x42, expired_arg);

   // Check that timer is stopped
   expired_calls = 0;
   os_usleep (10 * TEN_MS);
   EXPECT_NEAR (0, expired_calls, 1);

   os_timer_destroy (timer);
}

TEST_F (Osal, OneshotTimer)
{
   os_timer_t * timer;

   timer = os_timer_create (TEN_MS, expired, (void *)0x42, true);

   os_timer_start (timer);
   os_usleep (10 * TEN_MS);

   EXPECT_EQ (1, expired_calls);
   EXPECT_EQ ((void *)0x42, expired_arg);

   os_timer_destroy (timer);
}

TEST_F (Osal, MultipleTimers)
{
   os_timer_t * timer1;
   os_timer_t * timer2;

   timer1 = os_timer_create (10 * 1000, expired, (void *)1, true);
   timer2 = os_timer_create (20 * 1000, expired, (void *)2, true);

   os_timer_start (timer1);
   os_timer_start (timer2);
   os_usleep (100 * 1000);

   EXPECT_EQ (2, expired_calls);
   EXPECT_EQ ((void *)2, expired_arg);

   os_timer_destroy (timer1);
   os_timer_destroy (timer2);
}

TEST_F (Osal, CurrentTime)
{
   uint32_t t0, t1;

   t0 = os_get_current_time_us();
   os_usleep (100 * 1000);
   t1 = os_get_current_time_us();

   EXPECT_NEAR (100 * 1000, t1 - t0, 1000);
}


TEST_F (Osal, Tick)
{
   os_tick_t t0, t1, sleep;

   sleep = os_tick_from_us (100 * 1000);
   t0 = os_tick_current();
   os_tick_sleep (sleep);
   t1 = os_tick_current();

   EXPECT_NEAR ((double)sleep,
                (double)(t1 - t0),
                (double)os_tick_from_us (1000));
}
