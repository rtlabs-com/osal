/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2025 rt-labs AB, Sweden.
 *
 * This software is licensed under the terms of the BSD 3-clause
 * license. See the file LICENSE distributed with this software for
 * full license information.
 ********************************************************************/

#include <string.h>
#include <inttypes.h>
#include "osal_utils.h"
#include "osal_log.h"

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

#define MICROSECONDS_PER_SECONDS 10000000u

typedef struct os_exit_later_ctx
{
    const char * reason;
    os_exit_t    code;
} os_exit_later_ctx_t;

static void os_exit_later_callback (os_timer_t *timer, void * arg)
{
    os_exit_later_ctx_t * ctx = arg;
    LOG_INFO (LOG_STATE_ON,
              "Exiting system due to: %s\n",
              ctx->reason);
    os_exit (ctx->code);
}

void os_exit_later (os_exit_t code, const char * reason, uint32_t us)
{
    os_exit_later_ctx_t * ctx;
    os_timer_t * tmr;
    LOG_INFO (LOG_STATE_ON,
              "System will exit in %"PRIu32" seconds due to: %s\n",
              us / MICROSECONDS_PER_SECONDS, reason);

    ctx = os_malloc (sizeof (os_exit_later_ctx_t));
    CC_ASSERT (ctx != NULL);

    memset (ctx, 0, sizeof (os_exit_later_ctx_t));
    ctx->code = code;
    ctx->reason = reason;

    tmr = os_timer_create (us, os_exit_later_callback, ctx, true);
    CC_ASSERT (tmr != NULL);

    os_timer_start (tmr);
}
