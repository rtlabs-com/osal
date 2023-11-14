/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LWIP_HOOKS_H
#define LWIP_HOOKS_H

err_enum_t lwip_hook_unknown_eth_protocol (struct pbuf * pbuf, struct netif * netif);

#endif /* LWIP_HOOKS_H */