#ifndef __BLUTEETH_PID_UART_H
#define __BLUTEETH_PID_UART_H
#include "stdio.h"	
#include "sys.h" 

extern u8 USART3_RX_BUF[500];
extern u16 USART3_RX_STA;
	  	

//如果想串口中断接收，请不要注释以下宏定义
void blueteeth_pid_usart3_init(void);
#endif

