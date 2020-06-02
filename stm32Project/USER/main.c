#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "misc.h"
#include "adc.h"
#include "onenet.h"
#include "esp8266.h"
#include "edpkit.h"
#include "stm32f10x_adc.h"
	
void Key_Control_Pwm () {
	u16 led0pwmval=0;
	int KEY_VALUE = 0;
	while(1) {
		KEY_VALUE = KEY_Scan(0);
		if (KEY_VALUE == 1) {
				LED1 = 0; // ����
				led0pwmval += 1000;
			} else if (KEY_VALUE == 2) {
				LED1 = 1; // �ư�
				led0pwmval -= 1000;
			} else if(KEY_VALUE == 3) { // �л��ع�������
				break;
			}
			TIM_SetCompare2(TIM3,led0pwmval);	
	}
}

void Send_Data_Onenet () {
	int Light_Value;
	int timeCount = 0;	//���ͼ������
	int KEY_VALUE = 0;
	
	while(OneNet_DevLink())	//����OneNET  
	delay_ms(500);

	while(1) {
		KEY_VALUE = KEY_Scan(0);
		Light_Value = Get_Adc_Average(ADC_Channel_1,10);  // ADC ����ǿ��ֵ

		if(++timeCount >= 20)									//���ͼ��2s
		{
			UsartPrintf(USART1,"��onenetƽ̨��������:\r\n");
			OneNet_SendData(Light_Value);									//��������

			timeCount = 0;
			ESP8266_Clear();
		}
		//delay_ms(10);
		
		if(KEY_VALUE == 4) { // �ر����ݴ���
			LED1 = 1; // LED��Ϩ��
			break;
		}
	}
}

void esp8266_rec(void)			/*�������е��������н���*/
{
	unsigned char *dataPtr = NULL;
	int num = 0;
	int KEY_VALUE = 0;
	u16 led0pwmval=0;
	
	while(1) {
		KEY_VALUE = KEY_Scan(0);
		dataPtr = ESP8266_GetIPD(0);
			
		if(dataPtr != NULL)
		{
			UsartPrintf(USART_DEBUG, "���յ�������s��%s\r\n", dataPtr);
			//data_pro(dataPtr);
			num = atoi((const char *)dataPtr);				//תΪ��ֵ��ʽ
			UsartPrintf(USART_DEBUG, "��������ת����num��: %d\r\n", num);
			if(num == 1)								//�����������Ϊ1������
			{
				LED1 = 0;
				led0pwmval += 1000;
			}
			else if(num == 0)							//�����������Ϊ0�������
			{
				LED1 = 1;
				led0pwmval -= 1000;
			}
		}
		TIM_SetCompare2(TIM3,led0pwmval);	
		
		if(KEY_VALUE == 5) { // �رս�������
			LED1 = 1; // LED��Ϩ��
			break;
		}
	}
}
	
 int main()
{	
 	u16 led0pwmval=0;	
	u16 adcx;
	int KEY_VALUE=0;
	
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	Usart1_Init(115200);							//����1����ӡ��Ϣ��
  Usart2_Init(115200);							//����2������ESP8266��
 	LED_Init();			     //LED�˿ڳ�ʼ��
	KEY_Init(); //��ʼ������
	Adc_Init();		  		//ADC��ʼ��
  TIM3_PWM_Init(7200-1,0);	 //����Ƶ��PWMƵ��=72000/900=8Khz
	
  while(1)
	{
		KEY_VALUE = KEY_Scan(0);
		adcx = Get_Adc_Average(ADC_Channel_1,10);  // ADC ֵ
		
		if (KEY_VALUE == 3) { // ��������PWM
			Key_Control_Pwm();
		} 
		
		if (KEY_VALUE == 4) { // �ϴ����ݵ�onenetƽ̨
			LED1 = 0; // LED����
			ESP8266_Init();	// ��ʼ��ESP8266����ģʽ
			Send_Data_Onenet(); // �������ݵ��ƶ�
		}
		
		if (KEY_VALUE == 5) { // ��׿APP����ESP8266
			LED1 = 0; // LED����
			ESP8266_StaTcpClient_UnvarnishTest(); // ��ʼ��ESP8266Ϊ����ģʽ
			esp8266_rec(); // ����APP������
		}
		
		// ��������PWM
		if (adcx > 3000) {
			UsartPrintf(USART_DEBUG,"######### ADC ֵ #####: %d\r\n", adcx);
			led0pwmval = 5000;
		} else if (adcx > 2000) {
			UsartPrintf(USART_DEBUG,"######### ADC ֵ #####: %d\r\n", adcx);
			led0pwmval = 3000;
		} else {
			led0pwmval = 1000;
		}
		TIM_SetCompare2(TIM3,led0pwmval);	
	}
}
