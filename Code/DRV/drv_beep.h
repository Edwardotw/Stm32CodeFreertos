#ifndef __DRV_BEEP_H
#define __DRV_BEEP_H	 
#include "sys.h"
#include "bsp_gpio.h"

//�������˿ڶ���
#define BEEP PBout(8)	// BEEP,�������ӿ�		   

void BEEP_Init(void);	//��ʼ��
		 				    
#endif

