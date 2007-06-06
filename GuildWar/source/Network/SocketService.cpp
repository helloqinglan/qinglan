
// *-------------------------------------------------------
//
// socket服务处理器基类
//   负责将接收到的消息放入消息缓冲队列, 并通知当前状态处理
//   提供一个向对应的客户端发消息的接口
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


// 转换状态
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

// 接收数据
bool SocketService::recvData(char* data, size_t len)
{
	if (!m_dataBuffer->write(data, len))
		return false;

	return currentState()->process();
}

// 发送数据
bool SocketService::sendData(const char* data, size_t len)
{
	// ***TODO*** 这里需要再处理一下, 做到完全正确发送
	m_peerStream->send(data, len);
	return true;
}
