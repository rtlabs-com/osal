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

#if defined(__rtk__)
#include "osal_cc_rtk.h"
#elif defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
#include "osal_cc_gcc.h"
#elif defined(_MSC_VER)
#include "osal_cc_msvc.h"
#else
#error "Unsupported compiler"
#endif
