
// *-------------------------------------------------------
//
// �������б�״̬
//
// *-------------------------------------------------------

#pragma once

#include "Network/SocketState.h"

class RealmListState : public SocketState
{
public:
	RealmListState(SocketService* sm);
	~RealmListState();

	// ��������
	bool process(void* arg = 0);

private:
	bool requestRealmList();
};
