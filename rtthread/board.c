/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-24     Tanek        the first version
 * 2018-11-12     Ernest Chen  modify copyright
 */
#include <stdint-gcc.h>
#include "include/rthw.h"
//#include "gd32vf103.h"  //change to pulpino.h
//#include "riscv_encoding.h"  //maybe useless
#include "../pulpino/file_s/pulpino.h"
#include "libcpu/risc-v/riscv-ops.h"
#include "../pulpino/file_c/int.h"
#include "../pulpino/file_c/event.h"
#include "../pulpino/file_c/timer.h"
#include "libcpu/risc-v/interrupt.h"
#include "../pulpino/file_c/gpio.h"


// need to change the address of these definition
// #define TMR_MSIP 0xFFC
// #define TMR_MSIP_size   0x4
// #define TMR_MTIMECMP 0x8
// #define TMR_MTIMECMP_size 0x8
// #define TMR_MTIME 0x0
// #define TMR_MTIME_size 0x8

//#define TMR_CTRL_ADDR           0xd1000000
#define SystemCoreClock 24000000  //units Hz
//#define TMR_REG(offset)         _REG32(TMR_CTRL_ADDR, offset)
#define TMR_FREQ                ((uint32_t)SystemCoreClock)  //units HZ

/* This is the timer interrupt service routine. */
void ISR_TA_CMP(void)
{
    /* clear value */
    //reset_timer();
    ICP = 1 << 29;

    /* enter interrupt */
    //rt_interrupt_enter();
    /* tick increase */
    rt_tick_increase();
    printf("tick:%d\n", rt_tick_get());
    /* leave interrupt */
    //rt_interrupt_leave();
}

void riscv_clock_init(void)
{
    //useless in zero-riscy
    //SystemInit();

    /* interrupt initialization */
    //clear event pending and interrupt pending
    ECP = 0xFFFFFFFF;
    ICP = 0xFFFFFFFF;
    int_enable();
    set_csr(mstatus, 0x08);
}

static void ostick_config(rt_uint32_t ticks)
{
    /* interrupt initialization */
    //clear event pending and interrupt pending
    ECP = 0xFFFFFFFF;
    ICP = 0xFFFFFFFF;

    /* set interrupt handler of timer compare */
    SystemIrqHandler_set(ISR_TA_CMP, 29);
    /* set value */
    //*(rt_uint64_t *)(TOCRA) = ticks;
    TOCRA = ticks;
    printf("ticks value:%d\n", ticks);
    /* enable timer A compare interrupt */
    IER = 1 << 29 | 1 << 28;
    int_enable();
    /* clear timer value */
    reset_timer();
}

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)

//可以更改 RT_HEAP_SIZE 的大小，至少大于各个动态申请内存大小之和，但要小于芯片 RAM 总大小
#define RT_HEAP_SIZE 2048// 2k
static uint32_t rt_heap[RT_HEAP_SIZE];  // heap default size: 8K(2048 * 4)  total 32k
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

/**
 * This function will initial your board.
 */
void rt_hw_board_init()
{
    printf("board init, start\n");//monitor
    /* system clock configuration is not necessary for pulpino*/
    //riscv_clock_init();

    /* OS Tick Configuration */
    ostick_config(TMR_FREQ / RT_TICK_PER_SECOND);
    printf("ostick config, done\n");//monitor
    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();

#endif

//使用宏定义选择是否打开内存堆功能，默认不打开，比较小巧
//开启则可以使用可以使用动态内存功能，如使用 rt_malloc、rt_free 以及各种系统动态创建对象的 API
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());

#endif
}