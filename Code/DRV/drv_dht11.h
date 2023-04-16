#ifndef __DRV_DHT11_H
#define __DRV_DHT11_H	 
#include "sys.h"
#include "bsp_gpio.h"
#include "delay.h"

typedef enum
{
	STATUS_OK             = 0,
	STATUS_ERROR_SELFCHK  = 1,
	STATUS_ERROR_READFAIL = 2
}DHT11_STATUS;

#define DHT11_DATA_OUT PGout(11)// PG11
#define DHT11_DATA_IN  PGin(11)// PG11

extern DHT11_STATUS Dht11_Init(void);//≥ı ºªØ
extern DHT11_STATUS DHT11_Read_Data(float *temp,float *humi); 				    
#endif
