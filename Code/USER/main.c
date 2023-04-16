//FreeRTOSͷ�ļ�
#include "FreeRTOS.h"	
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
//ϵͳͷ�ļ�
#include "sys.h"
#include "delay.h"
//оƬ����ͷ�ļ�
#include "bsp_usart.h"
#include "bsp_timer.h"
#include "bsp_wdg.h"
//������ͷ�ļ�
#include "drv_led.h"
#include "drv_key.h"
#include "drv_beep.h"
#include "drv_dht11.h"
//Ӧ�ò�ͷ�ļ�

//��׼ͷ�ļ�
#include "string.h"

//�����������
#define START_TASK_PRIO  1             //�������ȼ�
#define START_TASK_STKSIZE  50           //�����ջ��С������Ϊ��λ��4���ֽ�
TaskHandle_t START_TASK_HANDLER;        //������
void start_task(void* pvParameters);   //������

#define IWDG_TASK_PRIO  2              //�������ȼ�
#define IWDG_TASK_STKSIZE  50             //�����ջ��С������Ϊ��λ��4���ֽ�
TaskHandle_t IWDG_TASK_HANDLER;         //������
void iwdg_task(void* pvParameters);    //������

#define LED_TASK_PRIO  3              //�������ȼ�
#define LED_TASK_STKSIZE  256             //�����ջ��С������Ϊ��λ��4���ֽ�
TaskHandle_t LED_TASK_HANDLER;         //������
void led_task(void* pvParameters);    //������

#define KEY_TASK_PRIO  4              //�������ȼ�
#define KEY_TASK_STKSIZE  256             //�����ջ��С������Ϊ��λ��4���ֽ�
TaskHandle_t KEY_TASK_HANDLER;         //������
void key_task(void* pvParameters);    //������

#define TEST_TASK_PRIO  5              //�������ȼ�
#define TEST_TASK_STKSIZE  256             //�����ջ��С������Ϊ��λ��4���ֽ�
TaskHandle_t TEST_TASK_HANDLER;         //������
void test_task(void* pvParameters);    //������

//�����õ�����Ϣ�����ڴ˴���
QueueHandle_t UART1_QUEUE;       //��Ϣ���о��

typedef struct
{
	uint8_t model[3];
	uint8_t lot[8];
	uint8_t serial[5];
}SN;

//Ӳ����ʼ��
void HardWare_Init()
{
	DHT11_STATUS dht11_status;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	delay_init();	    //��ʱ������ʼ��	 
	uart1_init(9600);
	uart3_init(9600);
//	TIM3_Int_Init(7199,9999);
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
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
	HardWare_Init();   //��ЩӲ����ʼ������õ����У��紮���жϣ��������ڶ��д�����ɺ�Ӳ����ʼ��
	
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
	taskENTER_CRITICAL();  //�����ٽ���
	
	UART1_QUEUE = xQueueCreate(1,sizeof(SN)); //������ϢMessage_Queue,��������Ǵ��ڽ��ջ���������
	
    //����KEY����
    xTaskCreate((TaskFunction_t )key_task,     
                (const char*    )"key_task",   
                (uint16_t       )KEY_TASK_STKSIZE,
                (void*          )NULL,
                (UBaseType_t    )KEY_TASK_PRIO,
                (TaskHandle_t*  )&KEY_TASK_HANDLER); 
	//����LED����
    xTaskCreate((TaskFunction_t )led_task,     
                (const char*    )"led_task",   
                (uint16_t       )LED_TASK_STKSIZE,
                (void*          )NULL,
                (UBaseType_t    )LED_TASK_PRIO,
                (TaskHandle_t*  )&LED_TASK_HANDLER); 				

	//����IWDG����
    xTaskCreate((TaskFunction_t )iwdg_task,     
                (const char*    )"iwdg_task",   
                (uint16_t       )IWDG_TASK_STKSIZE,
                (void*          )NULL,
                (UBaseType_t    )IWDG_TASK_PRIO,
                (TaskHandle_t*  )&IWDG_TASK_HANDLER); 	
				
	//����IWDG����
    xTaskCreate((TaskFunction_t )test_task,     
                (const char*    )"test_task",   
                (uint16_t       )TEST_TASK_STKSIZE,
                (void*          )NULL,
                (UBaseType_t    )TEST_TASK_PRIO,
                (TaskHandle_t*  )&TEST_TASK_HANDLER); 	
				
	printf("Task Created!\r\n");			
	vTaskDelete(NULL);  //ɾ����ʼ���񣬲���ΪNULL��ʾɾ�������Լ���Ҳ����������
	taskEXIT_CRITICAL();     //�˳��ٽ���
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

