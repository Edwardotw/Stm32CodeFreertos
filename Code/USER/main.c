//Github test at 2023/04/17 edit at web
//Github test at 2023/04/16
//FreeRTOS头文件
#include "FreeRTOS.h"	
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
//系统头文件
#include "sys.h"
#include "delay.h"
//芯片配置头文件
#include "bsp_usart.h"
#include "bsp_timer.h"
#include "bsp_wdg.h"
//驱动层头文件
#include "drv_led.h"
#include "drv_key.h"
#include "drv_beep.h"
#include "drv_dht11.h"
//应用层头文件

//标准头文件
#include "string.h"

//任务参数配置
#define START_TASK_PRIO  1             //任务优先级
#define START_TASK_STKSIZE  50           //任务堆栈大小，以字为单位，4个字节
TaskHandle_t START_TASK_HANDLER;        //任务句柄
void start_task(void* pvParameters);   //任务函数

#define IWDG_TASK_PRIO  2              //任务优先级
#define IWDG_TASK_STKSIZE  50             //任务堆栈大小，以字为单位，4个字节
TaskHandle_t IWDG_TASK_HANDLER;         //任务句柄
void iwdg_task(void* pvParameters);    //任务函数

#define LED_TASK_PRIO  3              //任务优先级
#define LED_TASK_STKSIZE  256             //任务堆栈大小，以字为单位，4个字节
TaskHandle_t LED_TASK_HANDLER;         //任务句柄
void led_task(void* pvParameters);    //任务函数

#define KEY_TASK_PRIO  4              //任务优先级
#define KEY_TASK_STKSIZE  256             //任务堆栈大小，以字为单位，4个字节
TaskHandle_t KEY_TASK_HANDLER;         //任务句柄
void key_task(void* pvParameters);    //任务函数

#define TEST_TASK_PRIO  5              //任务优先级
#define TEST_TASK_STKSIZE  256             //任务堆栈大小，以字为单位，4个字节
TaskHandle_t TEST_TASK_HANDLER;         //任务句柄
void test_task(void* pvParameters);    //任务函数

//所有用到的消息队列在此创建
QueueHandle_t UART1_QUEUE;       //信息队列句柄

typedef struct
{
	uint8_t model[3];
	uint8_t lot[8];
	uint8_t serial[5];
}SN;

//硬件初始化
void HardWare_Init()
{
	DHT11_STATUS dht11_status;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	delay_init();	    //延时函数初始化	 
	uart1_init(9600);
	uart3_init(9600);
//	TIM3_Int_Init(7199,9999);
	LED_Init();		  	//初始化与LED连接的硬件接口
	BEEP_Init();
	KEY_Init();
	delay_ms(1000);
	dht11_status = Dht11_Init();
	if(dht11_status == STATUS_OK) printf("dht11 check ok!\r\n");
	else printf("dht11 check fail %d\r\n",dht11_status);

	IWDG_Init(4,625);
}

int main(void)
{	
	HardWare_Init();   //有些硬件初始化后会用到队列（如串口中断），所以在队列创建完成后硬件初始化
	
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET)
	{
		printf("IWDG reset once!\r\n");
		BEEP = 1;
		RCC_ClearFlag();
	}
	else
	{
		BEEP = 0;
	}
	
	xTaskCreate((TaskFunction_t  )start_task,
				(const char*     )"start_task",
				(uint16_t        )START_TASK_STKSIZE,
				(void*           )NULL,
				(UBaseType_t     )START_TASK_PRIO,
				(TaskHandle_t*   )&START_TASK_HANDLER);
	vTaskStartScheduler();
}

void start_task(void* pvParameters)
{
	taskENTER_CRITICAL();  //进入临界区
	
	UART1_QUEUE = xQueueCreate(1,sizeof(SN)); //创建消息Message_Queue,队列项长度是串口接收缓冲区长度
	
    //创建KEY任务
    xTaskCreate((TaskFunction_t )key_task,     
                (const char*    )"key_task",   
                (uint16_t       )KEY_TASK_STKSIZE,
                (void*          )NULL,
                (UBaseType_t    )KEY_TASK_PRIO,
                (TaskHandle_t*  )&KEY_TASK_HANDLER); 
	//创建LED任务
    xTaskCreate((TaskFunction_t )led_task,     
                (const char*    )"led_task",   
                (uint16_t       )LED_TASK_STKSIZE,
                (void*          )NULL,
                (UBaseType_t    )LED_TASK_PRIO,
                (TaskHandle_t*  )&LED_TASK_HANDLER); 				

	//创建IWDG任务
    xTaskCreate((TaskFunction_t )iwdg_task,     
                (const char*    )"iwdg_task",   
                (uint16_t       )IWDG_TASK_STKSIZE,
                (void*          )NULL,
                (UBaseType_t    )IWDG_TASK_PRIO,
                (TaskHandle_t*  )&IWDG_TASK_HANDLER); 	
				
	//创建IWDG任务
    xTaskCreate((TaskFunction_t )test_task,     
                (const char*    )"test_task",   
                (uint16_t       )TEST_TASK_STKSIZE,
                (void*          )NULL,
                (UBaseType_t    )TEST_TASK_PRIO,
                (TaskHandle_t*  )&TEST_TASK_HANDLER); 	
				
	printf("Task Created!\r\n");			
	vTaskDelete(NULL);  //删除开始任务，参数为NULL表示删除任务自己，也可用任务句柄
	taskEXIT_CRITICAL();     //退出临界区
}

void key_task(void* pvParameters)
{
	uint8_t key;
	while(1)
	{
		key = KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES:
				vTaskSuspend(LED_TASK_HANDLER);
				break;
			case KEY1_PRES:
				vTaskResume(LED_TASK_HANDLER);
				break;
			case KEY2_PRES:
				
				break;
			case WKUP_PRES:
				
				break;
			default:
				break;
		}		
		vTaskDelay(10);
	}
}

void led_task(void* pvParameters)
{
	BaseType_t err;

	while(1)
	{
		LED0 = ~LED0;
		LED1 = ~LED1;
		vTaskDelay(500);
	}
}

void iwdg_task(void* pvParameters)
{
	while(1)
	{
		IWDG_Feed();
		vTaskDelay(500);
	}
}

void test_task(void* pvParameters)
{
	BaseType_t err;
	DHT11_STATUS dht11_status;
	float temp = 0.0;
	float humi = 0.0;
	uint8_t i;
	while(1)
	{
		taskENTER_CRITICAL();
		dht11_status = DHT11_Read_Data(&temp,&humi);
		taskEXIT_CRITICAL();
		if(dht11_status == STATUS_OK)
		{
			printf("temp=%.2f\r\n",temp);
			printf("humi=%.2f\r\n",humi);
		}
		else
		{
			printf("DHT11 Read Fail\r\n");
		}
		vTaskDelay(2000);
		
	}
}

