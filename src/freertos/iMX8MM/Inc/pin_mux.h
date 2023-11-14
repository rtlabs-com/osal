/*
 * Copyright 2022 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

#include "board.h"

#if defined(__cplusplus)
extern "C" {
#endif

void BOARD_InitBootPins(void);

void BOARD_InitPins(void);                                 

#if defined(__cplusplus)
}
#endif

#endif /* _PIN_MUX_H_ */

