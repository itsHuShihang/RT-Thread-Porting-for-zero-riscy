#include "../../rtthread/include/rtthread.h"
#include "../../rtthread/libcpu/risc-v/interrupt.h"

// void ISR_UART(void)
// {
// 	printf("hello world! here's your character: %c\n", *(volatile int *)UART_REG_RBR);
// 	ICP |= 1 << 24;
// }

// int main(void)
// {
// 	for (int i = 0; i < 8; i++)
// 	{
// 		set_gpio_pin_direction(i, 1);
// 		set_gpio_pin_value(i, 0);
// 	}
// 	int_disable();
// 	SystemIrqHandler_set(ISR_UART, 24);
// 	uart_set_interrupt(1);
// 	IER |= 1 << 24;
// 	int_enable();

// 	return 0;
// }

// int main()
// {
// 	IER |= 1 << 29;
// 	int_enable();
// 	start_timer();
// 	while (1)
// 	{
// 		sleep();
// 	}
// 	return 0;
// }

#define THREAD_PRIORITY         3
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

static rt_thread_t tid1 = RT_NULL;

/* 线程 1 的入口函数 */
static void thread1_entry(void *parameter)
{
	printf("\nthread name is %s\n",rt_thread_self()->name);
	printf("thread init priority is %d\n",rt_thread_self()->init_priority);
	printf("thread current priority is %d\n",rt_thread_self()->current_priority);
	printf("thread1 exit\n");
}

ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
static struct rt_thread thread2;
/* 线程 2 入口 */
static void thread2_entry(void *param)
{
	printf("\nthread name is %s\n",rt_thread_self()->name);
	printf("thread init priority is %d\n",rt_thread_self()->init_priority);
	printf("thread current priority is %d\n",rt_thread_self()->current_priority);
    printf("thread2 exit\n");
    /* 线程 2 运行结束后也将自动被系统脱离 */
}

/* 线程示例 */
int thread_sample(void)
{
    /* 创建线程 1，名称是 thread1，入口是 thread1_entry*/
    tid1 = rt_thread_create("thread1",
                            thread1_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);

    /* 如果获得线程控制块，启动这个线程 */
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);
	else
	{
		printf("thread1 error\n");
	}

	/* 初始化线程 2，名称是 thread2，入口是 thread2_entry */
    rt_thread_init(&thread2,
                   "thread2",
                   thread2_entry,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),
                   THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    rt_thread_startup(&thread2);

    return 0;
}


int main(void)
{
	printf("\nThis is my main function\n");
	printf("thread name is %s\n",rt_thread_self()->name);
	printf("thread init priority is %d\n",rt_thread_self()->init_priority);
	printf("thread current priority is %d\n",rt_thread_self()->current_priority);

	thread_sample();
	rt_thread_yield();

	printf("\nwelcome back to main function\n");
	printf("thread name is %s\n",rt_thread_self()->name);
	printf("thread init priority is %d\n",rt_thread_self()->init_priority);
	printf("thread current priority is %d\n",rt_thread_self()->current_priority);

	return 0;
}