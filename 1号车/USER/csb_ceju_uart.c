#include "sys.h"
#include "csb_ceju_uart.h"	 
#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

 
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u16 USART_TX_BUF[10]; 
u8 approve=0;
u8 i;
u8 len=0;
u16 temp=0;
u16 jiange_mm=0;
  
void uart_csb_init(){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
   //USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
	
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = 9600;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

}

void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
		u8 Res;
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
			{
				Res =USART_ReceiveData(USART1);	//读取接收到的数据
				if(Res==0x23)approve=1;
				if(approve==1&&Res!=0x23)
				{
					if(Res!=0x24)
					{
						USART_RX_BUF[len]=Res;
						len++;
					}
					else
					{
						approve=0;
						temp=0;
						for(i=0;i<len;i++)temp=temp*10+(USART_RX_BUF[i]-0x30);
						jiange_mm=temp;
						approve=0;
						temp=0;
						len=0;			
					}
				}
			} 

	} 
u16 get_jianju()
	{
		u16 j;
		u16 min=3440;
		for(j=0;j<1000;j++)
		{
			if(min>jiange_mm)min=jiange_mm ;
			delay_us (10);
		}
		return(min);
	}
void fasong(u32 data)
{
	u8 t,i=0;
	if(data==0){USART_SendData(USART1,'0');//向串口1发送数据
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	}//等待发送结束
	
	while(data>0)
	{
		t=data%10;
		data=data/10;
		USART_TX_BUF[i]=t+0x30;
		i++;
	}
	for(;i>0;i--)
	{
		USART_SendData(USART1,USART_TX_BUF[i-1]);//向串口1发送数据
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	}
	
	
}
