#ifndef Y_LCD_UART_H
#define Y_LCD_UART_H

#include "reg51.h"
#include "uart.h"
#include <string.h>


#define PARITYBIT2 NONE_PARITY   //����У��λ

void uart2_init();
void GpuSend(char *s);
//void Uasrt2SendData(BYTE dat);

#endif