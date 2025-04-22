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
#include "osal_utils.h"
#include <gtest/gtest.h>


class OsalUtilsDeathTest : public ::testing::Test
{
 protected:
   virtual void SetUp()
   {
   }
};

TEST_F (OsalUtilsDeathTest, ExitLater)
{
   const uint32_t delay_in_us = 1000 * 1000; /* 1.0 seconds */
   const uint32_t error_in_us =  100 * 1000; /* 0.1 seconds */
   ASSERT_DEATH_IF_SUPPORTED (
      {
         os_exit_later (1, "ExitLater", delay_in_us);
         os_usleep (delay_in_us + error_in_us);
      },
      ""
   );
}
