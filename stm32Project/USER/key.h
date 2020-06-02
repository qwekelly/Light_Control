#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
 
#define KEY0  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)//读取按键1
#define KEY1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//读取按键2
#define KEY2  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)//读取按键3
#define KEY3  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)//读取按键4
#define KEY4  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键5


#define KEY_UP 		4
#define KEY_LEFT	3
#define KEY_DOWN	2
#define KEY_RIGHT	1

void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8);  	//按键扫描函数					    
#endif
