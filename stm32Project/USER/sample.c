/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	sample.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-06-14
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		���ļ���������룬����ֻ��SDKʹ�õ���ʾ
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/


#include "edpkit.h"


unsigned char dataMem[64];		//ȫ�����鷽ʽ


//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	devid�������豸��devid���ƷID
//				auth_key�������豸��masterKey��apiKey���豸��Ȩ��Ϣ
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_DevLink(const char* devid, const char* auth_key)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};
	
	edpPacket._data = dataMem;				//ȫ�����鷽ʽ
	edpPacket._len = sizeof(dataMem);		//ȫ�����鷽ʽ

//---------------------------------------------����һ�����---------------------------------------------
	if(EDP_PacketConnect1(devid, auth_key, 256, &edpPacket) == 0)
	{
//---------------------------------------------�������������������--------------------------------------
		NET_DEVICE_SendData(edpPacket._data, edpPacket._len);
		
//---------------------------------------------���������жϷ�������--------------------------------------
		if(EDP_UnPacketRecv(dataPtr) == CONNRESP)
		{
//---------------------------------------------�����ģ��������ؽ��--------------------------------------
			switch(EDP_UnPacketConnectRsp(dataPtr))
			{
				case 0:
					UsartPrintf(USART_DEBUG, "Tips:	���ӳɹ�\r\n");
					oneNetInfo.netWork = 1;
				break;
				
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
		
//---------------------------------------------�����壺ɾ��---------------------------------------------
		EDP_DeleteBuffer(&edpPacket);
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	EDP_PacketConnect Failed\r\n");
	
}

//==========================================================
//	�������ƣ�	OneNet_PushData
//
//	�������ܣ�	PUSHDATA
//
//	��ڲ�����	dst_devid�������豸��devid
//				data����������
//				data_len�����ݳ���
//
//	���ز�����	��
//
//	˵����		�豸���豸֮���ͨ��
//==========================================================
_Bool OneNet_PushData(const char* dst_devid, const char* data, unsigned int data_len)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};

//---------------------------------------------����һ�����---------------------------------------------
	if(EDP_PacketPushData(dst_devid, data, data_len, &edpPacket) == 0)
	{
//---------------------------------------------���������������-----------------------------------------
		NET_DEVICE_SendData(edpPacket._data, edpPacket._len);
		
//---------------------------------------------��������ɾ��---------------------------------------------
		EDP_DeleteBuffer(&edpPacket);
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	OneNet_PushData Failed\r\n");
	
	return 0;

}

//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
_Bool OneNet_SendData(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};
	
	_Bool status = 0;
	short body_len = 0;
	
	if(type < 1 && type > 5)
		return 1;
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-EDP_TYPE%d\r\n", type);
	
	if(type != kTypeBin)
	{
//---------------------------------------------����һ����������������---------------------------------------------
		body_len = DSTREAM_GetDataStream_Body_Measure(type, streamArray, streamArrayCnt, 0);
		
		if(body_len)
		{
//---------------------------------------------���������дЭ��ͷ-------------------------------------------------
			if(EDP_PacketSaveData(devid, body_len, NULL, (SaveDataType)type, &edpPacket) == 0)
			{
//---------------------------------------------�����������-------------------------------------------------------
				body_len = DSTREAM_GetDataStream_Body(type, streamArray, streamArrayCnt, edpPacket._data, edpPacket._size, edpPacket._len);
				
				if(body_len)
				{
					edpPacket._len += body_len;
//---------------------------------------------�����ģ���������---------------------------------------------------
					NET_DEVICE_SendData(edpPacket._data, edpPacket._len);
					UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", edpPacket._len);
				}
				else
					UsartPrintf(USART_DEBUG, "WARN:	DSTREAM_GetDataStream_Body Failed\r\n");
				
//---------------------------------------------�����壺ɾ��-------------------------------------------------------
				EDP_DeleteBuffer(&edpPacket);
			}
			else
				UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
		}
		else
			status = 1;
	}
	else
	{
		OneNet_SendData_Picture(devid, Array, sizeof(Array));
	}
	
	return status;
	
}

//==========================================================
//	�������ƣ�	OneNet_SendData_EDPType2
//
//	�������ܣ�	�ϴ����������ݵ�ƽ̨
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		���ǵ����豸����������ʱ������ʹ�������豸��͸��ģʽ
//				�ϴ�ͼƬ�ǣ�ǿ�ҽ���devid�ֶ�Ϊ�գ�����ƽ̨�ὫͼƬ�����·����豸
//==========================================================
#define PKT_SIZE 1024
void OneNet_SendData_Picture(char *devid, const char* picture, unsigned int pic_len)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};

	char type_bin_head[] = "{\"ds_id\":\"pic\"}";
	unsigned char *pImage = (unsigned char *)picture;
	
//---------------------------------------------����һ����װЭ��ͷ---------------------------------------------
	if(EDP_PacketSaveData(devid, pic_len, type_bin_head, kTypeBin, &edpPacket) == 0)
	{
		UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", edpPacket._len);
//---------------------------------------------�����������Э��ͷ---------------------------------------------
		NET_DEVICE_SendData(edpPacket._data, edpPacket._len);
		
//---------------------------------------------��������ɾ��--------------------------------------------------
		EDP_DeleteBuffer(&edpPacket);
		
		UsartPrintf(USART_DEBUG, "image len = %d\r\n", pic_len);
		
//---------------------------------------------�����ģ�����bin�ļ�����----------------------------------------
		while(pic_len > 0)
		{
			RTOS_TimeDly(20);
			
			if(pic_len >= PKT_SIZE)
			{
				NET_DEVICE_SendData(pImage, PKT_SIZE);
				
				pImage += PKT_SIZE;
				pic_len -= PKT_SIZE;
			}
			else
			{
				NET_DEVICE_SendData(pImage, (unsigned short)pic_len);
				pic_len = 0;
			}
		}
		
		UsartPrintf(USART_DEBUG, "image send ok\r\n");
	}
	else
		UsartPrintf(USART_DEBUG, "EDP_PacketSaveData Failed\r\n");

}

//==========================================================
//	�������ƣ�	OneNet_HeartBeat
//
//	�������ܣ�	�������
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_HeartBeat(void)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};
	unsigned char dataMem1[8];				//�ֲ����鷽ʽ

	unsigned char sCount = 3;
	
	edpPacket._data = dataMem;				//�ֲ����鷽ʽ
	edpPacket._len = sizeof(dataMem);		//�ֲ����鷽ʽ
	
//---------------------------------------------����һ�����---------------------------------------------
	if(EDP_PacketPing(&edpPacket))
		return;
	
	while(sCount--)
	{
//---------------------------------------------���������������-----------------------------------------
		NET_DEVICE_SendData(edpPacket._data, edpPacket._len);
		
//---------------------------------------------��������������������-------------------------------------
		if(EDP_UnPacketRecv(dataPtr) == PINGRESP)
		{
			UsartPrintf(USART_DEBUG, "Tips:	HeartBeat OK\r\n");
			
			break;
		}
		else
		{
			UsartPrintf(USART_DEBUG, "Check Device\r\n");
		}
		
		RTOS_TimeDly(2);
	}
	
//---------------------------------------------�����ģ�ɾ��---------------------------------------------
	EDP_DeleteBuffer(&edpPacket);

}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};
	
	char *cmdid_devid;
	unsigned short cmdid_len = 0;
	char *req;
	unsigned int req_len = 0;
	
	short result = 0;
	
//---------------------------------------------����һ����ȡ������������---------------------------------------------
	switch(EDP_UnPacketRecv(cmd))						//�ж���pushdata���������·�
	{
//---------------------------------------------����������ú�������-------------------------------------------------
		case PUSHDATA:									//��pushdata��
			
			result = EDP_UnPacketPushData(cmd, &cmdid_devid, &req, &req_len);
		
			if(result == 0)
				UsartPrintf(USART_DEBUG, "src_devid: %s, req: %s, req_len: %d\r\n", cmdid_devid, req, req_len);
			
		break;
		
		case CMDREQ:									//�������
			
			result = EDP_UnPacketCmd(cmd, &cmdid_devid, &cmdid_len, &req, &req_len);
			
			if(result == 0)								//����ɹ������������ظ������
			{
				EDP_PacketCmdResp(cmdid_devid, cmdid_len, req, req_len, &edpPacket);
				UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_devid, req, req_len);
			}
			
		break;
			
		case SAVEACK:
			
			if(cmd[3] == MSG_ID_HIGH && cmd[4] == MSG_ID_LOW)
			{
				UsartPrintf(USART_DEBUG, "Tips:	Send %s\r\n", cmd[5] ? "Err" : "Ok");
			}
			else
				UsartPrintf(USART_DEBUG, "Tips:	Message ID Err\r\n");
			
		break;
	}
	
//---------------------------------------------�������������---------------------------------------------
	if(req)
	{
		
	}
	
//---------------------------------------------�����ģ��ͷ��ڴ�---------------------------------------------
	if(cmd[0] == PUSHDATA && result == 0)				//�����pushdata �� ����ɹ�
	{
		EDP_FreeBuffer(cmdid_devid);					//�ͷ��ڴ�
		EDP_FreeBuffer(req);
	}
//---------------------------------------------�����壺�ظ�����ͷ��ڴ�------------------------------------
	else if(cmd[0] == CMDREQ && result == 0)			//���������� �� ����ɹ�
	{
		EDP_FreeBuffer(cmdid_devid);					//�ͷ��ڴ�
		EDP_FreeBuffer(req);
														//�ظ�����
		NET_DEVICE_SendData(edpPacket._data, edpPacket._len);//�ϴ�ƽ̨
		EDP_DeleteBuffer(&edpPacket);					//ɾ��
	}

}
