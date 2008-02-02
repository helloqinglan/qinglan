
// *-----------------------------------------------------------------
//
// socket服务处理器基类
//   负责将接收到的消息放入消息缓冲队列, 并通知当前状态处理
//   提供一个向对应的客户端发消息的接口
//
// *-----------------------------------------------------------------

#pragma once

// 数据接收缓冲区大小 (10K)
#define RECEIVE_BUFFER_LEN 10240

#include "GuildWarFwd.h"

class SocketService
{
public:
	SocketService(ACE_SOCK_Stream* peer);
	virtual ~SocketService();

	// 返回当前状态
	SocketState* currentState() const
	{
		assert(m_currentState && "SocketService::currentState current state is null.");
		return m_currentState;
	}

	// 转换状态
	virtual bool transition(SocketState* state);

	// 连接建立
	virtual void accepted() {}

	// 连接关闭
	virtual void closed() {}

	// 接收数据
	virtual bool recvData(char* data, size_t len);

	// 发送数据
	virtual bool sendData(const char* data, size_t len);

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
