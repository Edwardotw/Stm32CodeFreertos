#include "drv_dht11.h"
#include "bsp_usart.h"

//��λDHT11
static void DHT11_Rst(void)	   
{                 
	Dht11_Gpio_Init(OUT); 	//SET OUTPUT
    DHT11_DATA_OUT=0; 	//����DQ
    delay_xms(20);    	//��������18ms
    DHT11_DATA_OUT=1; 	//DQ=1 
	delay_us(30);     	//��������20~40us
}

//�ȴ�DHT11�Ļ�Ӧ
//����1:δ��⵽DHT11�Ĵ���
//����0:����
static DHT11_STATUS DHT11_Check(void) 	   
{   
	uint8_t retry=0;
	Dht11_Gpio_Init(IN);//SET INPUT	 
    while (DHT11_DATA_IN&&retry<100)//DHT11������40~80us
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=100)return STATUS_ERROR_SELFCHK;
	else retry=0;
    while (!DHT11_DATA_IN&&retry<100)//DHT11���ͺ���ٴ�����40~80us
	{
		retry++;
		delay_us(1);
	};
	if(retry>=100)return STATUS_ERROR_SELFCHK;	    
	return STATUS_OK;
}

//��DHT11��ȡһ��λ
//����ֵ��1/0
static uint8_t DHT11_Read_Bit(void) 			 
{
 	uint8_t retry=0;
	while(DHT11_DATA_IN&&retry<100)//�ȴ���Ϊ�͵�ƽ
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!DHT11_DATA_IN&&retry<100)//�ȴ���ߵ�ƽ
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);//�ȴ�40us
	if(DHT11_DATA_IN)return 1;
	else return 0;		   
}

//��DHT11��ȡһ���ֽ�
//����ֵ������������
static uint8_t DHT11_Read_Byte(void)    
{        
    uint8_t i,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1; 
	    dat|=DHT11_Read_Bit();
    }						    
    return dat;
}

//��DHT11��ȡһ������
//temp:�¶�ֵ(��Χ:0~50��)
//humi:ʪ��ֵ(��Χ:20%~90%)
//����ֵ��0,����;1,��ȡʧ��
DHT11_STATUS DHT11_Read_Data(float *temp,float *humi)    
{        
 	uint8_t buf[5];
	uint8_t i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//��ȡ40λ����
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{	
			*humi=(float)buf[0] + ((float)buf[1]) / 10;
			*temp=(float)buf[2] + ((float)buf[3]) / 10;
		}
	}else return STATUS_ERROR_READFAIL;
	return STATUS_OK;	    
}

//��ʼ��DHT11��IO�� DQ ͬʱ���DHT11�Ĵ���
//����1:������
//����0:����  
DHT11_STATUS Dht11_Init(void)
{
	Dht11_Gpio_Init(OUT);
	DHT11_Rst();
	return DHT11_Check();	
}

