
// *-------------------------------------------------------
//
// socket处理器状态基类
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

// 向客户端发数据
bool SocketState::sendData(const char* data, size_t len)
{
	return m_socketService->sendData(data, len);
}

// 变换状态
bool SocketState::transition(SocketState* state)
{
	return m_socketService->transition(state);
}

// ***TODO*** 该方法返回了类内部的私有变量, 不符合封装的原则
CircularBuffer* SocketState::dataBuffer() const
{
	return m_socketService->dataBuffer();
}
