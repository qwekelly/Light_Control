#ifndef	_ESP8266_H
#define	_ESP8266_H
#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include <stdbool.h>

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define RX_BUF_MAX_LEN     1024                                     //�����ջ����ֽ���

extern struct  STRUCT_USARTx_Fram                                  //��������֡�Ĵ���ṹ��
{
	char  Data_RX_BUF [ RX_BUF_MAX_LEN ];
	
  union {
    __IO u16 InfAll;
    struct {
		  __IO u16 FramLength       :15;                               // 14:0 
		  __IO u16 FramFinishFlag   :1;                                // 15 
	  } InfBit;
  }; 
	
} strEsp8266_Fram_Record;


typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;



/*��������ֻҪ�滻����������*/
#define DEVID	"599555758"

#define APIKEY	"QoQOysR5HchzOHO01EWsw9rTqhk="
                                         /*wifi����       wifi����*/
#define ESP8266_WIFI_INFO		"AT+CWJAP=\"Wesley\",\"qwe123456\"\r\n"

/*��������ֻҪ�滻����������*/

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"183.230.40.39\",876\r\n"

/********************************** �û���Ҫ���õĲ���**********************************/
#define   macUser_ESP8266_BulitApSsid         "qweESP8266"      //Ҫ�������ȵ������
#define   macUser_ESP8266_BulitApEcn           OPEN               //Ҫ�������ȵ�ļ��ܷ�ʽ
#define   macUser_ESP8266_BulitApPwd           "123456"         //Ҫ�������ȵ����Կ

#define   macUser_ESP8266_BulitApWifi          "AT+CWSAP=\"qweESP8266\",\"123456\",1,0\r\n"  //������wifi���ơ����롢���ܷ�ʽ
#define   macUser_ESP8266_MultipleId					 "AT+CIPMUX=1\r\n"   //WF-ESP8266ģ������������

#define   macUser_ESP8266_TcpServer_IP         "AT+CIPAP=\"192.168.123.169\"\r\n"      //������������IP��ַ
#define   macUser_ESP8266_TcpServer_Port       "AT+CIPSERVER=1,8086\r\n"     //�����������Ķ˿�   

#define   macUser_ESP8266_TcpServer_OverTime   "AT+CIPSTO=5000\r\n"          //��������ʱʱ�䣨��λ���룩

#define REV_OK		0	//������ɱ�־
#define REV_WAIT	1	//����δ��ɱ�־


void ESP8266_Init(void);
void ESP8266_Clear(void);
void ESP8266_SendData(unsigned char *data, unsigned short len);
void ESP8266_StaTcpClient_UnvarnishTest( void );

unsigned char *ESP8266_GetIPD(unsigned short timeOut);


#endif

