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

set(_osal_sys osal-rt-kernel)
add_library(${_osal_sys})

target_compile_features(${_osal_sys}
  PUBLIC
  c_std_99
  )

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
  )

target_link_libraries(${_osal_sys}
  PRIVATE
  osal-interface
  kern
  )

target_include_directories(${_osal_sys}
  PRIVATE
  src/rt-kernel
  )

install(
  TARGETS ${_osal_sys}
  EXPORT OsalTargets
  )
