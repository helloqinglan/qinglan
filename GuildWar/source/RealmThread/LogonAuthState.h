
// *-------------------------------------------------------
//
// ��¼��֤״̬
//
// *-------------------------------------------------------

#pragma once

#include "Network/SocketState.h"

class LogonAuthState : public SocketState
{
public:
	LogonAuthState(SocketService* sm);
	~LogonAuthState();

	// ��������
	virtual bool process(void* arg = 0);

private:
	bool logonRequest();

	bool logonCheck();

private:
	// ���ڵ�¼��֤
	BigNumber N, s, g, v;
	BigNumber b, B;
	BigNumber rs;
	BigNumber K;
};
