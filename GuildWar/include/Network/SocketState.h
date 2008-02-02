
// *-----------------------------------------------------------------
//
// socket处理器状态基类
//
// *-----------------------------------------------------------------

#pragma once

#include "GuildWarFwd.h"

class SocketState
{
public:
	SocketState(SocketService* sm);
	virtual ~SocketState();

	// 状态开始时的回调方法
	virtual void enter() {}

	// 状态结束时的回调方法
	virtual void leave() {}

	// 处理接口 各状态自行定义该接口的意义
	virtual bool process(void* arg = 0) { return true; }

	// 向客户端发数据
	bool sendData(const char* data, size_t len);

	// 变换状态
	bool transition(SocketState* state);

	CircularBuffer* dataBuffer() const;

protected:
	SocketService* m_socketService;	// 所属的服务处理器
};
