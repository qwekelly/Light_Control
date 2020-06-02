#ifndef _DELAY_H
#define _DELAY_H
#include "stm32f10x.h"
void delay_init(void);
void delay_us(u32 nus);
void delay_ms(u16 nms);
void my_delay_ms(int ms);
void TIM2_Config(void);

//typedef __IO uint32_t  vu32;
//uint32_t runtime = 0;//保存当前系统运行的时间 单位ms
//vu32 timeled[2] = {0,1000};//初始值  延时值
//vu32 timebeep[2] = {0,100};
//vu32 timekey[2] = {0,8};
//vu32 timeusart1[2] = {0,100};
//vu32 timeadc[2] = {0,1000};
//vu32 timeperson[2] = {0,100};


#endif