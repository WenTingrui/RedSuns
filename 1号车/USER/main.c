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

int main(void)
 {	
	u8 i;
	 u16 uin4,uin5;
	 float uin1,uin2,uin3,y,left,
		 W1[5][3]={0.478289401668822,	0.699036628214231,	-2.01353668108713,
-2.63529565936215,	-1.32552713172024,	2.18329808018675,
-4.32197239504676,	-0.249722046050457,	3.18080726146058,
-5.49648545345733,	-3.63018882285688,	-0.938280153415333,
-2.00479515332131,	2.09248753414870,	1.50499143584965},
		 W2[5]={-1.93445302777796,	0.750889261427496	,-0.775572964010817	,-3.71349566096047,	-2.78268415585047},
		 W3[3]={0.522649564157108	,0.196349468986277,	0.273749136664140},
		 B1[5]={-1.99157952337052,
2.86891966581915,
0.151501075295175,
0.740350580790896,
-0.597730055196391},
		 B2=-5.34754995516199,
		 umid[5];
	 
	delay_init();	    	 //延时函数初始化	  
	 Adc_Init();
	uart_csb_init();
 	TIM3_PWM_Init(199,72-1);//电机	 
	 TIM2_PWM_Init(19999,72-1);//舵机	
	  TIM4_Cap_Init(500,720/5-1);	//以500khz即0.002ms的频率计数 ，一次计500下
	TIM1_Int_Init(9,719);//100Khz的计数频率，计数到10为0.1ms  
		
   	while(1)
	{			
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
		else if(uin1<0.1)left=0;	
		//进入十字路口直走
	  if(uin4>3103&&uin5>3103)left=500;
		if(left<0)turnleft(0);
		else if(left>1000)turnleft (1000);
		else turnleft((u16)left);
		
		
	}	 
 }




