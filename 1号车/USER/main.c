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
#include "blueteeth_pid_uart.h"

#define exp 2.718281828
#define et_len 20

float KP=5000.0;
float KI=100.0;
float KD=0.0 ;

u32 heng;
int sum_et=0;
int eet[et_len]={0};

int main(void)
 {	
	 u32 k_temp=0;
	u8 i;
	 u16 uin4,uin5;
	 float uin1,uin2,uin3,y,left,
		 W1[5][3]={-0.0426658121597683,-1.11785057822365,	-1.32852006775889,
1.07761405387666,	1.01247585712380,	1.10819194072038,
-0.541168743330897,	-1.85610232250142,	0.280170839545695,
-0.681948820278535,	-1.39001343680797,	-1.13116662874335,
0.303416860711514,	-1.35275176317598,	0.945182328742368},
		 W2[5]={1.46954879611704,	0.244901152418197,	-1.08754222775234,	-0.128573610212021,	0.528727923573703},
		 W3[3]={0.0869682307364712,0.103657298269278,-0.703300632763677},
		 B1[5]={0.489317644833280,
-5.47786975936034,
-0.123198068811170,
2.99578697881509,
1.99396042515172},
		 B2=0.656864223744382,
		 umid[5];
	 
	delay_init();	    	 //延时函数初始化	  
	 Adc_Init();
	uart_csb_init();
 	TIM3_PWM_Init(1999,7-1);//电机	 
	 TIM2_PWM_Init(19999,72-1);//舵机	
	  TIM4_Cap_Init(50000,9-1);	//以500khz即0.002ms的频率计数 ，一次计500下
	TIM1_Int_Init(100-1,3840-1);//100Khz的计数频率，计数到10为0.1ms  
		blueteeth_pid_usart3_init();

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
		left=y*500/200.0+550;
		
		//采集横向偏差
		y=y*100+50000;
		heng=y;
		
		//以下防止急弯判断反了
		if(uin1<0.2&&uin3<0.2)
		{
			if(uin4>uin5)left=0;
			else left=1050;
		}
		else if(uin1<0.2&&uin3>0.2)left=0;
		else if(uin1>0.2&&uin3<0.2)left=1050;
		
//		left=600;
		//进入十字路口直走
	  if(uin4>3103&&uin5>3103)left=500;
		if(left<0)turnleft(0);
		else if(left>1050)turnleft (1050);
		else turnleft((u16)left);
		
		//蓝牙PID参数调试
		if(USART3_RX_STA&0x8000)
		{	
    USART_SendData(USART1,10);//发空格
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			k_temp =0;
			
			if(USART3_RX_BUF[0]=='p'&&USART3_RX_BUF[1]=='p'&&USART3_RX_BUF[(USART3_RX_STA&0x3fff)-1]=='n')
			{
				k_temp =0;
				for(i=2;i<(USART3_RX_STA&0x3fff)-1;i++)
				{
					k_temp =k_temp *10+USART3_RX_BUF[i]-'0';
				}
				KP=k_temp ;
//				fasong(KP);
			}
			else if(USART3_RX_BUF[0]=='i'&&USART3_RX_BUF[1]=='i'&&USART3_RX_BUF[(USART3_RX_STA&0x3fff)-1]=='n')
			{
				k_temp =0;
				for(i=2;i<(USART3_RX_STA&0x3fff)-1;i++)
				{
					k_temp =k_temp *10+USART3_RX_BUF[i]-'0';
				}
				KI=k_temp ;
//				fasong(KI);
			}
			else if(USART3_RX_BUF[0]=='d'&&USART3_RX_BUF[1]=='d'&&USART3_RX_BUF[(USART3_RX_STA&0x3fff)-1]=='n')
			{
				k_temp =0;
				for(i=2;i<(USART3_RX_STA&0x3fff)-1;i++)
				{
					k_temp =k_temp *10+USART3_RX_BUF[i]-'0';
				}
				KD=k_temp ;
//				fasong(KD);
			}
//			KP=100;
//			USART_SendData(USART1,10);//发空格
//			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
//			fasong(111111);
//			USART_SendData(USART1,10);//发空格
//			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			USART3_RX_STA=0;
			TIM_Cmd(TIM1, ENABLE);
		}
	}	 
 }




