#ifndef __BSP_USART_H
#define __BSP_USART_H
#include "stdio.h"	
#include "sys.h" 
#include "bsp_gpio.h"

#define UART1_RX_LEN  50 
#define UART3_RX_LEN  50

extern void uart1_init(u32 bound);
extern void uart3_init(u32 bound);
extern void UART1_SendData(uint8_t *data_buf,uint8_t len);
extern void UART3_SendData(uint8_t *data_buf,uint8_t len);
#endif


