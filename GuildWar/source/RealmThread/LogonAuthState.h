
// *-------------------------------------------------------
//
// 登录验证状态
//
// *-------------------------------------------------------

#pragma once

#include "Network/SocketState.h"

class LogonAuthState : public SocketState
{
public:
	LogonAuthState(SocketService* sm);
	~LogonAuthState();

	// 处理数据
	virtual bool process(void* arg = 0);

private:
	bool logonRequest();

	bool logonCheck();

private:
	// 用于登录认证
	BigNumber N, s, g, v;
	BigNumber b, B;
	BigNumber rs;
	BigNumber K;
};
