
// *-------------------------------------------------------
//
// 服务器列表状态
//
// *-------------------------------------------------------

#pragma once

#include "Network/SocketState.h"

class RealmListState : public SocketState
{
public:
	RealmListState(SocketService* sm);
	~RealmListState();

	// 处理数据
	bool process(void* arg = 0);

private:
	bool requestRealmList();
};
