
// *-----------------------------------------------------------------
//
// 会话状态, 验证session key的有效性, 登录处理
//
// *-----------------------------------------------------------------

#pragma once

#include "Network/SocketState.h"
#include "WorldThread/WorldPacket.h"

// 登录验证状态
class SessionState : public SocketState
{
public:
	SessionState(SocketService* sm);

	// 处理数据
	bool process(void* arg = 0);

private:
	// 处理来自客户端的ping包
	bool pingRequest(WorldPacket& packet);

	// 处理登录验证请求
	bool logonCheck(WorldPacket& packet);

	bool sendData(WorldPacket& packet);

private:
};
