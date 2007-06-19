
// *-----------------------------------------------------------------
//
// �Ự״̬, ��֤session key����Ч��, ��¼����
//
// *-----------------------------------------------------------------

#pragma once

#include "Network/SocketState.h"
#include "WorldThread/WorldPacket.h"

// ��¼��֤״̬
class SessionState : public SocketState
{
public:
	SessionState(SocketService* sm);

	// ��������
	bool process(void* arg = 0);

private:
	// �������Կͻ��˵�ping��
	bool pingRequest(WorldPacket& packet);

	// �����¼��֤����
	bool logonCheck(WorldPacket& packet);

	bool sendData(WorldPacket& packet);

private:
};
