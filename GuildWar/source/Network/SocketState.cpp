
// *-------------------------------------------------------
//
// socket������״̬����
//
// *-------------------------------------------------------

#include "GuildWarPch.h"
#include "Network/SocketService.h"
#include "Network/SocketState.h"

SocketState::SocketState(SocketService* sm)
: m_stateMachine(sm)
{
	// ***TODO*** ��������״̬����˽�б����������, �������ڹ����������
	m_dataBuffer = m_stateMachine->dataBuffer();
}

// ��ͻ��˷�����
bool SocketState::sendData(const char* data, size_t len)
{
	return m_stateMachine->sendData(data, len);
}

// �任״̬
bool SocketState::transition(SocketState* state)
{
	return m_stateMachine->transition(state);
}
