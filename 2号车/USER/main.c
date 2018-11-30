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
		 W1[5][3]={-0.0818872168649904,	-0.550949370024538,	-8.37467827259221,
1.16107713549230	,0.00190147472342644,	2.47627940777597,
-8.69799612817567	,-1.54574192569701	,-1.26229814243903,
0.374202919091057	,-1.13949926220354	,-1.27929401608251,
-1.13461640542088	,-1.50189704640609	,-0.0376568257861453},
		 W2[5]={5.52747933599124,	1.23961408295873	,-5.88944865350467	,0.723494993952555	,-0.879622864836254},
		 W3[3]={-0.304702304500126,	0.0417771102963694,	-0.873922978676511},
		 B1[5]={0.106167170685937,
-4.36733447690222,
-0.202121729980319,
2.58552345622319,
1.69116231132036},
		 B2=0.201457509505782,
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
		if(left>600&&uin3>uin1)left=1000-left;
		else if(left<400&&uin3<uin1)left=1000-left;

			//left=500;
		//进入十字路口直走
	  if(uin4>3103&&uin5>3103)left=500;
		if(left<0)turnleft(0);
		else if(left>1000)turnleft (1000);
		else turnleft((u16)left);
		

	}	 
 }




