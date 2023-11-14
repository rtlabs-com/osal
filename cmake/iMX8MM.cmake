#********************************************************************
#        _       _         _
#  _ __ | |_  _ | |  __ _ | |__   ___
# | '__|| __|(_)| | / _` || '_ \ / __|
# | |   | |_  _ | || (_| || |_) |\__ \
# |_|    \__|(_)|_| \__,_||_.__/ |___/
#
# www.rt-labs.com
# Copyright 2021 rt-labs AB, Sweden.
# Copyright 2023 NXP
#
# This software is licensed under the terms of the BSD 3-clause
# license. See the file LICENSE distributed with this software for
# full license information.
#*******************************************************************/

target_sources(osal PRIVATE
  src/freertos/osal.c
  src/freertos/osal_log.c
  )

target_include_directories(osal PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/freertos/>
  )

install(FILES
  src/freertos/sys/osal_cc.h
  src/freertos/sys/osal_sys.h
  DESTINATION include/sys
  )

add_library(mcuxsdk)
target_link_libraries(osal mcuxsdk)
install(TARGETS mcuxsdk EXPORT OsalTargets)


set(CORE_DIR src/freertos/${BOARD})
include(MCUXSDK/${BOARD})
