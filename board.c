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
#include "rthw.h"
#include "gd32vf103.h"
#include "riscv_encoding.h"

#define TMR_MSIP 0xFFC
#define TMR_MSIP_size   0x4
#define TMR_MTIMECMP 0x8
#define TMR_MTIMECMP_size 0x8
#define TMR_MTIME 0x0
#define TMR_MTIME_size 0x8

#define TMR_CTRL_ADDR           0xd1000000
#define TMR_REG(offset)         _REG32(TMR_CTRL_ADDR, offset)
#define TMR_FREQ                ((uint32_t)SystemCoreClock/4)  //units HZ

void riscv_clock_init(void)
{
    SystemInit();

    /* ECLIC init */
    //ECLIC是GD32VF103里的改进型中断控制器，这里应该改成Pulpino里的中断控制器相关函数
    eclic_init(ECLIC_NUM_INTERRUPTS);
    eclic_mode_enable();
    set_csr(mstatus, MSTATUS_MIE);
}

static void ostick_config(rt_uint32_t ticks)
{
    /* set value */
    *(rt_uint64_t *)(TMR_CTRL_ADDR + TMR_MTIMECMP) = ticks;
    /* enable interrupt */
    eclic_irq_enable(CLIC_INT_TMR, 0, 0);
    /* clear value */
    *(rt_uint64_t *)(TMR_CTRL_ADDR + TMR_MTIME) = 0;
}

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)

//可以更改 RT_HEAP_SIZE 的大小，至少大于各个动态申请内存大小之和，但要小于芯片 RAM 总大小
#define RT_HEAP_SIZE 1024
static uint32_t rt_heap[RT_HEAP_SIZE];  // heap default size: 4K(1024 * 4)
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
    /* system clock Configuration */
    riscv_clock_init();

    /* OS Tick Configuration */
    ostick_config(TMR_FREQ / RT_TICK_PER_SECOND);

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

/* This is the timer interrupt service routine. */
void eclic_mtip_handler(void)
{
    /* clear value */
    *(rt_uint64_t *)(TMR_CTRL_ADDR + TMR_MTIME) = 0;

    /* enter interrupt */
    rt_interrupt_enter();
    /* tick increase */
    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}