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

set(_osal_sys osal-freertos)
add_library(${_osal_sys})

target_include_directories(${_osal_sys} PRIVATE
  sys
)

target_compile_definitions(${_osal_sys}
  PUBLIC
  "OS_MAIN=int _main"
)

target_sources(${_osal_sys}
  PRIVATE
  osal.c
  osal_log.c
  )

target_link_libraries(${_osal_sys}
  PUBLIC
  osal-interface
  )

install(
  TARGETS ${_osal_sys}
  EXPORT OsalTargets
  )
