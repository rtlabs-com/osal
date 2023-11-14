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

set(MCUX_DEVICE "MIMX8MM6")
set(MCUX_SDK_PROJECT_NAME mcuxsdk)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${OSAL_SOURCE_DIR}/${CORE_DIR}/Inc>
    $<INSTALL_INTERFACE:bsp/Inc>

    ${MCUXSDK_DIR}/rtos/freertos/freertos_kernel
    ${MCUXSDK_DIR}/rtos/freertos/freertos_kernel/include
    ${MCUXSDK_DIR}/rtos/freertos/freertos_kernel/portable/GCC/ARM_CM4F

    ${MCUXSDK_DIR}/middleware/lwip/src/include
    ${MCUXSDK_DIR}/middleware/lwip/port 
    ${MCUXSDK_DIR}/middleware/lwip/port/arch 

    ${MCUXSDK_DIR}/core/boards/evkmimx8mm
)

set(LWIP_DIR ${MCUXSDK_DIR}/middleware/lwip)

include(${MCUXSDK_DIR}/middleware/lwip/src/Filelists.cmake)

target_sources(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${MCUXSDK_DIR}/core/CMSIS/Core/Include/cmsis_gcc.h
    ${MCUXSDK_DIR}/core/boards/evkmimx8mm/board.c
    ${MCUXSDK_DIR}/core/boards/evkmimx8mm/clock_config.c 

    ${MCUXSDK_DIR}/rtos/freertos/freertos_kernel/tasks.c
    ${MCUXSDK_DIR}/rtos/freertos/freertos_kernel/timers.c
    ${MCUXSDK_DIR}/rtos/freertos/freertos_kernel/queue.c
    ${MCUXSDK_DIR}/rtos/freertos/freertos_kernel/list.c
    ${MCUXSDK_DIR}/rtos/freertos/freertos_kernel/event_groups.c
    ${MCUXSDK_DIR}/rtos/freertos/freertos_kernel/portable/GCC/ARM_CM4F/port.c
    ${MCUXSDK_DIR}/rtos/freertos/freertos_kernel/portable/MemMang/heap_3.c

    ${lwipnetif_SRCS}
    ${lwipapi_SRCS}
    ${lwipcore_SRCS}
    ${lwipcore4_SRCS}

    ${MCUXSDK_DIR}/middleware/lwip/port/enet_ethernetif.c
    ${MCUXSDK_DIR}/middleware/lwip/port/enet_ethernetif_imx.c
    ${MCUXSDK_DIR}/middleware/lwip/port/sys_arch.c 
)

set(CMAKE_MODULE_PATH
    ${MCUXSDK_DIR}/core/
    ${MCUXSDK_DIR}/core/drivers/common/
    ${MCUXSDK_DIR}/core/drivers/enet/
    ${MCUXSDK_DIR}/core/drivers/rdc/
    ${MCUXSDK_DIR}/core/drivers/iuart/
    ${MCUXSDK_DIR}/core/drivers/igpio/
    ${MCUXSDK_DIR}/core/components/phy/
    ${MCUXSDK_DIR}/core/components/phy/device/phyar8031/
    ${MCUXSDK_DIR}/core/components/phy/mdio/enet/
    ${MCUXSDK_DIR}/core/utilities/
    ${MCUXSDK_DIR}/core/utilities/utilities/debug_console_lite/
    ${MCUXSDK_DIR}/core/utilities/assert/
    ${MCUXSDK_DIR}/core/utilities/misc_utilities/
    ${MCUXSDK_DIR}/rtos/freertos/freertos_kernel/
    ${MCUXSDK_DIR}/core/components/serial_manager/
    ${MCUXSDK_DIR}/core/components/uart/
    ${MCUXSDK_DIR}/core/components/lists/
    ${MCUXSDK_DIR}/core/CMSIS/Core/Include/
)

#Include Entry cmake component
include(all_devices)

# include modules
include(driver_mdio-enet)
include(driver_phy-device-ar8031)
include(utility_debug_console_lite)
include(utility_assert_lite)
include(driver_clock)
include(driver_enet)
include(driver_common)
include(driver_rdc)
include(device_CMSIS)
include(component_iuart_adapter)
include(component_lists)
include(driver_iuart)
include(device_startup)
include(driver_igpio)
include(driver_mdio-common)
include(CMSIS_Include_core_cm)
include(driver_phy-common)
include(utilities_misc_utilities)
include(device_system)
include(middleware_freertos-kernel_MIMX8MM6)
include(CMSIS_Include_core_cm)
include(middleware_freertos-kernel_heap_3)

add_library(mcuxsdk-bsp INTERFACE)

target_sources(mcuxsdk-bsp INTERFACE 
    $<BUILD_INTERFACE:${OSAL_SOURCE_DIR}/${CORE_DIR}/Src/pin_mux.c>
    $<BUILD_INTERFACE:${OSAL_SOURCE_DIR}/${CORE_DIR}/Src/main.c>

    $<INSTALL_INTERFACE:bsp/Src/pin_mux.c>
    $<INSTALL_INTERFACE:bsp/Src/main.c>
)

target_link_options(mcuxsdk-bsp INTERFACE 
    -T${OSAL_SOURCE_DIR}/${CORE_DIR}/MIMX8MM6xxxxx_cm4_ddr_ram.ld
)

target_link_libraries(mcuxsdk-bsp INTERFACE ${MCUX_SDK_PROJECT_NAME})
if (CMAKE_PROJECT_NAME STREQUAL OSAL AND BUILD_TESTING)
  target_link_libraries(osal_test PRIVATE mcuxsdk-bsp)
endif()

install(TARGETS mcuxsdk-bsp EXPORT OsalTargets)
install(DIRECTORY ${CORE_DIR}/Src DESTINATION bsp)
install(DIRECTORY ${CORE_DIR}/Inc DESTINATION bsp)

