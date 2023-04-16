#include "drv_beep.h"   

//初始化PB8为输出口.并使能这个口的时钟		    
//蜂鸣器初始化
void BEEP_Init(void)
{
	Beep_Gpio_Init();
}

