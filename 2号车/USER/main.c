#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "math.h"

#include "diancixunji_adc.h"
#include "dianji_tim3_pwm.h"
#include "duoji_tim2_pwm.h"
#include "cesu_tim4_capture.h"
#include "csb_ceju_uart.h"
#include "timer.h"

#define exp 2.718281828
u16 jianju;

int main(void)
 {	
	u8 i;
	 u16 uin4,uin5;
	 float uin1,uin2,uin3,y,left,
		 W1[5][3]={4.26328953055848,	-1.41441931283312	,-1.54824744143772,
-8.25330005995552,	1.96087894693396,	5.36505045329395,
1.35272589699328,	-0.320810515609156	,0.666901606669460,
-1.16218373435530	,-0.457337165921314,	-4.30703727089836,
-3.18135331509660	,0.780123748421666,	2.51204611864060},
		 W2[5]={-1.99238748177820,	-5.02658793772510,	-1.38598407232673	,2.10435974942875	,3.59027245957617},
		 W3[3]={2.05942361404777	,-0.664657517432823,	0.333990021921118},
		 B1[5]={-1.03749804291510,
0.429571844276008,
-3.92635237739360,
2.30861454896395,
-1.01450383128033},
		 B2=-1.81246036208828,
		 umid[5];
	 
	delay_init();	    	 //延时函数初始化	  
	 Adc_Init();
	uart_csb_init();
 	TIM3_PWM_Init(199,72-1);//电机	 
	 TIM2_PWM_Init(19999,72-1);//舵机	
TIM1_Int_Init(9,719);//100Khz的计数频率，计数到10为0.1ms  
		

   	while(1)
	{			
		 jianju=get_jianju();
		uin1=(float)(Get_Adc_Average(ADC_Channel_0,3)/1000.0);
		uin2=(float)(Get_Adc_Average(ADC_Channel_2,3)/1000.0);
		uin3=(float)(Get_Adc_Average(ADC_Channel_3,3)/1000.0);
		uin4=Get_Adc_Average(ADC_Channel_8,3);
		uin5=Get_Adc_Average(ADC_Channel_9,3);
		
		for(i=0;i<5;i++)umid[i]=W1[i][0]*uin1+W1[i][1]*uin2+W1[i][2]*uin3+B1[i];
		for(i=0;i<5;i++)umid[i]=1-2*pow(exp,-umid[i])/(pow(exp,umid[i])+pow(exp,-umid[i]));
		y=W3[0]*uin1+W3[1]*uin2+W3[2]*uin3+W2[0]*umid[0]+W2[1]*umid[1]+W2[2]*umid[2]+W2[3]*umid[3]+W2[4]*umid[4]+B2;
		y=y*100.0; //mm级
		left=y*500/200.0+500;
		
		//以下防止急弯判断反了
		if(uin3<0.1)left=1000;
		if(uin1<0.1)left=0;	

			//left=500;
		//进入十字路口直走
	  if(uin4>3103&&uin5>3103)left=500;
		if(left<0)turnleft(0);
		else if(left>1000)turnleft (1000);
		else turnleft((u16)left);
		

	}	 
 }




