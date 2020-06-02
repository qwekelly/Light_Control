//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "esp8266.h"

//协议文件
#include "onenet.h"
#include "edpkit.h"

//硬件驱动
#include "usart.h"
#include "delay.h"

//C库
#include <string.h>
#include <stdio.h>

extern unsigned char esp8266_buf[128];

//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};				//协议包
	unsigned char *dataPtr;
	unsigned char status = 1;
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
                        "DEVID: %s,     APIKEY: %s\r\n"
                        , DEVID, APIKEY);
	if(EDP_PacketConnect1(DEVID, APIKEY, 256, &edpPacket) == 0)		//根据devid 和 apikey封装协议包
	{
		ESP8266_SendData(edpPacket._data, edpPacket._len);			//上传平台
		dataPtr = ESP8266_GetIPD(250);								//等待平台响应
		if(dataPtr != NULL)
		{
			if(EDP_UnPacketRecv(dataPtr) == CONNRESP)
			{
				switch(EDP_UnPacketConnectRsp(dataPtr))
				{
					case 0:UsartPrintf(USART_DEBUG, "Tips:	连接成功\r\n");status = 0;break;
					case 1:UsartPrintf(USART_DEBUG, "WARN:	连接失败：协议错误\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	连接失败：设备ID鉴权失败\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	连接失败：服务器失败\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	连接失败：用户ID鉴权失败\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	连接失败：未授权\r\n");break;
					case 6:UsartPrintf(USART_DEBUG, "WARN:	连接失败：授权码无效\r\n");break;
					case 7:UsartPrintf(USART_DEBUG, "WARN:	连接失败：激活码未分配\r\n");break;
					case 8:UsartPrintf(USART_DEBUG, "WARN:	连接失败：该设备已被激活\r\n");break;
					case 9:UsartPrintf(USART_DEBUG, "WARN:	连接失败：重复发送连接请求包\r\n");break;
					
					default:UsartPrintf(USART_DEBUG, "ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		EDP_DeleteBuffer(&edpPacket);								//删包
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	EDP_PacketConnect Failed\r\n");
	return status;
}

unsigned char OneNet_FillBuf(char *buf, int lightVal)
{
	char text[16];
	float Led_U; // 电压值
	float Led_I; // 电流值
	float Led_P; // 功率
	Led_U = (float)lightVal*(3.3/4096);
	Led_I = Led_U*100;
	Led_P = Led_U*Led_I;
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{");
	
	memset(text, 0, sizeof(text));	/*清零text数组*/					
	sprintf(text, "\"Light_Value\":%d,", lightVal);			/*(1)*/
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));	/*清零text数组*/						
	sprintf(text, "\"Led_U\":%f,", Led_U);		/*(2)*/
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));	/*清零text数组*/						
	sprintf(text, "\"Led_I\":%f,", Led_I);		/*(3)*/
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));	/*清零text数组*/						
	sprintf(text, "\"Led_P\":%f", Led_P);		/*(4)*/
	strcat(buf, text);
	 
	/*
	注意：数据传输格式按照上面的格式来添加
				在(1)点中那个%d后面有标点符号，(2)%d后面没有标点符号这个要注意
				反正插入的数据最后面一个不能有标点符号，其他的都要，不然的话数据传输不了
				Light_Value和Led_U是数据流名字，后面的就是数据了
	*/
	strcat(buf, "}");
	return strlen(buf);

}

//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_SendData(int lightValue)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};												//协议包
	
	char buf[128];
	
	short body_len = 0, i = 0;
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-EDP\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf, lightValue);																	//获取当前需要发送的数据流的总长度
	
	if(body_len)
	{
		if(EDP_PacketSaveData(DEVID, body_len, NULL, kTypeSimpleJsonWithoutTime, &edpPacket) == 0)	//封包
		{
			for(; i < body_len; i++)
				edpPacket._data[edpPacket._len++] = buf[i];
			
			ESP8266_SendData(edpPacket._data, edpPacket._len);										//上传数据到平台
			UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", edpPacket._len);
			
			EDP_DeleteBuffer(&edpPacket);															//删包
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
	}	
}
