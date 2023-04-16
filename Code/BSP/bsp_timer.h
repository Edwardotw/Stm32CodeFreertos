#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H
#include "sys.h"
#include "FreeRTOS.h"					 
#include "queue.h"	

void TIM3_Int_Init(u16 arr,u16 psc);
 
#endif
