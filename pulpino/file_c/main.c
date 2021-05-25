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

int main()
{
	
}