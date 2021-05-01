/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/10/01     Bernard      The first version
 */

#include <stdint.h>
#include "interrupt.h"
#include "../../include/rthw.h"
#include "../../../pulpino/file_c/event.h"
#include "../../../pulpino/file_c/int.h"

typedef void (*irq_handler_t)(void);
extern irq_handler_t isrTable[];

void SystemIrqHandler(uint32_t mcause)
{
    uint32_t intNum;

    if (mcause & 0x80000000) /* For external interrupt. get the value of the 31st bit*/
    {
        intNum = mcause & 0x1FUL; // get the values of the 0th~4th bits

        /* Clear pending flag in EVENT unit .*/
        ICP = (1U << intNum);

        /* Now call the real irq handler for intNum */
        isrTable[intNum]();
    }
}

void SystemIrqHandler_set(irq_handler_t userHandler, int vector)
{
    isrTable[vector] = userHandler;
}