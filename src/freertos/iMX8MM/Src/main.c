/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pin_mux.h"
#include "fsl_gpio.h"
#include "board.h"

#include <lwip/netif.h>
#include <string.h>

#include "enet_ethernetif.h"
#include "lwip/netifapi.h"
#include "lwip/tcpip.h"
#include "lwip/ip4_addr.h"

#include "fsl_enet_mdio.h"
#include "fsl_phyar8031.h"

#define PHY_ADDRESS 0x00u
#define ENET_CLOCK_FREQ 250000000

#define EXAMPLE_CLOCK_FREQ CLOCK_GetFreq(kCLOCK_IpgClk)

#define EXAMPLE_PHY_INTERFACE_RGMII
#ifndef PHY_AUTONEGO_TIMEOUT_COUNT
#define PHY_AUTONEGO_TIMEOUT_COUNT (100000)
#endif
#ifndef PHY_STABILITY_DELAY_US
#define PHY_STABILITY_DELAY_US (0U)
#endif

/*! @brief Stack size of the temporary lwIP initialization thread. */
#define INIT_THREAD_STACKSIZE 1024

/*! @brief Priority of the temporary lwIP initialization thread. */
#define INIT_THREAD_PRIO DEFAULT_THREAD_PRIO

/* MDIO operations. */
#define EXAMPLE_MDIO_OPS enet_ops
/* PHY operations. */
#define EXAMPLE_PHY_OPS phyar8031_ops

#ifndef EXAMPLE_NETIF_INIT_FN
/*! @brief Network interface initialization function. */
#define EXAMPLE_NETIF_INIT_FN ethernetif0_init
#endif /* EXAMPLE_NETIF_INIT_FN */

/* IP address configuration. */
#ifndef configIP_ADDR0
#define configIP_ADDR0 192
#endif
#ifndef configIP_ADDR1
#define configIP_ADDR1 168
#endif
#ifndef configIP_ADDR2
#define configIP_ADDR2 11
#endif
#ifndef configIP_ADDR3
#define configIP_ADDR3 3
#endif

/* Netmask configuration. */
#ifndef configNET_MASK0
#define configNET_MASK0 255
#endif
#ifndef configNET_MASK1
#define configNET_MASK1 255
#endif
#ifndef configNET_MASK2
#define configNET_MASK2 255
#endif
#ifndef configNET_MASK3
#define configNET_MASK3 0
#endif

/* Gateway address configuration. */
#ifndef configGW_ADDR0
#define configGW_ADDR0 192
#endif
#ifndef configGW_ADDR1
#define configGW_ADDR1 168
#endif
#ifndef configGW_ADDR2
#define configGW_ADDR2 11
#endif
#ifndef configGW_ADDR3
#define configGW_ADDR3 1
#endif

/* MAC address configuration. */
#define configMAC_ADDR                 \
  {                                    \
    0x00, 0x04, 0x9F, 0x05, 0xCF, 0xFC \
  }

static mdio_handle_t mdioHandle = {.ops = &EXAMPLE_MDIO_OPS};
static phy_handle_t phyHandle = {.phyAddr = PHY_ADDRESS, .mdioHandle = &mdioHandle, .ops = &EXAMPLE_PHY_OPS};

struct netif netif_mini;

__attribute ((weak)) int _main ()
{
  printf ("Hello, world\n");
  return 0;
}

static void stack_init(void *arg)
{
  ip4_addr_t netif_ipaddr;
  ip4_addr_t netif_netmask;
  ip4_addr_t netif_gw;

  ethernetif_config_t enet_config = {
      .phyHandle = &phyHandle,
      .macAddress = configMAC_ADDR,
  };

  mdioHandle.resource.csrClock_Hz = ENET_CLOCK_FREQ;

  IP4_ADDR(&netif_ipaddr, configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3);
  IP4_ADDR(&netif_netmask, configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3);
  IP4_ADDR(&netif_gw, configGW_ADDR0, configGW_ADDR1, configGW_ADDR2, configGW_ADDR3);

  tcpip_init(NULL, NULL);
  netifapi_netif_add(&netif_mini, &netif_ipaddr, &netif_netmask, &netif_gw, &enet_config, EXAMPLE_NETIF_INIT_FN,
                     &tcpip_input);

  netifapi_netif_set_default(&netif_mini);
  netifapi_netif_set_up(&netif_mini);

  _main ();

  /* Hang here if _main returns */
  for(;;)
  {
  }
}

int main(void)
{
  /* Hardware Initialization. */
  BOARD_InitMemory();
  BOARD_RdcInit();
  BOARD_InitPins();
  BOARD_BootClockRUN();
  BOARD_InitDebugConsole();

  CLOCK_SetRootDivider(kCLOCK_RootEnetAxi, 1U, 1U);
  CLOCK_SetRootMux(kCLOCK_RootEnetAxi, kCLOCK_EnetAxiRootmuxSysPll2Div4); /* SYSTEM PLL2 divided by 4: 250Mhz */

  /* Time stamp clock */
  CLOCK_SetRootDivider(kCLOCK_RootEnetTimer, 1U, 1U);
  CLOCK_SetRootMux(kCLOCK_RootEnetTimer, kCLOCK_EnetTimerRootmuxSysPll2Div10); /* SYSTEM PLL2 divided by 10: 100Mhz */

  /* mii/rgmii interface clock */
  CLOCK_SetRootDivider(kCLOCK_RootEnetRef, 1U, 1U);
  CLOCK_SetRootMux(kCLOCK_RootEnetRef, kCLOCK_EnetRefRootmuxSysPll2Div8); /* SYSTEM PLL2 divided by 8: 125MHz */

  gpio_pin_config_t gpio_reset_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
  gpio_pin_config_t gpio_led1_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
  gpio_pin_config_t gpio_led2_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
  gpio_pin_config_t gpio_button_config = {kGPIO_DigitalInput, 0, kGPIO_NoIntmode};

  /* Data LED pin */
  GPIO_PinInit(GPIO5, 13, &gpio_led1_config);
  /* Signal LED pin */
  GPIO_PinInit(GPIO5, 10, &gpio_led2_config);
  /* Botton pin */
  GPIO_PinInit(GPIO5, 12, &gpio_button_config);
  /* PHY reset pin */
  GPIO_PinInit(GPIO4, 22, &gpio_reset_config);

  /* PHY reset */
  GPIO_WritePinOutput(GPIO4, 22, 0);
  SDK_DelayAtLeastUs(10000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
  GPIO_WritePinOutput(GPIO4, 22, 1);
  SDK_DelayAtLeastUs(30000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

  EnableIRQ(ENET_MAC0_Rx_Tx_Done1_IRQn);
  EnableIRQ(ENET_MAC0_Rx_Tx_Done2_IRQn);

   /* Initialize lwIP from thread */
  if (sys_thread_new("main", stack_init, NULL, INIT_THREAD_STACKSIZE, INIT_THREAD_PRIO) == NULL)
  {
    LWIP_ASSERT("main(): Task creation failed.", 0);
  }

  vTaskStartScheduler();

  return 0;
}
