#ifndef __DRV_BEEP_H
#define __DRV_BEEP_H	 
#include "sys.h"
#include "bsp_gpio.h"

//蜂鸣器端口定义
#define BEEP PBout(8)	// BEEP,蜂鸣器接口		   

void BEEP_Init(void);	//初始化
		 				    
#endif

