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

find_package(Threads)

option (USE_SCHED_FIFO
  "Use SCHED_FIFO policy. May require extra privileges to run"
  OFF)

set(_osal_sys osal-linux)
add_library(${_osal_sys})

target_compile_features(${_osal_sys}
  PUBLIC
  c_std_99
  )

# Use position independent code if platform supports shared libraries
get_property(SUPPORTS_SHARED GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS)
if (SUPPORTS_SHARED)
  set_property(TARGET ${_osal_sys} PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

target_sources(${_osal_sys}
  PRIVATE
  osal.c
  osal_log.c
  )

target_compile_options(${_osal_sys}
  PRIVATE
  -Wall
  -Wextra
  -Werror
  -Wno-unused-parameter
  $<$<BOOL:${USE_SCHED_FIFO}>:-DUSE_SCHED_FIFO>
  )

target_include_directories(${_osal_sys} PRIVATE
  sys
)

target_link_libraries(${_osal_sys}
  PUBLIC
  osal-interface
  Threads::Threads
  rt
  )

install(
  TARGETS ${_osal_sys}
  EXPORT OsalTargets
  )
