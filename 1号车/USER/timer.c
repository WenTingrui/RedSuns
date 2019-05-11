#include "timer.h"
#include "cesu_tim4_capture.h"	 
#include "dianji_tim3_pwm.h"
#include "csb_ceju_uart.h"

#define INTERVAL 2000 //编码器转一圈花时500*0.002ms*100=100ms

extern float KP;
extern float KI;
extern float KD;

extern  u32 interval;//编码器产生一个脉冲内所计个数，计一个数为0.02ms
extern u32 weiyi;
extern u32 heng;
extern int sum_et;
extern int eet[];
int et_len=20;
u8 ii;
int et=10,et_1=10,et_2=10;
float umotor=0;//电机输出量大小取值0~2000

//定时器1中断服务程序

void TIM1_UP_IRQHandler(void)   //TIM1中断
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );  //清除TIMx更新中断标志 
	  
		//上一时刻与上上时刻的偏差et值
		et_2=et_1;
		et_1=et;
		
		sum_et=sum_et-eet[0]+interval-INTERVAL;
		et=sum_et/et_len;
		for(ii=0;ii<(et_len-1);ii++)
		{
			eet[ii]=eet[ii+1];
		}
		eet[et_len-1]=interval-INTERVAL;
		
		//增量式PID控制
		umotor=umotor+((KP+KI+KD)*et-(KP+2*KD)*et_1+KD*et_2)/1000000.0;
		
		if(umotor>1000.0)umotor=1000;
		else if(umotor<0)umotor=0;
		u_motor((u32)umotor);
			
			//每多发一个都要降低TIM1中断周期
//				fasong(umotor );
	     	fasong(weiyi );
				USART_SendData(USART1,32);//发空格
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			
//				fasong((u32)et+2000000000 );
		    fasong(interval);
				USART_SendData(USART1,59);//发分号
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		}
}

//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM1_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能
	
	//定时器TIM1初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //使能指定的TIM1中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM1, ENABLE);  //使能TIMx					 
}












