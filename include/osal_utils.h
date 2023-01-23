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

#ifndef OSAL_LOG_H
#define OSAL_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

/**
 * \brief Tick state for time updates
 *
 */
typedef struct os_tick_state
{
    uint32_t last;
    uint64_t base;
} os_tick_state_t;


/**
 * \brief Extend 32 bit tick to 64 bits without wrap
 *
 * \return Number of ticks extended to 64 bit
 */
static uint64_t os_tick_update (volatile os_tick_state_t * state_ptr, uint32_t tick)
{
   const uint64_t wrap = 1ull + UINT32_MAX;
   os_tick_state_t state;

   state.base = state_ptr->base;
   state.last = state_ptr->last;
   if (tick < state.last)
   {
      /* The tick wrapped around */
      state.base += wrap;
   }
   else if ((tick - state.last) > (UINT32_MAX / 2))
   {
      /* The tick is from before wrap, this can occur
         if the call was preempted after getting
         the current tick value and before evaulating
         previous tick relation. We undo the wrap
         wrap around, it will be re-done on next call */
      state.base -= wrap;
   }
   state.last = tick;
   state_ptr->base = state.base;
   state_ptr->last = state.last;

   return state.base + tick;
}


#ifdef __cplusplus
}
#endif

#endif /* OSAL_LOG_H */
