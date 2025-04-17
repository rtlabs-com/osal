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

#ifndef OSAL_UTILS_H
#define OSAL_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "osal.h"

void os_exit_later (os_exit_t code, const char * reason, uint32_t us);

#ifdef __cplusplus
}
#endif

#endif /* OSAL_UTILS_H */
