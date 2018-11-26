#include "cesu_tim4_capture.h"
#include "sys.h"


//��ʱ��4ͨ��1���벶������

TIM_ICInitTypeDef  TIM4_ICInitStructure;

void TIM4_Cap_Init(u16 arr,u16 psc)
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ��TIM4ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //ʹ��GPIOBʱ��
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;  //PB6 ���֮ǰ����  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PB6 ����  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_6);						 //PB6 ����
	
	//��ʼ����ʱ��4 TIM4	 
	TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  
	//��ʼ��TIM4���벶�����
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  	TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising ;	//�����ز���
  	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  	TIM4_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
  	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	
	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
	
	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC1,ENABLE);//��������ж� ,����CC1IE�����ж�	
	
   	TIM_Cmd(TIM4,ENABLE ); 	//ʹ�ܶ�ʱ��4
   


}

u16 overflow=0;	//��ʱ���������	    				
u32 interval=0; 
u32 interval_buffer=0;
u8 buhuo=0;
//��ʱ��4�жϷ������	 
void TIM4_IRQHandler(void)
{ 
		if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//���������¼�
		{	    	
				if(overflow==100)//����Ѿ����50�Σ���0.5sδ��⵽����
				{
					interval=overflow*500;
					overflow=0;			//���
					TIM_SetCounter(TIM4,0); 
				}
				else
				{
					overflow++;
				}
		}
	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)//����1���������¼�
		{	
			if(buhuo==4)
			{
			interval=interval_buffer+overflow*500+TIM_GetCapture1(TIM4);
				overflow=0;			//���
				buhuo=0;
				interval_buffer =0;
				TIM_SetCounter(TIM4,0); 
			}
			else
			{
				buhuo++;
				interval_buffer+=overflow*500+TIM_GetCapture1(TIM4);
				overflow=0;			//���
	 			TIM_SetCounter(TIM4,0); 
			}
		}			     	    					   
 
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC1|TIM_IT_Update); //����жϱ�־λ
 
}
