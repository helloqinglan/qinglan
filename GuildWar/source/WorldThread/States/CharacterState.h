
// *-----------------------------------------------------------------
//
// ��ɫ����״̬
//
// *-----------------------------------------------------------------

#pragma once

#include "NetWork/SocketState.h"
#include "WorldThread/WorldPacket.h"

class CharacterState : public SocketState
{
public:
	CharacterState(SocketService* sm);

	// ��������
	bool process(void* arg = 0);

private:
	// �������Կͻ��˵�ping��
	bool pingRequest(WorldPacket& packet);

	// ������ɫ
	bool createCharacter(WorldPacket& packet);

	// ö�ٽ�ɫ
	bool enumCharacter(WorldPacket& packet);

	// ɾ����ɫ
	bool deleteCharacter(WorldPacket& packet);

	// ������Ϸ
	bool playerLogin(WorldPacket& packet);

	// ***TODO*** δ֪����
	bool realmSplitInfo(WorldPacket& packet);

	bool sendData(WorldPacket& packet);

private:
};
