/*
#include "sram.h"
#include "stdlib.h"
#include "gpio.h"
#include "uart.h"
#include "string_lib.h"
#include "spi.h"

unsigned char state = ADJUST;//begin to adjust
unsigned char dir = UP;
unsigned int D_avg_pre = 0xFFFFFFFF;
unsigned int err_pre = 0xFFFFFFFF;

char * hex_to_string(unsigned int hex)
{
	char numchar[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	 'A', 'B', 'C', 'D', 'E', 'F'};
	static char str[9];

	for(int i = 0; i < 8; ++i)
		str[i] = numchar[(hex >> (4 * i)) & 0x0F];
	str[8] = '\0';
	return str;
}

void main()
{
	//configure();
	//for(int i = 0;i < 4;i ++){
	//	set_gpio_pin_direction(i,1);
	//	set_gpio_pin_value(i,1);
	//}
  	//for(int i = 4;i < 8;i ++){
	//	set_gpio_pin_direction(i,1);
	//	set_gpio_pin_value(i,0);
  	//}
	//while(1);
	
	char str[] = {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '\n', '\0'};
	int addr;
	char c;
	unsigned int id[3];
	for(int i = 0; i < 8; i++)
	{
		set_gpio_pin_direction(i,1);
		set_gpio_pin_value(i, 1);
	}

	uart_set_cfg(0, 77);  //Core clock 12MHz, baud rate 9600 bps
	
	uart_send("hello world!\n", 12);

	spi_setup_master(1);
	for (int i = 0; i < 3000; i++) {
    // //wait some time to have proper power up of external flash
    // #ifdef __riscv__
    //     asm volatile ("nop");
    // #else
    //     asm volatile ("l.nop");
    // #endif
 	}

	// divide sys clock by 4
	*(volatile int*) (SPI_REG_CLKDIV) = 0x4;

	// if (check_spi_flash()) {
	// 	uart_send("ERROR: Spansion SPI flash not found\n", 36);
	// 	while (1);
	// }

	uart_send("hello world!\n", 13);

	uart_send("Release Power down / Device ID\n", 31);

	spi_setup_dummy(24, 0);
	spi_setup_cmd_addr(0xAB, 8, 0, 0);
	spi_set_datalen(64);
	spi_start_transaction(SPI_CMD_RD, SPI_CSN0);
	spi_read_fifo(id, 64);

	uart_send(hex_to_string(id[0]), 8);
	uart_send("\r\n", 2);

	uart_send("JEDEC ID\n", 31);

	spi_setup_dummy(0, 0);
	spi_setup_cmd_addr(0x9F, 8, 0, 0);
	spi_set_datalen(64);
	spi_start_transaction(SPI_CMD_RD, SPI_CSN0);
	spi_read_fifo(id, 64);

	uart_send(hex_to_string(id[0]), 8);
	uart_send(hex_to_string(id[1]), 8);
	uart_send(hex_to_string(id[2]), 8);
	uart_send("\r\n", 2);
	
	while(1)
	{
		uart_send("hello world!\n", 13);
		for(int i = 0; i < 1000000; i++);
	}
}


void configure(void)
{
	ECP = 0xFFFFFFFF;
	IER = (1 << 29) | (1 << 22);//
	int_enable();
	  // enable timer and wait for 1000 cycles before triggering
	TPRA  = 0x0;
	TIRA  = 0x0;
	TOCRA = 0x000FFFFF;
	//TPRA  = 0x1;
}


void reset_ssram(void)
{
	SACR = 0;
	SCCR = 0;
	SECR = 0;
}

void ISR_TA_CMP(void)
{
	ICP = 1 << 29;
	SCFR = SCFR | (1 << 16);//start counter
	TPRA  = 0x0;//stop counter
}

void ISR_SRAM (void)
{
	unsigned int D_avg_cur;
	unsigned int access_count, total_time, err_cur;
	unsigned int tbl = SCFR & (0x00000FFF);//get tbl
	ICP = 1 << 22;
	access_count = SACR;
	total_time = SCCR;
	err_cur = SECR;
	D_avg_cur =  total_time / access_count;
	if(state)//adjust
	{
		if(D_avg_cur < D_avg_pre)
		{
			D_avg_pre = D_avg_cur;
			if(dir = UP)
			{
				tbl += 1;
			}
			else
			{
				tbl -= 1; 
			}
			err_pre = err_cur;
			SCFR = SCFR | (1 << 16);//start SRAM
		}
		else
		{
			if(dir = UP)
			{
				tbl -= 1;
			}
			else
			{
				tbl += 1; 
			}
			state = MONITOR;
			REG_FLL = 0x00100233;//<----------adjust clock
			TPRA  = 0x1;
			SCFR = SCFR & 0xFFFEFFFF;//stop count:set bit 16 as 0
		}
		reset_ssram();//reset ssram model,write tbl
		SCFR = (SCFR & (0xFFFFF000)) | tbl;//write tbl

	}
	else
	{
		if(abs(err_cur - err_pre) * 100 < 3 * err_cur)
		{
			TPRA  = 0x1;//start timer
		}
		else
		{
			TPRA  = 0x0;//close timer----->adjust
			if(err_cur < err_pre)
			{
				tbl -= 1;
				dir = DOWN;
			}
			else
			{
				REG_FLL = 0x00100277;//<----------adjust clock
				tbl += 1;
				dir = UP;
			}
			state = ADJUST;
			SCFR = (SCFR & (0xFFFFF000)) | tbl;//write tbl
			SCFR = SCFR | (1 << 16);//start counter
		}
		reset_ssram();//reset ssram model,write tbl
	}
}*/


#include "../../rtthread/include/rtthread.h"

/**
 * \brief This is main function.
 * \param void
 * \return 0
*/
// int main(void)
// {
// 	printf("This is my main function\n");
// 	for (int i = 0; i < 8; i++)
// 	{
// 		set_gpio_pin_direction(i, 1);
// 		set_gpio_pin_value(i, 0);
// 	}

// 	ECP = 0xFFFFFFFF;
// 	ICP = 0xFFFFFFFF;
// 	start_timer();

// 	int j = 0;
// 	while (1)
// 	{
// 		sleep();
// 		// set_gpio_pin_value(j + 1, 1);
// 		// set_gpio_pin_value(j, 0);
// 		// j++;
// 		// j = j % 8;
// 		// rt_thread_delay(100);
// 	}
// 	return 0;
// }

#define THREAD_PRIORITY         3
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        8

static rt_thread_t tid1 = RT_NULL;

/* 线程 1 的入口函数 */
static void thread1_entry(void *parameter)
{
	printf("\nthread name is %s\n",rt_thread_self()->name);
	printf("thread init priority is %d\n",rt_thread_self()->init_priority);
	printf("thread current priority is %d\n",rt_thread_self()->current_priority);

    rt_uint32_t count = 0;

	for (count = 0; count < 10; count++)
	{
        /* 线程 1 采用低优先级运行，一直打印计数值 */
		printf("thread1 count: %d\n", count);
		for (int i = 0; i < 1000000; ++i);
	}
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

    rt_uint32_t count = 0;

    /* 线程 2 拥有较高的优先级，以抢占线程 1 而获得执行 */
    for (count = 0; count < 10 ; count++)
    {
        /* 线程 2 打印计数值 */
        printf("thread2 count: %d\n", count);
		for(int i = 0; i < 800000; ++i);
    }
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


// int main(void)
// {
// 	printf("This is my main function\n");
// 	printf("\nthread name is %s\n",rt_thread_self()->name);
// 	printf("thread init priority is %d\n",rt_thread_self()->init_priority);
// 	printf("thread current priority is %d\n",rt_thread_self()->current_priority);

// 	thread_sample();
// 	rt_thread_yield();

// 	printf("\nwelcome back to main function\n");
// 	printf("thread name is %s\n",rt_thread_self()->name);
// 	printf("thread init priority is %d\n",rt_thread_self()->init_priority);
// 	printf("thread current priority is %d\n",rt_thread_self()->current_priority);

// 	return 0;
// }

int main(void)
{
	printf("This is my main function\n");
	for (int i = 0; i < 8; i++)
	{
		set_gpio_pin_direction(i, 1);
		set_gpio_pin_value(i, 0);
	}

	ECP = 0xFFFFFFFF;
	ICP = 0xFFFFFFFF;
	//start_timer();

	thread_sample();

	return 0;
}