#include "../../rtthread/include/rtthread.h"
#include "../../rtthread/libcpu/risc-v/interrupt.h"

/* you can choose one demo function here */
//#define UART_INTERRUPT_SAMPLE
//#define OS_TICK_SAMPLE
//#define THREAD_SWITCH_SAMPLE
#define MAIL_BOX_SAMPLE



/* main function */
int main()
{
	for (int i = 0; i < 8; i++)
	{
		set_gpio_pin_direction(i, 1);
		set_gpio_pin_value(i, 0);
	}
#ifdef OS_TICK_SAMPLE
	os_tick();
#endif
#ifdef THREAD_SWITCH_SAMPLE
	thread_switch();
#endif
#ifdef UART_INTERRUPT_SAMPLE
	int_disable();
	SystemIrqHandler_set(ISR_UART, 24);
	uart_set_interrupt(1);
	IER |= 1 << 24;
	int_enable();
#endif
#ifdef MAIL_BOX_SAMPLE
	mailbox_sample();
	rt_thread_yield();
#endif

	return 0;
}

/* function definitions */
#ifdef UART_INTERRUPT_SAMPLE
void ISR_UART(void)
{
	printf("hello world! here's your character: %c\n", *(volatile int *)UART_REG_RBR);
	ICP |= 1 << 24;
}
#endif

#ifdef OS_TICK_SAMPLE
int os_tick()
{
	IER |= 1 << 29;
	int_enable();
	start_timer();
	while (1)
	{
		sleep();
	}
	return 0;
}
#endif

#ifdef THREAD_SWITCH_SAMPLE
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


int thread_switch(void)
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
#endif

#ifdef MAIL_BOX_SAMPLE
#define THREAD_PRIORITY      3
#define THREAD_TIMESLICE     5

/* 邮箱控制块 */
static struct rt_mailbox mb;
/* 用于放邮件的内存池 */
static char mb_pool[128];

static char mb_str1[] = "I'm a mail!";
static char mb_str2[] = "this is another mail!";
static char mb_str3[] = "over";

ALIGN(RT_ALIGN_SIZE)
static char thread1_stack[1024];
static struct rt_thread thread1;

/* 线程 1 入口 */
static void thread1_entry(void *parameter)
{
    char *str;

    while (1)
    {
        printf("thread1: try to recv a mail\n");

        /* 从邮箱中收取邮件 */
        if (rt_mb_recv(&mb, (rt_uint32_t *)&str, RT_WAITING_FOREVER) == RT_EOK)
        {
            printf("thread1: get a mail from mailbox, the content:%s\n", str);
            if (str == mb_str3)
                break;

            /* 延时 */
			for (int j = 0; j < 10000;j++);
		}
	}
    /* 执行邮箱对象脱离 */
    rt_mb_detach(&mb);
}

ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
static struct rt_thread thread2;

/* 线程 2 入口 */
static void thread2_entry(void *parameter)
{
    rt_uint8_t count;

    count = 0;
    while (count < 10)
    {
        count ++;
        if (count & 0x1)
        {
            /* 发送 mb_str1 地址到邮箱中 */
            rt_mb_send(&mb, (rt_uint32_t)&mb_str1);
        }
        else
        {
            /* 发送 mb_str2 地址到邮箱中 */
            rt_mb_send(&mb, (rt_uint32_t)&mb_str2);
        }

        /* 延时 */
        for (int j = 0; j < 20000;j++);
    }

    /* 发送邮件告诉线程 1，线程 2 已经运行结束 */
    rt_mb_send(&mb, (rt_uint32_t)&mb_str3);
}

int mailbox_sample(void)
{
    rt_err_t result;

    /* 初始化一个 mailbox */
    result = rt_mb_init(&mb,
                        "mbt",                      /* 名称是 mbt */
                        &mb_pool[0],                /* 邮箱用到的内存池是 mb_pool */
                        sizeof(mb_pool) / 4,        /* 邮箱中的邮件数目，因为一封邮件占 4 字节 */
                        RT_IPC_FLAG_FIFO);          /* 采用 FIFO 方式进行线程等待 */
    if (result != RT_EOK)
    {
        printf("init mailbox failed.\n");
        return -1;
    }

    rt_thread_init(&thread1,
                   "thread1",
                   thread1_entry,
                   RT_NULL,
                   &thread1_stack[0],
                   sizeof(thread1_stack),
                   THREAD_PRIORITY, THREAD_TIMESLICE);
    rt_thread_startup(&thread1);

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
#endif