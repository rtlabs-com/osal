#********************************************************************
#        _       _         _
#  _ __ | |_  _ | |  __ _ | |__   ___
# | '__|| __|(_)| | / _` || '_ \ / __|
# | |   | |_  _ | || (_| || |_) |\__ \
# |_|    \__|(_)|_| \__,_||_.__/ |___/
#
# www.rt-labs.com
# Copyright 2017 rt-labs AB, Sweden.
#
# This software is licensed under the terms of the BSD 3-clause
# license. See the file LICENSE distributed with this software for
# full license information.
#*******************************************************************/

target_sources(osal PRIVATE
  src/windows/osal.c
  src/windows/osal_log.c
  )

target_compile_options(osal
  PRIVATE
  /W4
  /WX
  /wd4100
  /wd4152
  PUBLIC
  /wd4200
  )

target_link_libraries(osal
  winmm)

target_include_directories(osal PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/windows>
  )

install(FILES
  src/windows/sys/osal_cc.h
  src/windows/sys/osal_sys.h
  DESTINATION include/sys
  )

if (BUILD_TESTING)
  set(GOOGLE_TEST_INDIVIDUAL TRUE)
endif()
