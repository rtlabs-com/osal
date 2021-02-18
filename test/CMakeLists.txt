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

# Get osal properties
get_target_property(OSAL_SOURCES osal SOURCES)
get_target_property(OSAL_OPTIONS osal COMPILE_OPTIONS)
get_target_property(OSAL_INCLUDES osal INCLUDE_DIRECTORIES)
get_target_property(OSAL_LIBS osal LINK_LIBRARIES)

if (OSAL_LIBS STREQUAL "OSAL_LIBS-NOTFOUND")
  set(OSAL_LIBS "")
endif()

# Make source paths absolute
list(TRANSFORM OSAL_SOURCES PREPEND ${OSAL_SOURCE_DIR}/)

set_target_properties (osal_test
  PROPERTIES
  C_STANDARD 99
  CXX_STANDARD 11
  )

target_sources(osal_test PRIVATE
  # Units to be tested
  ${OSAL_SOURCES}

  # Unit tests
  test_osal.cpp

  # Testrunner
  osal_test.cpp
  )

target_compile_options(osal_test
  PRIVATE
  -DUNIT_TEST
  ${OSAL_OPTIONS}
  )

target_include_directories(osal_test
  PRIVATE
  ${OSAL_SOURCE_DIR}/include
  ${OSAL_INCLUDES}
  )

target_link_libraries(osal_test
  PRIVATE
  ${OSAL_LIBS}
  )

# No need for gmock
set(BUILD_GMOCK OFF CACHE BOOL "")
