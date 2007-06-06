
// *-------------------------------------------------------
//
// socket������������
//   ���𽫽��յ�����Ϣ������Ϣ�������, ��֪ͨ��ǰ״̬����
//   �ṩһ�����Ӧ�Ŀͻ��˷���Ϣ�Ľӿ�
//
// *-------------------------------------------------------

#include "GuildWarPch.h"
#include "Network/SocketState.h"
#include "Network/SocketService.h"

SocketService::SocketService(ACE_SOCK_Stream* peer)
: m_peerStream(peer), m_currentState(0), m_dataBuffer(new CircularBuffer(RECEIVE_BUFFER_LEN))
{
}

SocketService::~SocketService()
{
	delete m_dataBuffer;
	m_peerStream->close();
}


// ת��״̬
bool SocketService::transition(SocketState* state)
{
	if (!state)
	{
		assert(state && "SocketService::transition state is null.");
		return false;
	}

	SocketState* oldState = m_currentState;
	m_currentState = state;

	if (oldState)
		oldState->leave();
	m_currentState->enter();

	return true;
}

// ��������
bool SocketService::recvData(char* data, size_t len)
{
	if (!m_dataBuffer->write(data, len))
		return false;

	return currentState()->process();
}

// ��������
bool SocketService::sendData(const char* data, size_t len)
{
	// ***TODO*** ������Ҫ�ٴ���һ��, ������ȫ��ȷ����
	m_peerStream->send(data, len);
	return true;
}
