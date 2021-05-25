#include "../../rtthread/include/rtthread.h"
#include "../../rtthread/libcpu/risc-v/interrupt.h"

void ISR_UART(void)
{
	printf("hello world! here's your character: %c\n", *(volatile int *)UART_REG_RBR);
	ICP |= 1 << 24;
}

int main(void)
{
	for (int i = 0; i < 8; i++)
	{
		set_gpio_pin_direction(i, 1);
		set_gpio_pin_value(i, 0);
	}
	int_disable();
	IER |= 1 << 24;
	SystemIrqHandler_set(ISR_UART, 24);
	uart_set_interrupt(1);
	int_enable();

	return 0;
}