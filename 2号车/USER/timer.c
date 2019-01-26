#include "timer.h"
#include "csb_ceju_uart.h"
#include "dianji_tim3_pwm.h"
#include "csb_ceju_uart.h"

#define DISTANCE 500
#define KP 0.5
#define KI 0.00
#define KD -0.0

extern  u32 interval;//����������һ�����������Ƹ�������һ����Ϊ0.02ms
extern u16 jianju;
extern u32 weiyi;
int et=10,et_1=10,et_2=10;
float sudu=110;//�ٶȴ�Сȡֵ0~1000

//��ʱ��1�жϷ������
void TIM1_UP_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		et_2=et_1;
		et_1=et;
		et=jianju-DISTANCE ;
		if(jianju<DISTANCE +500)
		{
			sudu=KP*et+90;
			if(sudu>120)sudu=120;
			else if(sudu<95)sudu=95;
		}
		else sudu=120;
		umotor((u32)sudu);
			
			//ÿ�෢һ����Ҫ����TIM1�ж�����
//				fasong(weiyi );
//				USART_SendData(USART1,32);//���ո�
//				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
//			
//				fasong(interval );
//				USART_SendData(USART1,59);//���ֺ�
//				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}
}

//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM1_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM1��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM1�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIMx					 
}












