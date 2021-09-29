#********************************************************************
#        _       _         _
#  _ __ | |_  _ | |  __ _ | |__   ___
# | '__|| __|(_)| | / _` || '_ \ / __|
# | |   | |_  _ | || (_| || |_) |\__ \
# |_|    \__|(_)|_| \__,_||_.__/ |___/
#
# www.rt-labs.com
# Copyright 2021 rt-labs AB, Sweden.
#
# This software is licensed under the terms of the BSD 3-clause
# license. See the file LICENSE distributed with this software for
# full license information.
#*******************************************************************/

target_compile_options(cube PUBLIC
  -DUSE_HAL_DRIVER
  -DSTM32F769xx
  )

target_include_directories(cube PUBLIC
  $<BUILD_INTERFACE:${OSAL_SOURCE_DIR}/${CORE_DIR}/Inc>
  $<INSTALL_INTERFACE:bsp/Inc>

  ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include
  ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1
  ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2

  ${CUBE_DIR}/Drivers/CMSIS/Device/ST/STM32F7xx/Include
  ${CUBE_DIR}/Drivers/CMSIS/Include
  ${CUBE_DIR}/Drivers/STM32F7xx_HAL_Driver/Inc

  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/include
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/system

  ${CUBE_DIR}/Middlewares/Third_Party/FatFs/src
  )

target_include_directories(cube PRIVATE
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/posix
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/posix/net
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/posix/sys
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/stdc
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/include/netif
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/system/arch
  )

target_sources(cube PRIVATE
  ${CORE_DIR}/Src/bsp_driver_sd.c
  ${CORE_DIR}/Src/ethernetif.c
  ${CORE_DIR}/Src/fatfs.c
  ${CORE_DIR}/Src/fatfs_platform.c
  ${CORE_DIR}/Src/lwip.c
  ${CORE_DIR}/Src/sd_diskio.c

  ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c
  ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
  ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/list.c
  ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1/port.c
  ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
  ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/queue.c
  ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/tasks.c
  ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/timers.c

  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/api/api_lib.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/api/api_msg.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/api/err.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/api/if_api.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/api/netbuf.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/api/netdb.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/api/netifapi.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/api/sockets.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/api/tcpip.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/def.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/init.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/ip.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/ipv4/dhcp.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/ipv4/etharp.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/ipv4/icmp.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/ipv4/ip4.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_addr.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_frag.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/mem.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/memp.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/netif.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/pbuf.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/stats.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/tcp.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/tcp_in.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/tcp_out.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/timeouts.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/core/udp.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/src/netif/ethernet.c
  ${CUBE_DIR}/Middlewares/Third_Party/LwIP/system/OS/sys_arch.c

  ${CUBE_DIR}/Middlewares/Third_Party/FatFs/src/diskio.c
  ${CUBE_DIR}/Middlewares/Third_Party/FatFs/src/ff.c
  ${CUBE_DIR}/Middlewares/Third_Party/FatFs/src/ff_gen_drv.c
  ${CUBE_DIR}/Middlewares/Third_Party/FatFs/src/ff_gen_drv.c
  ${CUBE_DIR}/Middlewares/Third_Party/FatFs/src/option/ccsbcs.c
  ${CUBE_DIR}/Middlewares/Third_Party/FatFs/src/option/syscall.c

  ${CUBE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c
  ${CUBE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c
  ${CUBE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c
  ${CUBE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_eth.c
  ${CUBE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c
  ${CUBE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c
  ${CUBE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c
  ${CUBE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c
  ${CUBE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_sd.c
  ${CUBE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.c
  ${CUBE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim_ex.c
  ${CUBE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c
  ${CUBE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_sdmmc.c
  )

add_library(cube-bsp INTERFACE)
target_sources(cube-bsp INTERFACE
  $<BUILD_INTERFACE:${OSAL_SOURCE_DIR}/${CORE_DIR}/Src/freertos.c>
  $<BUILD_INTERFACE:${OSAL_SOURCE_DIR}/${CORE_DIR}/Src/main.c>
  $<BUILD_INTERFACE:${OSAL_SOURCE_DIR}/${CORE_DIR}/Src/startup_stm32f769xx.s>
  $<BUILD_INTERFACE:${OSAL_SOURCE_DIR}/${CORE_DIR}/Src/stm32f7xx_hal_msp.c>
  $<BUILD_INTERFACE:${OSAL_SOURCE_DIR}/${CORE_DIR}/Src/stm32f7xx_hal_timebase_tim.c>
  $<BUILD_INTERFACE:${OSAL_SOURCE_DIR}/${CORE_DIR}/Src/stm32f7xx_it.c>
  $<BUILD_INTERFACE:${OSAL_SOURCE_DIR}/${CORE_DIR}/Src/stubs.c>
  $<BUILD_INTERFACE:${OSAL_SOURCE_DIR}/${CORE_DIR}/Src/syscalls.c>
  $<BUILD_INTERFACE:${OSAL_SOURCE_DIR}/${CORE_DIR}/Src/system_stm32f7xx.c>

  $<INSTALL_INTERFACE:bsp/Src/freertos.c>
  $<INSTALL_INTERFACE:bsp/Src/main.c>
  $<INSTALL_INTERFACE:bsp/Src/startup_stm32f769xx.s>
  $<INSTALL_INTERFACE:bsp/Src/stm32f7xx_hal_msp.c>
  $<INSTALL_INTERFACE:bsp/Src/stm32f7xx_hal_timebase_tim.c>
  $<INSTALL_INTERFACE:bsp/Src/stm32f7xx_it.c>
  $<INSTALL_INTERFACE:bsp/Src/stubs.c>
  $<INSTALL_INTERFACE:bsp/Src/syscalls.c>
  $<INSTALL_INTERFACE:bsp/Src/system_stm32f7xx.c>
  )
target_link_options(cube-bsp INTERFACE
  -specs=nano.specs
  -T${OSAL_SOURCE_DIR}/${CORE_DIR}/STM32F769NIHx_FLASH.ld
  )
target_link_libraries(cube-bsp INTERFACE cube)
if (CMAKE_PROJECT_NAME STREQUAL OSAL AND BUILD_TESTING)
  target_link_libraries(osal_test PRIVATE cube-bsp)
endif()

install(TARGETS cube-bsp EXPORT OsalTargets)
install(DIRECTORY ${CORE_DIR}/Src DESTINATION bsp)
install(DIRECTORY ${CORE_DIR}/Inc DESTINATION bsp)
