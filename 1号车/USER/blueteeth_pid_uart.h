#ifndef __BLUTEETH_PID_UART_H
#define __BLUTEETH_PID_UART_H
#include "stdio.h"	
#include "sys.h" 

extern u8 USART3_RX_BUF[500];
extern u16 USART3_RX_STA;
	  	

//����봮���жϽ��գ��벻Ҫע�����º궨��
void blueteeth_pid_usart3_init(void);
#endif

