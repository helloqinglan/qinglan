
// *-----------------------------------------------------------------
//
// socket������������
//   ���𽫽��յ�����Ϣ������Ϣ�������, ��֪ͨ��ǰ״̬����
//   �ṩһ�����Ӧ�Ŀͻ��˷���Ϣ�Ľӿ�
//
// *-----------------------------------------------------------------

#pragma once

// ***TODO*** �������ŵ�һ��������ͷ�ļ���
// ���ݽ��ջ�������С (10K)
#define RECEIVE_BUFFER_LEN 10240

#include "GuildWarFwd.h"

class SocketService
{
public:
	SocketService(ACE_SOCK_Stream* peer);
	virtual ~SocketService();

	// ���ص�ǰ״̬
	SocketState* currentState() const
	{
		assert(m_currentState && "SocketService::currentState current state is null.");
		return m_currentState;
	}

	// ת��״̬
	virtual bool transition(SocketState* state);

	// ���ӽ���
	virtual void accepted() {}

	// ���ӹر�
	virtual void closed() {}

	// ��������
	virtual bool recvData(char* data, size_t len);

	// ��������
	virtual bool sendData(const char* data, size_t len);

	// ***TODO*** �÷������������ڲ���˽�б���, �����Ϸ�װ��ԭ��
	CircularBuffer* dataBuffer() const
	{
		assert(m_dataBuffer && "SocketService::dataBuffer m_dataBuffer is null.");
		return m_dataBuffer;
	}

protected:
	ACE_SOCK_Stream* m_peerStream;
	CircularBuffer* m_dataBuffer;

	SocketState* m_currentState;
};
