/*
 * Copyright (c) 2006-2021, Hu Shihang
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021/4/25     Hu Shihang    The first version
 */

#include "cpuport.h"

#define EXCEPTION_STACK_SIZE 128

  .section      .text.entry
  .align 2
  .global IRQ_Handler
IRQ_Handler:
    //STORE is sw, LOAD is lw
    /* save all from thread context */
    addi sp, sp, -EXCEPTION_STACK_SIZE

    STORE x1,   1 * REGBYTES(sp)
    li    t0,   0x80 // t0 is x5
    STORE t0,   2 * REGBYTES(sp)

    STORE x4,   4 * REGBYTES(sp)
    STORE x5,   5 * REGBYTES(sp)
    STORE x6,   6 * REGBYTES(sp)
    STORE x7,   7 * REGBYTES(sp)
    STORE x8,   8 * REGBYTES(sp)
    STORE x9,   9 * REGBYTES(sp)
    STORE x10, 10 * REGBYTES(sp)
    STORE x11, 11 * REGBYTES(sp)
    STORE x12, 12 * REGBYTES(sp)
    STORE x13, 13 * REGBYTES(sp)
    STORE x14, 14 * REGBYTES(sp)
    STORE x15, 15 * REGBYTES(sp)

    move  s0, sp

    /* switch to interrupt stack */
    // la：将某个内存中存的地址加载到寄存器中
    la    sp, _stack

    /* interrupt handle */
    call  rt_interrupt_enter
    csrr  a0, mcause
    csrr  a1, mepc
    mv    a2, sp
    call  SystemIrqHandler
    call  rt_interrupt_leave

    /* switch to from thread stack */
    move  sp, s0

    /* need to switch new thread */
    la    s0, rt_thread_switch_interrupt_flag
    lw    s2, 0(s0)
    beqz  s2, spurious_interrupt
    /* beqz指令: 若s2=0,则跳转到以spurious_interrupt为偏移量的地方 */

    /* clear switch interrupt flag */
    sw    zero, 0(s0)

    csrr  a0, mepc
    STORE a0, 0 * REGBYTES(sp)

    la    s0, rt_interrupt_from_thread
    LOAD  s1, 0(s0)
    STORE sp, 0(s1)

    la    s0, rt_interrupt_to_thread
    LOAD  s1, 0(s0)
    LOAD  sp, 0(s1)

    LOAD  a0,  0 * REGBYTES(sp)
    csrw  mepc, a0

spurious_interrupt:
    LOAD  x1,   1 * REGBYTES(sp)

    /* Remain in M-mode after mret */
    li    t0, 0x00001800
    csrs  mstatus, t0
    /* csrs指令：用t0的值按位对mstatus置q1 */
    LOAD  t0,   2 * REGBYTES(sp)
    csrs  mstatus, t0 

    LOAD  x4,   4 * REGBYTES(sp)
    LOAD  x5,   5 * REGBYTES(sp)
    LOAD  x6,   6 * REGBYTES(sp)
    LOAD  x7,   7 * REGBYTES(sp)
    LOAD  x8,   8 * REGBYTES(sp)
    LOAD  x9,   9 * REGBYTES(sp)
    LOAD  x10, 10 * REGBYTES(sp)
    LOAD  x11, 11 * REGBYTES(sp)
    LOAD  x12, 12 * REGBYTES(sp)
    LOAD  x13, 13 * REGBYTES(sp)
    LOAD  x14, 14 * REGBYTES(sp)
    LOAD  x15, 15 * REGBYTES(sp)

    addi  sp, sp, EXCEPTION_STACK_SIZE
    mret