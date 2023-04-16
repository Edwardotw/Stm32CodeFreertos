#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H	 
#include "sys.h"
typedef enum
{
	IN = 0,
	OUT = 1
}GPIO_DIR;

extern void Beep_Gpio_Init(void);
extern void Key_Gpio_Init(void);
extern void Led_Gpio_Init(void);
extern void Uart1_Gpio_Init(void);
extern void Uart3_Gpio_Init(void);
extern void Dht11_Gpio_Init(GPIO_DIR dir);

#endif

