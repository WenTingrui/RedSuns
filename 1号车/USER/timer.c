#include "timer.h"
#include "cesu_tim4_capture.h"	 
#include "dianji_tim3_pwm.h"
#include "csb_ceju_uart.h"

#define INTERVAL 2000 //������תһȦ��ʱ500*0.002ms*100=100ms

extern float KP;
extern float KI;
extern float KD;

extern  u32 interval;//����������һ�����������Ƹ�������һ����Ϊ0.02ms
extern u32 weiyi;
extern u32 heng;
extern int sum_et;
extern int eet[];
int et_len=20;
u8 ii;
int et=10,et_1=10,et_2=10;
float umotor=0;//����������Сȡֵ0~2000

//��ʱ��1�жϷ������

void TIM1_UP_IRQHandler(void)   //TIM1�ж�
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
	  
		//��һʱ��������ʱ�̵�ƫ��etֵ
		et_2=et_1;
		et_1=et;
		
		sum_et=sum_et-eet[0]+interval-INTERVAL;
		et=sum_et/et_len;
		for(ii=0;ii<(et_len-1);ii++)
		{
			eet[ii]=eet[ii+1];
		}
		eet[et_len-1]=interval-INTERVAL;
		
		//����ʽPID����
		umotor=umotor+((KP+KI+KD)*et-(KP+2*KD)*et_1+KD*et_2)/1000000.0;
		
		if(umotor>1000.0)umotor=1000;
		else if(umotor<0)umotor=0;
		u_motor((u32)umotor);
			
			//ÿ�෢һ����Ҫ����TIM1�ж�����
//				fasong(umotor );
	     	fasong(weiyi );
				USART_SendData(USART1,32);//���ո�
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			
//				fasong((u32)et+2000000000 );
		    fasong(interval);
				USART_SendData(USART1,59);//���ֺ�
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIMx					 
}












