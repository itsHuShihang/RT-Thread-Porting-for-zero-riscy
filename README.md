# RT-Thread Nano for zero-riscy Core
查看此文档的[中文版本](README_CN.md)。
## Introduction
### Project Overview
This project utilizes the PULPino SoC and the Anlogic EG4 series FPGA development board to run the RT-Thread Nano operating system kernel.
### PULPino Overview
PULPino is an open-source single-core SoC based on a 32-bit RISC-V core developed by ETH Zurich and PULP. It is characterized by low power consumption and small area and can be configured to use either the RISCY or zero-riscy core. For more information, please refer to the [PULPino GitHub](https://github.com/pulp-platform/pulpino).
### RT-Thread Overview
RT-Thread is a real-time operating system specifically designed for embedded devices. It is mainly written in C language, easy to understand, and has good portability and configurability. For more information, please visit the [RT-Thread official website](https://www.rt-thread.org/) or [RT-Thread GitHub](https://github.com/RT-Thread/rt-thread).
## How to Use
### Compilation
This project uses the 32-bit RISC-V GNU toolchain and requires installation of this toolchain for cross-compilation on a Linux environment. You can follow the instructions in [this post](https://www.cnblogs.com/mikewolf2002/p/10799553.html). Other versions of the toolchain should also work as long as they are 32-bit RISC-V, but I haven't tested them, so I can't guarantee compatibility.
### Execution
A quick way to debug is to directly write the program to RAM via the serial port. First, you need to write the receiving program to the FPGA development board. This part has been implemented by my colleague and mainly utilizes the serial communication functions in the logic code. Please refer to [this repository](https://github.com/HighLevelWallace/pulpino_project) for more details. Synthesize the Verilog code using the software provided with the hardware, and then burn it to run.

Next, connect the hardware serial port to your computer. In a Linux environment, simply run:
```bash
sh run.sh
```
to automatically complete the compilation and burning process.

Alternatively, you can compile separately using:
```bash
python C2SO.py
```
to compile and use
```bash
python serialout.py
```
to burn.

Several sample programs, including interrupt handling, OS tick, and thread management, have been pre-written in the `pulpino/file_c/main.c` file. At the top of the file, there is a macro definition section where you can enable or disable specific macro definitions to choose which sample program to run. However, please note that it is recommended to enable only one macro definition at a time, as running different sample programs together may cause conflicts and errors.

To run your own application, simply delete the sample programs and define your own main function. In RT-Thread Nano, there is no separation between the operating system and application programs. Therefore, when customizing an application program, you only need to add the necessary files to the `pulpino/file_c` folder and write your own main function in the `pulpino/file_c/main.c` file. You don't need to worry about other files, but make sure to include all the files, including the operating system files, when compiling.

Note that if you need to add additional folders, you need to modify the compilation script in `C2SO.py` to avoid missing files during compilation.

## Porting Process
### File Structure
The file structure of this project is as follows,
```
RT-Thread Nano for PULPino
 |  
 +-- pulpino
 |  |  
 |  +-- file_c
 |  +-- file_s
 |  
 +-- rtthread
 |  |  
 |  +-- include
 |  +-- libcpu
 |  |   |
 |  |   +-- risc-v
 |  +-- src
 |  +-- board.c
 |  +-- rtconfig.h
 |    
 +-- C2SO.py
 |  
 +-- hex2hex2.py
 | 
 +-- serialout.py
 |  
 +-- run.sh
 |  
 +-- libc.a, libgcc.a
 | 
 +-- link.ld
```
In the top-level directory, there are two folders: "pulpino" and "rtthread." The "pulpino" folder contains the bare-metal support code for PULPino, while the "rtthread" folder contains the code required for the RT-Thread operating system. The files "C2SO.py," "hex2hex2.py," and "run.sh" are project compilation and file format conversion script files. "libc.a" and "libgcc.a" are library files required for project compilation, and "link.ld" is the project link file. 

Under the "pulpino" folder, the "file_c" directory contains the C language code for PULPino bare-metal, while the "file_s" directory contains the startup code, which is RISC-V assembly code. Under the "rtthread" folder, the "include" directory contains all the header files. The "libcpu" folder contains files related to chip porting, including header files, C files, and RISC-V assembly files. Since there is only one SoC in this project, there is only one "risc-v" folder under "libcpu." The "src" folder contains all the resource files for RT-Thread, which are C language files. The "board.c" and "rtconfig.h" files are respectively the board-level initialization C file and the RT-Thread configuration header file.

### Startup Code Porting
This portion of the code is located in `pulpino/file_s/crt0_riscv_E.S`.

The startup process after power-on of the machine is primarily determined by the startup code. In PULPino hardware, the system starts from the location `0x80` after power-on, and in the startup code, this location contains a jump instruction that directly jumps to the `reset_handler` function. The `reset_handler` function initializes all the registers by clearing them to zero and loads the starting address of the stack area and the starting address of the bss segment into the respective registers. It then calls the `main_entry` function to prepare for jumping to the initial C code.

When running PULPino bare-metal, the `main_entry` function directly jumps to the `main` function to start executing the user-defined code. However, when porting the operating system, this call needs to be changed to jump to the `entry` function. This function is the starting point for the RT-Thread operating system when compiled with the RISC-V GNU toolchain. After entering the `entry` function, the operating system distinguishes the hardware architecture and performs a series of initialization tasks.

```c
main_entry:
  addi   x10, x0, 0
  addi   x11, x0, 0x1
  jal  uart_set_cfg;

  /* jump to main program entry point (argc = argv = 0) */
  addi x10, x0, 0
  addi x11, x0, 0
  jal x1, entry    //jal to entry instead of main
  mv s0, a0
  jal  uart_wait_tx_done;
  mv a0, s0
```

After this point, the execution will enter the `entry()` function in `rtthread/src/components.c`. This function performs a series of initialization tasks, which generally do not require modification during the porting process. Only the interface functions need to be ported.

### Board-level Initialization Porting
This portion of the porting code is located in `rtthread/board.c`.

The clock signal used in this project is fixed at 24MHz, so the `void riscv_clock_init(void)` function is not needed and can be commented out in the startup process.

The `static void ostick_config(rt_uint32_t ticks)` function is used to initialize the OS tick. It configures the hardware timer compare interrupt. In this function, the relevant registers for the timer compare interrupt need to be configured by setting the trigger value to the parameter passed in, enabling the interrupt, and clearing the timer. It's important to note that this function only configures the timer compare interrupt and does not enable the timer. The `void ISR_TA_CMP(void)` function is the interrupt handler for the timer compare interrupt, and it simply increments the global variable `tick` by 1.

The `#define RT_HEAP_SIZE 2048` defines the size of the dynamic heap. It should be used in conjunction with the `link.ld` linker file, and the sizes allocated for the stack in both places should match to avoid stack overflow.

The remaining functions do not need to be modified.

### Interrupt Management Porting
In theory, an operating system can use the bare-metal interrupt handling approach, but for consistency, we implemented a separate set of interrupt management routines within the operating system.

In this project, the porting code related to interrupt management is located in the `libcpu/risc-v` directory. The relevant code is mainly concentrated in the files `interrupt_gcc.S`, `interrupt.h`, and `interrupt.c`.

The `interrupt_gcc.S` file is a RISC-V assembly file that defines the low-level hardware operations when an interrupt occurs. The main flow is as follows:
1. Allocate stack space to save the program context and use store instructions to save the register values to the allocated stack.
2. Call the `rt_interrupt_enter` function to notify the kernel that it has entered an interrupt, incrementing the interrupt nesting level.
3. Read the values of the `mcause` and `mepc` registers and save them to the `a0` and `a1` registers, respectively, to pass them as parameters to the upcoming interrupt handler function.
4. Call the `SystemIrqHandler` function to enter the unified interrupt handler of the RT-Thread operating system. In the `SystemIrqHandler` function, the appropriate interrupt handling function is called based on the interrupt vector number.
5. Call the `rt_interrupt_leave` function to notify the kernel that it is leaving the interrupt, decrementing the interrupt nesting level.
6. Use load instructions to retrieve the previously saved values from the stack back to the registers and free the corresponding stack space.

The `interrupt.h` file is the header file related to interrupt handling. In this header file, it first defines the data type `irq_handler_t`, which is a custom function pointer type used to declare function pointers related to interrupt handling. Additionally, it declares two functions required for interrupt handling: the interrupt entry function, `SystemIrqHandler(uint32_t mcause)`, and the user-defined interrupt handler mount function, `SystemIrqHandle_set(irq_handler_t userHandler, int vector)`. This allows places that need interrupt calls to include the `interrupt.h` file to use the functions and data types related to interrupt management.

The `interrupt.c` file defines the aforementioned two functions. In the code, there is a function pointer array called `isrTable` defined at line 2. From the PULPino's basic structure, it can be seen that there are 32 interrupts to be handled, ranging from interrupt 0 to interrupt 31. Therefore, a 32-bit function pointer array is defined here. In the `SystemIrqHandler` function, the parameter `mcause` passed in the `a0` register is retrieved. The highest bit of this parameter indicates whether an external interrupt occurred, and the lowest 5 bits contain the interrupt vector number generated by the hardware. The operation at line 8 checks if an interrupt occurred by checking the highest bit of `mcause`. If an external interrupt occurred, it is processed accordingly. The operation at line 10 retrieves the lowest 5 bits of `mcause`, which is the interrupt vector number, and saves it to the `intNum` variable. At line 13, the interrupt flag for this interrupt is cleared, and at line 16, the corresponding interrupt handling function is called through the function pointer array `isrTable` based on the retrieved interrupt vector number, thus executing the user-defined interrupt handling operation. The purpose of the `SystemIrqHandler_set` function is to mount the user-defined interrupt handling function to the `SystemIrqHandler` function. It achieves this by passing the function pointer `userHandler` of the user's interrupt handling program and the desired interrupt vector number `vector` as parameters to `SystemIrqHandler_set` and assigning `userHandler` to the corresponding `isrTable[vector]`.

When writing application programs, if you want to define your own interrupt handler, you just need to define a user interrupt handling function and use the `SystemIrqHandler_set` function to mount your own interrupt handling function with the corresponding interrupt vector number. This allows users to easily customize interrupt handling.

Note that you need to modify the interrupt vector table in `pulpino/file_s/crt0_riscv_E.S` and change the instructions that jump to the interrupt service routines defined in bare-metal to jump to the unified `IRQ_Handler` routine. However, the interrupt vector numbers remain unchanged as they are determined by the hardware and are not easily modified.
```c
  // external interrupts are handled by the same callback
  // until compiler supports IRQ routines
  .org 0x00
  jal x0, reset_handler
  .rept 21
  nop                       // unused
  .endr

  jal x0, IRQ_Handler    // 22: sram
  jal x0, IRQ_Handler    // 23: i2c
  jal x0, IRQ_Handler    // 24: uart
  jal x0, IRQ_Handler    // 25: gpio
  jal x0, IRQ_Handler    // 26: spim end of transmissionecall_insn_handler
  jal x0, IRQ_Handler    // 27: spim R/T finished
  jal x0, IRQ_Handler    // 28: timer A overflow
  jal x0, IRQ_Handler    // 29: timer A compare
  jal x0, IRQ_Handler    // 30: timer B overflow
  jal x0, IRQ_Handler    // 31: timer B compareecall_insn_handler
```
### Context Management Porting
In this project, the porting code for context management is mainly located in the `context_gcc.S` and `cpuport.c` files, written in RISC-V assembly code and C code, respectively. Their main purpose is to implement function interfaces related to context switching based on the specific hardware configuration. The RT-Thread porting manual provides the following function interfaces that need to be implemented:

| Function and Variable | Description |
| --- | --- |
| rt_base_t rt_hw_interrupt_disable(void); | Disable global interrupts |
| void rt_hw_interrupt_enable(rt_base_t level); | Enable global interrupts |
| rt_uint8_t *rt_hw_stack_init(void *tentry, void *parameter, rt_uint8_t *stack_addr, void *texit); | Initialize thread stack; called in thread creation and initialization in the kernel |
| void rt_hw_context_switch_to(rt_uint32 to); | Context switch without a source thread; called when starting the first thread in the scheduler |
| void rt_hw_context_switch(rt_uint32 from, rt_uint32 to); | Switch from the `from` thread to the `to` thread; used for thread-to-thread context switching |
| void rt_hw_context_switch_interrupt(rt_uint32 from, rt_uint32 to); | Switch from the `from` thread to the `to` thread; used for context switching within an interrupt |
| rt_uint32_t rt_thread_switch_interrupt_flag; | Flag indicating the need for context switching within an interrupt |
| rt_uint32_t rt_interrupt_from_thread, rt_interrupt_to_thread; | Used to save the `from` and `to` threads during thread context switching |

In this project, interrupt enable and disable operations are implemented by modifying the value of the `mstatus` register in the CSR register set. To disable interrupts, the value of the `mstatus` register is saved to the `a0` register, and the third bit of `mstatus` is cleared. To enable interrupts, the saved value in the `a0` register is written back to `mstatus`. The thread stack initialization relies on the stack address parameter passed during thread creation to allocate stack space and assign necessary parameter values to the corresponding stack address. The thread switching functions mainly use load-store instructions to save and restore contexts, and utilize call and jal instructions to jump to the desired address or function. Additionally, a certain interface of stack addresses needs to be exposed in the `link.ld` linker file, combined with the thread number stored in the `mhartid` register, to conveniently access the stack space of different threads during thread switching. The variables `rt_interrupt_from_thread`, `rt_interrupt_to_thread`, and `rt_thread_switch_interrupt_flag` are used to store flag information and thread information during thread switching, and can be used as parameters in different functions in `cpuport.c` and `context_gcc.S`. You can understand the details by examining the code in `context_gcc.S`.

### Linker File
The linker file is the `link.ld` file in the root directory, which specifies the placement of the executable file in the hardware and is invoked in the linker command:
```c
riscv32-unknown-elf-ld -T link.ld
```
The top portion of `link.ld` contains code related to RAM space allocation. The ORIGIN parameter specifies the start address of each RAM region, while the LENGTH parameter specifies the size of each RAM region. The bottom portion of the file contains code related to SECTION space allocation. In general, for different hardware configurations, only the RAM allocation commands at the top need to be modified.

### About the Print Function
Here, we are using the `printf` function defined in the bare-metal code, which can be used normally. In the operating system, it defines an `rt_kprintf` function and leaves an unimplemented function as an interface. By implementing this function, the operating system's `rt_kprintf` function can be implemented. However, due to time constraints in this project, this step has not been implemented yet.