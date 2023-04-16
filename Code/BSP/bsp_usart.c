#include "bsp_usart.h"	  
#include "FreeRTOS.h"					 
#include "queue.h"					 				 

uint8_t Uart1_Rx_Buf[UART1_RX_LEN];
uint8_t Uart1_Rx_Cnt=0;
uint8_t Uart1_Rx_Flag=0;

uint8_t Uart3_Rx_Buf[UART3_RX_LEN];
uint8_t Uart3_Rx_Cnt=0;
uint8_t Uart3_Rx_Flag=0;

void uart1_init(u32 bound)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//GPIO端口设置
	Uart1_Gpio_Init();	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//使能USART1时钟
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//开启串口接受中断
	USART_Cmd(USART1, ENABLE);                    //使能串口1 
}

void uart3_init(u32 bound)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//GPIO端口设置
	Uart3_Gpio_Init();	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART1时钟
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART3, &USART_InitStructure); //初始化串口1
	
	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//开启串口接受中断
	USART_Cmd(USART3, ENABLE);                    //使能串口1 
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	BaseType_t xHigherPriorityTaskWoken;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		Uart1_Rx_Buf[Uart1_Rx_Cnt] = USART_ReceiveData(USART1);	//读取接收到的数据
		Uart1_Rx_Cnt++;	
     }
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		USART1->SR;
		USART1->DR;
		Uart1_Rx_Cnt = 0;
//		Uart1_Rx_Flag = 1;	
		
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
     } 

} 

void USART3_IRQHandler(void)                	//串口1中断服务程序
{
	BaseType_t xHigherPriorityTaskWoken;

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		Uart3_Rx_Buf[Uart3_Rx_Cnt] = USART_ReceiveData(USART3);	//读取接收到的数据
		Uart3_Rx_Cnt++;	
     }
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		USART3->SR;
		USART3->DR;
		Uart3_Rx_Cnt = 0;
//		Uart3_Rx_Flag = 1;
		
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
     } 

} 

void UART1_SendData(uint8_t *data_buf,uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
		USART_SendData(USART1,data_buf[i]);
	}
}

void UART3_SendData(uint8_t *data_buf,uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);
		USART_SendData(USART3,data_buf[i]);
	}
}

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

