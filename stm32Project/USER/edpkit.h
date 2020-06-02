#ifndef _EDPKIT_H_
#define _EDPKIT_H_


#include "Common.h"
#include "malloc.h"	  

//=============================����==============================
//===========�����ṩRTOS���ڴ��������Ҳ����ʹ��C���=========
#include <stdlib.h>

#define EDP_MallocBuffer	malloc

#define EDP_FreeBuffer		free
//==========================================================


#define MOSQ_MSB(A)         (uint8)((A & 0xFF00) >> 8)
#define MOSQ_LSB(A)         (uint8)(A & 0x00FF)


/*--------------------------------��Ϣ���--------------------------------*/
#define MSG_ID_HIGH			0x55
#define MSG_ID_LOW			0xAA


/*--------------------------------��Ϣ����--------------------------------*/
/* �������� */
#define CONNREQ             0x10
/* ������Ӧ */
#define CONNRESP            0x20
/* ת��(͸��)���� */
#define PUSHDATA            0x30
/* ���ӹر� */
#define DISCONNECT			0x40
/* �洢(ת��)���� */
#define SAVEDATA            0x80
/* �洢ȷ�� */
#define SAVEACK             0x90
/* �������� */
#define CMDREQ              0xA0
/* ������Ӧ */
#define CMDRESP             0xB0
/* �������� */
#define PINGREQ             0xC0
/* ������Ӧ */
#define PINGRESP            0xD0
/* �������� */
#define ENCRYPTREQ          0xE0
/* ������Ӧ */
#define ENCRYPTRESP         0xF0


#ifndef NULL
#define NULL (void*)0
#endif


/*--------------------------------SAVEDATA��Ϣ֧�ֵĸ�ʽ����--------------------------------*/
typedef enum
{
	
    kTypeFullJson = 0x01,
	
    kTypeBin = 0x02,
	
    kTypeSimpleJsonWithoutTime = 0x03,
	
    kTypeSimpleJsonWithTime = 0x04,
	
    kTypeString = 0x05
	
} SaveDataType;


/*--------------------------------�ڴ���䷽����־--------------------------------*/
#define MEM_FLAG_NULL		0
#define MEM_FLAG_ALLOC		1
#define MEM_FLAG_STATIC		2


typedef struct Buffer
{
	
	uint8	*_data;		//Э������
	
	uint32	_len;		//д������ݳ���
	
	uint32	_size;		//�����ܴ�С
	
	uint8	_memFlag;	//�ڴ�ʹ�õķ�����0-δ����	1-ʹ�õĶ�̬����		2-ʹ�õĹ̶��ڴ�
	
} EDP_PACKET_STRUCTURE;


/*--------------------------------ɾ��--------------------------------*/
void EDP_DeleteBuffer(EDP_PACKET_STRUCTURE *edpPacket);

/*--------------------------------���--------------------------------*/
uint8 EDP_UnPacketRecv(uint8 *dataPtr);

/*--------------------------------��¼��ʽ1���--------------------------------*/
uint1 EDP_PacketConnect1(const int8 *devid, const int8 *apikey, uint16 cTime, EDP_PACKET_STRUCTURE *edpPacket);

/*--------------------------------��¼��ʽ2���--------------------------------*/
uint1 EDP_PacketConnect2(const int8 *devid, const int8 *auth_key, uint16 cTime, EDP_PACKET_STRUCTURE *edpPacket);

/*--------------------------------���ӻظ����--------------------------------*/
uint8 EDP_UnPacketConnectRsp(uint8 *rev_data);

/*--------------------------------���ݵ��ϴ����--------------------------------*/
uint8 EDP_PacketSaveData(const int8 *devid, int32 send_len, int8 *type_bin_head, SaveDataType type, EDP_PACKET_STRUCTURE *edpPacket);

/*--------------------------------PushData���--------------------------------*/
uint8 EDP_PacketPushData(const int8 *devid, const int8 *msg, uint32 msg_len, EDP_PACKET_STRUCTURE *edpPacket);

/*--------------------------------PushData���--------------------------------*/
uint8 EDP_UnPacketPushData(uint8 *rev_data, int8 **src_devid, int8 **req, uint32 *req_len);

/*--------------------------------�����·����--------------------------------*/
uint8 EDP_UnPacketCmd(uint8 *rev_data, int8 **cmdid, uint16 *cmdid_len, int8 **req, uint32 *req_len);

/*--------------------------------����ظ����--------------------------------*/
uint1 EDP_PacketCmdResp(const int8 *cmdid, uint16 cmdid_len, const int8 *resp, uint32 resp_len, EDP_PACKET_STRUCTURE *edpPacket);

/*--------------------------------�����������--------------------------------*/
uint1 EDP_PacketPing(EDP_PACKET_STRUCTURE *edpPacket);


#endif
