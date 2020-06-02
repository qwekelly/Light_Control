//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸����
#include "esp8266.h"

//Ӳ������
#include "delay.h"
#include "usart.h"
#include "edpkit.h"

//C��
#include "led.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

unsigned char esp8266_buf[128];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;

struct  STRUCT_USARTx_Fram strEsp8266_Fram_Record = { 0 };


//==========================================================
//	�������ƣ�	ESP8266_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Clear(void)
{
	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;
}

//==========================================================
//	�������ƣ�	ESP8266_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool ESP8266_WaitRecive(void)
{
	if(esp8266_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
	if(esp8266_cnt == esp8266_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		esp8266_cnt = 0;							//��0���ռ���
		return REV_OK;								//���ؽ�����ɱ�־
	}
	esp8266_cntPre = esp8266_cnt;					//��Ϊ��ͬ
	return REV_WAIT;								//���ؽ���δ��ɱ�־
}

//==========================================================
//	�������ƣ�	ESP8266_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	unsigned char timeOut = 200;
	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//����յ�����
		{
				//Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
			if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
			{
				ESP8266_Clear();									//��ջ���
				return 0;
			}
		}
	  delay_ms(10);
	}
	return 1;
}

//==========================================================
//	�������ƣ�	ESP8266_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{
	char cmdBuf[32];
	ESP8266_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//�յ���>��ʱ���Է�������
	{
		Usart_SendString(USART2, data, len);		//�����豸������������
	}

}

//==========================================================
//	�������ƣ�	ESP8266_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��ESP8266�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{
	char *ptrIPD = NULL;
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//����������
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//������IPD��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
			}
		}
			delay_ms(5);													//��ʱ�ȴ�
	} while(timeOut--);
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��
}

//==========================================================
//	�������ƣ�	ESP8266_Init
//
//	�������ܣ�	��ʼ��ESP8266
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Init(void)
{
	UsartPrintf(USART_DEBUG, "1. AT\r\n");
	while(ESP8266_SendCmd("AT\r\n", "OK"))
			delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "2. AT+RST\r\n");
	while(ESP8266_SendCmd("AT+RST\r\n", "OK"))
			delay_ms(500);
	
	delay_ms(500);
	UsartPrintf(USART_DEBUG, "3. CWMODE\r\n");
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
			delay_ms(500);
	
	delay_ms(500);
	UsartPrintf(USART_DEBUG, "4. AT+CWDHCP\r\n");
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
			delay_ms(500);
	
	delay_ms(500);
	UsartPrintf(USART_DEBUG, "5. CWJAP\r\n");
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
			delay_ms(500);
	
	delay_ms(500);
	UsartPrintf(USART_DEBUG, "6. CIPSTART\r\n");
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
			delay_ms(500);
}

//==========================================================
//	�������ƣ�	USART2_IRQHandler
//
//	�������ܣ�	����2�շ��ж�
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //�����ж�
	{
		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //��ֹ���ڱ�ˢ��
		esp8266_buf[esp8266_cnt++] = USART2->DR;
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}
}







/**
  * @brief  ESP8266 ��Sta Tcp Client��͸��
  * @param  ��
  * @retval ��
  */
void ESP8266_StaTcpClient_UnvarnishTest ( void )
{
	UsartPrintf(USART_DEBUG, "\r\n����ת�� ESP8266 ����ģʽ......\r\n" );
	
	UsartPrintf(USART_DEBUG, "1. AT\r\n");
	while(ESP8266_SendCmd("AT\r\n", "OK"))
			delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "2. AT+RST\r\n");
	while(ESP8266_SendCmd("AT+RST\r\n", "OK"))
			delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "3. CWMODE\r\n");
	while(ESP8266_SendCmd("AT+CWMODE=2\r\n", "OK"))
			delay_ms(500);

	delay_ms(1000);
	UsartPrintf(USART_DEBUG, "4. AT+RST\r\n");
	while(ESP8266_SendCmd("AT+RST\r\n", "OK"))
			delay_ms(500);
	
	delay_ms(1000);
  UsartPrintf(USART_DEBUG, "4. AT+CWSAP\r\n");
	while(ESP8266_SendCmd("AT+CWSAP=\"qweESP8266\",\"123456\",1,0\r\n", "OK"))
			delay_ms(500);
	
	delay_ms(1000);
	UsartPrintf(USART_DEBUG, "5. AT+CIPMUX\r\n");
	while(ESP8266_SendCmd("AT+CIPMUX=1\r\n", "OK"))
			delay_ms(500);
	
	delay_ms(1000);
	UsartPrintf(USART_DEBUG, "6. AT+CIPSERVER\r\n");
	while(ESP8266_SendCmd("AT+CIPSERVER=1,8086\r\n", "OK"))
			delay_ms(500);
	
	delay_ms(1000);
	UsartPrintf(USART_DEBUG, "7. AT+CIPSTO\r\n");
	while(ESP8266_SendCmd("AT+CIPSTO=5000\r\n" , "OK"))
			delay_ms(500);
	
	delay_ms(1000);
	UsartPrintf(USART_DEBUG, "8. AT+CIFSR\r\n");
	while(ESP8266_SendCmd("AT+CIFSR\r\n", "OK"))
			delay_ms(500);
}


