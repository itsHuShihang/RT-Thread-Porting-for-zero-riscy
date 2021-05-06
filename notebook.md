# Notebook
## Interrupt
### Interrupt Flow
1. 保存现场
2. rt_interrupt_enter，通知内核进中断了
3. 中断处理函数
4. rt_interrupt_leave，通知内核出中断了
5. 恢复现场
### functions about enter and leave interrupt
`rt_interrupt_nest` is a global variable, 标志了中断嵌套层数

进中断
```c
void rt_interrupt_enter(void)
{
    rt_base_t level;

    level = rt_hw_interrupt_disable();
    rt_interrupt_nest ++;
    rt_hw_interrupt_enable(level);
}
```
出中断
```c
void rt_interrupt_leave(void)
{
    rt_base_t level;

    level = rt_hw_interrupt_disable();
    rt_interrupt_nest --;
    rt_hw_interrupt_enable(level);
}
```
挂载中断服务程序，将handler和vector关联起来
```c
rt_isr_handler_t rt_hw_interrupt_install(int vector,
                                        rt_isr_handler_t handler,
                                        void *param,
                                        char *name);
```
屏蔽中断和解除屏蔽
```c
void rt_hw_interrupt_mask(int vector);
void rt_hw_interrupt_umask(int vector);
```
全局中断锁和中断恢复
```c
rt_base_t rt_hw_interrupt_disable(void);
// 返回值是关中断前的中断状态

void rt_hw_interrupt_enable(rt_base_t level);
// 恢复调用rt_hw_interrupt_disable()函数前的中断状态
```
一个保证一行代码互斥运行的例子：
```c
    /* 关闭中断 */
    level = rt_hw_interrupt_disable();
    a = a + value;
    /* 恢复中断 */
    rt_hw_interrupt_enable(level);
```
## System tick
*ATTENTION:* RI5CY core use LPIT0 while zero-riscy core use LPIT1.

In zero-riscy core, has a LPIT1(low power periodic interval timer) module to implement a system tick with channel 0. If you want to know more details about it, you can find information in files called *system_RV32M1_zero_riscy.h* and *system_RV32M1_zero_riscy.c*. Every channel in LPIT1 has a 32bit timer for countdown. And when counting to 0, an interrupt flag will be setted to high level. Find more information in [this website](https://blog.csdn.net/whik1194/article/details/94066432).



显示PC指针是1228,但是不管怎么改handler都是1228`
`rt_hw_board_init()`函数调用失效,但是我觉得就是进1228的原因