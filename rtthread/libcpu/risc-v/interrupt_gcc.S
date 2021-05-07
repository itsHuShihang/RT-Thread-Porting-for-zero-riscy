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

    sw  x1, 0x5C(sp)

    sw  x3, 0x00(sp)  // gp
    sw  x4, 0x04(sp)  // tp
    sw  x5, 0x08(sp)  // t0
    sw  x6, 0x0c(sp)  // t1
    sw  x7, 0x10(sp)  // t2
    sw x10, 0x14(sp)  // a0
    sw x11, 0x18(sp)  // a1
    sw x12, 0x1c(sp)  // a2
    sw x13, 0x20(sp)  // a3
    sw x14, 0x24(sp)  // a4
    sw x15, 0x28(sp)  // a5

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
    LOAD  x1,  1 * REGBYTES(sp)

    /* Remain in M-mode after mret */
    li    t0, 0x00001800
    csrs  mstatus, t0
    /* csrs指令：用t0的值按位对mstatus置q1 */
    LOAD  t0,   2 * REGBYTES(sp)
    csrs  mstatus, t0 

    lw  x3, 0x00(sp)
    lw  x4, 0x04(sp)
    lw  x5, 0x08(sp)
    lw  x6, 0x0c(sp)
    lw  x7, 0x10(sp)
    lw x10, 0x14(sp)
    lw x11, 0x18(sp)
    lw x12, 0x1c(sp)
    lw x13, 0x20(sp)
    lw x14, 0x24(sp)
    lw x15, 0x28(sp)
    lw  x1, 0x5C(sp)

    addi sp, sp, EXCEPTION_STACK_SIZE
    mret