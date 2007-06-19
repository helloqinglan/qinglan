
// *-----------------------------------------------------------------
//
// �����������
//
// *-----------------------------------------------------------------

#pragma once

#include "Network/SocketService.h"
#include "WorldThread/WorldPacket.h"
#include "WorldThread/CryptStrategy.h"

class WorldSocket : public SocketService
{
public:
	WorldSocket(ACE_SOCK_Stream* peer);
	~WorldSocket();

	// ���ӽ���
	void accepted();

	// ���ӹر�
	void closed();

	// �������� (��Ҫ��д�����ӶԽ��ܵ�֧��)
	bool recvData(char* data, size_t len);

	// ��������� (������ֱ��ʹ��ԭ����sendData����)
	bool sendData(const WorldPacket& packet);

	// ���ü�����Կ
	void setCryptKey(u_char* data, size_t len);

private:
	bool processPacket();

private:
	CryptStrategy m_authCrypt;			// �ӽ��ܲ���

	u_short m_recvRemain;				// �������ݻ�ʣ��Ĳ�������ֽ���
	ByteBuffer m_tmpBuffer;				// ��ʱ���ջ�����
										// ������ʱ������յ�������, �����յ�һ�����������ٵݽ���m_dataBuffer
};
