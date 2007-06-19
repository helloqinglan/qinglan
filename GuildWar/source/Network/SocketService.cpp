
// *-----------------------------------------------------------------
//
// socket������������
//   ���𽫽��յ�����Ϣ������Ϣ�������, ��֪ͨ��ǰ״̬����
//   �ṩһ�����Ӧ�Ŀͻ��˷���Ϣ�Ľӿ�
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "Network/SocketState.h"
#include "Network/SocketService.h"

SocketService::SocketService(ACE_SOCK_Stream* peer)
: m_peerStream(peer), m_currentState(0), m_dataBuffer(new CircularBuffer(RECEIVE_BUFFER_LEN))
{
}

SocketService::~SocketService()
{
	transition(0);

	delete m_dataBuffer;
	m_peerStream->close();
}


// ת��״̬
bool SocketService::transition(SocketState* state)
{
	SocketState* oldState = m_currentState;
	m_currentState = state;

	// ��ִ����һ��״̬���˳���������ɾ��
	if (oldState)
	{
		oldState->leave();
		delete oldState;
	}

	if (m_currentState)
		m_currentState->enter();

	return true;
}

// ��������
bool SocketService::recvData(char* data, size_t len)
{
	if (!m_dataBuffer->write(data, len))
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("SocketService::recvData д�����ݵ����ջ�����ʧ��.\n")));
		return false;
	}

	return currentState()->process();
}

// ��������
bool SocketService::sendData(const char* data, size_t len)
{
	// ***TODO*** ������Ҫ�ٴ���һ��, ������ȫ��ȷ����
	m_peerStream->send(data, len);
	return true;
}
