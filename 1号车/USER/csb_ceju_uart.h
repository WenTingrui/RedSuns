#ifndef __CSB_CEJU_UART_H
#define __CSB_CEJU_UART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN  			10  	//定义最大接收字节数 10
	  	

    		
void uart_csb_init(void);
u16 get_jianju(void);
#endif


