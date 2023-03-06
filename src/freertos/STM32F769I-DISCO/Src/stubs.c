/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2018 rt-labs AB, Sweden.
 *
 * This software is licensed under the terms of the BSD 3-clause
 * license. See the file LICENSE distributed with this software for
 * full license information.
 ********************************************************************/

#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

char *getcwd(char *buf, size_t size)
{
   buf[0] = '/';
   buf[1] = 0;
   return buf;
}

int mkdir(const char *pathname, mode_t mode)
{
   return -1;
}

/* int _gettimeofday (struct timeval * ptimeval, void * ptimezone) */
/* { */
/*    return -1; */
/* } */
