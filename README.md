# RT-Thread Nano for zero-riscy Core
## 简介
### 项目简介
本项目使用PULPino SoC，使用安路公司EG4系列FPGA开发板运行，移植了RT-Thread Nano操作系统内核。
### PULPino简介
PULPino是一款开源的单核SoC，是基于苏黎世联邦理工学院开发的32位RISC-V内核，PULP开发的，具有功耗低、面积小的特点，可配置为使用RISCY或zero-riscy核心。具体信息详见[PULPino GitHub](https://github.com/pulp-platform/pulpino).
### RT-Thread简介
RT-Thread是一个专门针对嵌入式设备的实时操作系统，主要采用C语言进行编写，较为浅显易懂，方便移植，并且可裁剪性非常好。具体信息详见[RT-Thread官网](https://www.rt-thread.org/)或[RT-Thread GitHub](https://github.com/RT-Thread/rt-thread).
## 如何使用
### 编译
本项目使用32位RISC-V GNU工具链，需要在Linux环境下安装此工具链进行交叉编译。安装方法间[这篇帖子](https://www.cnblogs.com/mikewolf2002/p/10799553.html)。当然其它版本的工具链应该也可以，只要是32位的RISC-V就行，但是我没试过，这里不做保证。
### 运行
调试较为快速的方式是使用串口直接烧写程序到RAM里，首先需要在FPGA开发板中烧写接收程序，这一部分是我的同学实现的，主要利用了逻辑代码中串口收发相关的函数，详见[这个仓库](https://github.com/HighLevelWallace/pulpino_project)。使用与硬件配套的软件对Verilog代码进行综合，然后烧写进去运行就好了。

接着把硬件串口连接到电脑上，在Linux环境下只需使用
```bash
sh run.sh
```
就可以自动完成编译和烧写的整套流程。
也可以单独使用
```bash
python C2SO.py
```
进行编译，并且使用
```bash
python serialout.py
```
进行烧写。

在`pulpino/file_c/main.c`文件中已经预先写好了几个示例程序，包括中断、os tick和有关线程管理的几个功能。在文件的最上方有一个宏定义的区域，可以通过打开或注释掉宏定义的方式来选择要运行的示例程序，但是需要注意，建议一次只打开一个宏定义，应为不同的示例程序一起运行可能会出现某些冲突导致错误。

如果要运行自己的应用程序，只需将示例程序删除，并且定义自己的主函数即可。RT-Thread Nano中没有操作系统和应用程序分离的功能，所以在自定义应用程序时只需在`pulpino/file_c`文件夹中添加自己需要的文件，并在`pulpino/file_c/main.c`文件中书写自己的主函数即可，不需要关心其它文件，但在编译时要带着包括操作系统文件在内的所有文件一起编译。

注意，如果需要额外添加文件夹，则需要在`C2SO.py`中修改编译脚本，否则将出现编译时遗漏文件的问题。
## 移植过程
### 文件结构
本项目的文件结构如下，
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
在一级目录下，具有pulpino和rtthread两个文件夹，其中pulpino文件夹中存放的是PULPino的裸机支持代码，而rtthread文件夹中存放的是RT-Thread操作系统所需的代码；C2SO.py, hex2hex2.py, 以及run.sh是项目编译以及文件格式转化脚本文件；libc.a和libgcc.a是项目编译所需要的库文件，link.ld则是项目链接文件；在pulpino文件夹下，file_c中存放的是PULPino裸机的C语言代码，而file_s下存放的是启动代码，是RISC-V汇编代码；在rtthread文件夹下，include文件夹下存放的是所有的头文件；libcpu文件夹下存放的则是与芯片移植相关的文件，其中包含头文件，C文件以及RISC-V汇编文件，因为在本项目中只有一款SoC，所以在libcpu下只有risc-v一个文件夹；src文件夹下存放的是RT-Thread所有的资源文件，均为C语言文件；而board.c和rtconfig.h则分别是板级的初始化C文件和RT-Thread的配置头文件。
### 启动代码移植
这部分代码位于`pulpino/file_s/crt0_riscv_E.S`中。

机器上电后的启动过程首先是由启动代码所规定的，PULPino硬件规定系统上电后从`0x80`位置启动，而在启动代码中此位置是一条跳转指令，直接跳转至`reset_handler`函数，`reset_handler`函数会对所有的寄存器进行清零初始化，并且将堆栈区的起始地址以及bss段的起始地址装载到相应的寄存器中，接着调用`main_entry`函数准备跳转到起始的C代码中。

在PULPino裸机运行时，`main_entry`函数会直接跳转至`main`函数，从而开始执行用户自定义的main函数中的代码；但在操作系统移植时，需要将此调用更改为跳转至`entry`函数，这个函数是RT-Thread操作系统在RISC-V GNU工具链下编译时的启动位置，进入了`entry`函数之后操作系统会对硬件架构进行区分，并且进行一系列的初始化工作。
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
从这里启动之后，会进入`rtthread/src/components.c`中的`entry()`函数，这个函数会进行一系列的初始化工作，正常情况下不需要对这些初始化代码进行移植，只需要对一些接口函数进行移植即可。
### 板级初始化移植
这部分移植的代码在`rtthread/board.c`中。

本项目中使用的时钟信号是固定的24MHz，所以`void riscv_clock_init(void)`函数是没用的，在启动流程中直接注释掉。

`static void ostick_config(rt_uint32_t ticks)`函数用来初始化os tick，实现原理是配置硬件定时器比较中断，在这个函数中需要对定时器比较中断的相关寄存器进行配置，将触发值设为传进来的参数，然后打开中断，将定时器清零。这里要注意，这个函数只是配置定时器比较中断，但不要打开定时器。上面的`void ISR_TA_CMP(void)`就是定时器比较中断的处理函数，这中断处理函数中对全局变量tick进行`+1`操作即可。

在`#define RT_HEAP_SIZE 2048`出配置动态堆的大小，注意要配合链接文件`link.ld`使用，两个地方分派给堆栈的大小要一致，避免爆栈。

剩余函数不需要动就好。
### 中断管理移植
理论上操作系统可以沿用裸机的中断处理方式，但是为了一致性，我们没有这么做，而是在操作系统中另外实现了一套中断管理程序。

在本项目中，有关中断管理的移植代码都在libcpu/risc-v文件夹下，相关的代码主要集中在`interrupt_gcc.S`, `interrupt.h`和`interrupt.c`这三个文件中。

`interrupt_gcc.S`文件是RISC-V汇编文件，主要定义了中断发生时硬件层面的操作。主要流程如下：
1. 开辟用来保存程序上下文的堆栈空间，并使用store指令将寄存器中的值保存到开辟的堆栈中。
2. 调用rt_interrupt_enter函数，通知内核进入中断，中断嵌套数加1.
3. 读取mcause和mepc寄存器的值，并将它们分别保存到a0和a1寄存器中，作为参数传入即将调用的中断处理函数中。
4. 调用SystemIrqHandler函数，进入RT-Thread操作系统统一中断程序入口函数，在SystemIrqHandler函数中会进一步根据中断向量号调用相应的中断处理函数。
5. 调用rt_interrupt_leave函数，通知内核离开中断，中断嵌套数减1.
6. 使用load指令将之前保存到堆栈的值取回寄存器中，并且释放相应的的堆栈空间。

`interrupt.h`文件是与中断处理相关的头文件，在这个头文件中首先定义了中断处理程序调用时所需的数据类型`irq_handler_t`，这是一个自定义的函数指针类型，用来声明与中断处理相关的函数指针；此外还对中断处理中所需要的两个函数进行了声明，分别是中断处理入口函数，`SystemIrqHandler(uint32_t mcause)`和用户自定义的中断处理程序挂载函数， `SystemIrqHandle_set(irq_handler_t userHandler, int vector)`。这样，需要进行中断调用的地方只需要包含interrupt.h文件，就可以正常的使用与中断管理有关的函数和数据类型。

`interrupt.c`文件中定义了上述的两个函数，具体代码如上图3-5所示。首先在第2行的位置定义了一个函数指针数组isrTable。从2.1 PULPino的基本结构中PULPino的中断向量表中可以看出，需要用户进行处理的中断是0号中断到31号中断，共32个，所以这里定义了一个32位的函数指针数组。在`SystemIrqHandler`函数中，调取了之前传入a0寄存器的参数`mcause`，这个参数的最高位若为1则说明产生了外部中断，而这个参数的最低5位则包含了硬件产生的中断向量号。第8行的操作就是取出了`mcause`的最高位，用来判断是否产生了中断，若产生了外部中断，则进行处理；第10行的操作则是取出了`mcause`的最低5位，即中断向量号，并保存到intNum变量中。在第13行中先将这个中断的标志位清零，并在第16行根据取出的中断向量号intNum通过函数指针数组`isrTable`调用相应的中断处理函数，从而执行用户自定义的中断处理操作。`SystemIrqHandler_set`函数的作用则是将用户自定义的中断处理函数挂载到`SystemIrqHandler`函数中，实现的方法是将用户的中断处理程序的函数指针userHandler和期望设定的中断的向量号vector作为参数传入`SystemIrqHandler_set`中，并将`userHandler`赋给相应的`isrTable[vector]`即可。

在用户书写应用程序时，若想自己定义一个中断处理程序，只需要先定义一个用户中断处理函数，并使用SystemIrqHandler_set函数将自己写的用户中断处理函数和相应的中断向量号挂载起来即可。可以较为方便的使用户实现对中断处理的自定义。

这里要注意，需要将`pulpino/file_s/crt0_riscv_E.S`中的中断向量表进行更改，把跳转至裸机定义下的中断服务程序的指令全部改成跳转到统一的`IRQ_Handler`的指令。但是中断向量号还是不变的，这是硬件决定的，不太好改。
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
### 上下文管理移植
在本项目中，上下文管理的移植代码主要在`context_gcc.S`和`cpuport.c`这两个文件中，分别以RISC-V汇编代码和C代码书写，主要用途是根据硬件的具体情况实现一些有关上线文切换的函数接口。RT-Thread的移植手册给出了需要实现的函数接口，如下表。
|  函数和变量  |  描述  |
|  ----  | ----  |
| rt_base_t rt_hw_interrupt_disable(void);  | 关闭全局中断 |
| void rt_hw_interrupt_enable(rt_base_t level);  | 打开全局中断|
| rt_uint8_t *rt_hw_stack_init(void *tentry, void *parameter, rt_uint8_t *stack_addr, void *texit);  | 线程栈的初始化，内核在线程创建和线程初始化里面会调用这个函数 |
| void rt_hw_context_switch_to(rt_uint32 to);  | 没有来源线程的上下文切换，在调度器启动第一个线程的时候调用 |
| void rt_hw_context_switch(rt_uint32 from, rt_uint32 to);  | 从 from 线程切换到 to 线程，用于线程和线程之间的切换 |
| void rt_hw_context_switch_interrupt(rt_uint32 from, rt_uint32 to);  | 从 from 线程切换到 to 线程，用于中断里面进行切换的时候使用 |
| rt_uint32_t rt_thread_switch_interrupt_flag;  | 表示需要在中断里进行切换的标志 |
| rt_uint32_t rt_interrupt_from_thread, rt_interrupt_to_thread; | 在线程进行上下文切换时候，用来保存 from 和 to 线程 |

在本项目中，打开和关闭中断都是使用改变csr寄存器mstatus中的值实现的。关闭中断时将mstatus寄存器的值保存到a0寄存器，并将mstatus的第3位置0；打开中断时则将a0寄存器中保存的值再写入mstatus中即可。线程栈的初始化是依靠线程创建时传入的堆栈地址参数进行堆栈空间的开辟，并将一些必要的参数值赋值到相应的堆栈地址处。线程切换函数主要是依靠load-store指令进行上下文的保存和取出，并利用call和jal指令跳转到所需的地址或者函数处。同时，需要在link.ld链接文件中暴露一定的堆栈地址的接口，再配合mhartid寄存器中储存的线程号，就可以在线程切换时通过不同线程堆栈的起始地址比较方便地调用不同线程的堆栈空间了。rt_interrupt_from_thread, rt_interrupt_to_thread和rt_thread_switch_interrupt_flag这三个变量主要是用来在线程切换时保存一些标志信息和线程的信息，可以在cpuport.c和context_gcc.S两个文件的不同函数中用作参数。具体去看`context_gcc.S`的代码就懂了。
### 链接文件
链接文件就是根目录下的`link.ld`文件，主要作用是对可执行文件在硬件中放置的位置进行规定，在链接命令中要调用这个文件。
```c
riscv32-unknown-elf-ld -T link.ld
```
`link.ld`的上半部分是关于RAM空间分配的代码，其中ORIGIN参数规定了各个RAM区的起始地址，而LENGTH参数规定了各个RAM区的大小；下半部分是关于SECTION空间分配的部分代码。一般情况下针对不同的硬件，只需要更改最上面的RAM分配命令即可。

### 关于print函数
这里我们使用的都是裸机代码里定义好的`printf`函数，可以正常使用。在操作系统里它定义了一个`rt_kprintf`的函数，并且留了一个没有定义内容的函数作为接口，可以通过实现这个函数来实现操作系统的`rt_kprintf`函数，不过这次毕设时间有限，还没来得及做这一步。