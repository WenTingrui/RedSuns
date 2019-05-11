#include "sys.h"
#include "csb_ceju_uart.h"	 
#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

 
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u16 USART_TX_BUF[10]; 
u8 approve=0;
u8 i;
u8 len=0;
u16 temp=0;
u16 jiange_mm=0;
  
void uart_csb_init(){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
   //USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
	
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = 9600;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}

void USART1_IRQHandler(void)                	//����1�жϷ������
	{
		u8 Res;
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
			{
				Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
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
	if(data==0){USART_SendData(USART1,'0');//�򴮿�1��������
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	}//�ȴ����ͽ���
	
	while(data>0)
	{
		t=data%10;
		data=data/10;
		USART_TX_BUF[i]=t+0x30;
		i++;
	}
	for(;i>0;i--)
	{
		USART_SendData(USART1,USART_TX_BUF[i-1]);//�򴮿�1��������
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
	
	
}
