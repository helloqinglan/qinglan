
// *-------------------------------------------------------
//
// socket������״̬����
//
// *-------------------------------------------------------

#include "GuildWarPch.h"
#include "Network/SocketService.h"
#include "Network/SocketState.h"

SocketState::SocketState(SocketService* sm)
: m_socketService(sm)
{
}

SocketState::~SocketState()
{
}

// ��ͻ��˷�����
bool SocketState::sendData(const char* data, size_t len)
{
	return m_socketService->sendData(data, len);
}

// �任״̬
bool SocketState::transition(SocketState* state)
{
	return m_socketService->transition(state);
}

// ***TODO*** �÷������������ڲ���˽�б���, �����Ϸ�װ��ԭ��
CircularBuffer* SocketState::dataBuffer() const
{
	return m_socketService->dataBuffer();
}
