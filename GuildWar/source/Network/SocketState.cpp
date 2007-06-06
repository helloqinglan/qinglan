
// *-------------------------------------------------------
//
// socket处理器状态基类
//
// *-------------------------------------------------------

#include "GuildWarPch.h"
#include "Network/SocketService.h"
#include "Network/SocketState.h"

SocketState::SocketState(SocketService* sm)
: m_stateMachine(sm)
{
	// ***TODO*** 这样共享状态机的私有变量不大合适, 生命周期管理会有问题
	m_dataBuffer = m_stateMachine->dataBuffer();
}

// 向客户端发数据
bool SocketState::sendData(const char* data, size_t len)
{
	return m_stateMachine->sendData(data, len);
}

// 变换状态
bool SocketState::transition(SocketState* state)
{
	return m_stateMachine->transition(state);
}
