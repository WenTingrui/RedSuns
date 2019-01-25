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
		 W1[5][3]={0.539722819427850	,-0.726717240440989,	-0.880190623614997,
0.884184830730101	,0.888344911907088	,1.06504616473000,
0.0996506847312701	,-1.87408218400279,	0.201617474333627,
-0.432928098933220,	-1.32772701115660,	-0.974050063180461,
0.331541774103095	,-1.30471002322355	,1.04430396218920},
		 W2[5]={0.487953864919063	,0.146187642514791	,-0.469561163488016	,0.247502965556720	,-0.126732938408628},
		 W3[3]={-0.0594099695562955	,0.0342957143358726	,-0.296352869172276},
		 B1[5]={0.711245406023192,
-6.03816128313698,
0.672422209392738,
3.81628080873561,
2.47034477623373},
		 B2=0.639504061233502,
		 umid[5];
	 
	delay_init();	    	 //延时函数初始化	  
	 Adc_Init();
	uart_csb_init();
 	TIM3_PWM_Init(199,72-1);//电机	 
	TIM2_PWM_Init(19999,72-1);//舵机	
	TIM4_Cap_Init(50000,9-1);	//以500khz即0.002ms的频率计数 ，一次计500下
	TIM1_Int_Init(100-1,3840-1);//100Khz的计数频率，计数到10为0.1ms  
		

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
		left=y*400/200.0+500;
		
		//以下防止急弯判断反了
		if(uin1<0.1)left=0;	if(uin3<0.1)left=1000;
		if(uin1<0.1&&uin3<0.1)
		{
			if(uin4>uin5)left=1000;
			else left=0;
		}

			//left=500;
		//进入十字路口直走
	  if(uin4>3103&&uin5>3103)left=500;
		if(left<0)turnleft(0);
		else if(left>1000)turnleft (1000);
		else turnleft((u16)left);
		

	}	 
 }




