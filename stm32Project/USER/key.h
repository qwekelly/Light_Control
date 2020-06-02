#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
 
#define KEY0  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)//��ȡ����1
#define KEY1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//��ȡ����2
#define KEY2  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)//��ȡ����3
#define KEY3  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)//��ȡ����4
#define KEY4  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//��ȡ����5


#define KEY_UP 		4
#define KEY_LEFT	3
#define KEY_DOWN	2
#define KEY_RIGHT	1

void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8);  	//����ɨ�躯��					    
#endif
