#ifndef __DUOJI_TIM2_PWM_H
#define __DUOJI_TIM2_PWM_H
#include "sys.h"
 
void turnright(u16 grade);
void turnleft(u16 grade);
void TIM2_PWM_Init(u16 arr,u16 psc);
 
#endif 
