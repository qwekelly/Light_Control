//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸
#include "esp8266.h"

//Э���ļ�
#include "onenet.h"
#include "edpkit.h"

//Ӳ������
#include "usart.h"
#include "delay.h"

//C��
#include <string.h>
#include <stdio.h>

extern unsigned char esp8266_buf[128];

//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};				//Э���
	unsigned char *dataPtr;
	unsigned char status = 1;
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
                        "DEVID: %s,     APIKEY: %s\r\n"
                        , DEVID, APIKEY);
	if(EDP_PacketConnect1(DEVID, APIKEY, 256, &edpPacket) == 0)		//����devid �� apikey��װЭ���
	{
		ESP8266_SendData(edpPacket._data, edpPacket._len);			//�ϴ�ƽ̨
		dataPtr = ESP8266_GetIPD(250);								//�ȴ�ƽ̨��Ӧ
		if(dataPtr != NULL)
		{
			if(EDP_UnPacketRecv(dataPtr) == CONNRESP)
			{
				switch(EDP_UnPacketConnectRsp(dataPtr))
				{
					case 0:UsartPrintf(USART_DEBUG, "Tips:	���ӳɹ�\r\n");status = 0;break;
					case 1:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�Э�����\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��豸ID��Ȩʧ��\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�������ʧ��\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��û�ID��Ȩʧ��\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�δ��Ȩ\r\n");break;
					case 6:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ���Ȩ����Ч\r\n");break;
					case 7:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�������δ����\r\n");break;
					case 8:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ����豸�ѱ�����\r\n");break;
					case 9:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��ظ��������������\r\n");break;
					
					default:UsartPrintf(USART_DEBUG, "ERR:	����ʧ�ܣ�δ֪����\r\n");break;
				}
			}
		}
		EDP_DeleteBuffer(&edpPacket);								//ɾ��
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	EDP_PacketConnect Failed\r\n");
	return status;
}

unsigned char OneNet_FillBuf(char *buf, int lightVal)
{
	char text[16];
	float Led_U; // ��ѹֵ
	float Led_I; // ����ֵ
	float Led_P; // ����
	Led_U = (float)lightVal*(3.3/4096);
	Led_I = Led_U*100;
	Led_P = Led_U*Led_I;
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{");
	
	memset(text, 0, sizeof(text));	/*����text����*/					
	sprintf(text, "\"Light_Value\":%d,", lightVal);			/*(1)*/
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));	/*����text����*/						
	sprintf(text, "\"Led_U\":%f,", Led_U);		/*(2)*/
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));	/*����text����*/						
	sprintf(text, "\"Led_I\":%f,", Led_I);		/*(3)*/
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));	/*����text����*/						
	sprintf(text, "\"Led_P\":%f", Led_P);		/*(4)*/
	strcat(buf, text);
	 
	/*
	ע�⣺���ݴ����ʽ��������ĸ�ʽ�����
				��(1)�����Ǹ�%d�����б����ţ�(2)%d����û�б��������Ҫע��
				������������������һ�������б����ţ������Ķ�Ҫ����Ȼ�Ļ����ݴ��䲻��
				Light_Value��Led_U�����������֣�����ľ���������
	*/
	strcat(buf, "}");
	return strlen(buf);

}

//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_SendData(int lightValue)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};												//Э���
	
	char buf[128];
	
	short body_len = 0, i = 0;
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-EDP\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf, lightValue);																	//��ȡ��ǰ��Ҫ���͵����������ܳ���
	
	if(body_len)
	{
		if(EDP_PacketSaveData(DEVID, body_len, NULL, kTypeSimpleJsonWithoutTime, &edpPacket) == 0)	//���
		{
			for(; i < body_len; i++)
				edpPacket._data[edpPacket._len++] = buf[i];
			
			ESP8266_SendData(edpPacket._data, edpPacket._len);										//�ϴ����ݵ�ƽ̨
			UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", edpPacket._len);
			
			EDP_DeleteBuffer(&edpPacket);															//ɾ��
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
	}	
}
