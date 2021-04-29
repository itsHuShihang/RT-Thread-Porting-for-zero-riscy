#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

typedef void (*irq_handler_t)(void);
extern irq_handler_t isrTable[32];
void SystemIrqHandler(uint32_t mcause);
void SystemIrqHandler_set(irq_handler_t userHandler, int vector);

#endif