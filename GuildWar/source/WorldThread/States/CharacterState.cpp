
// *-----------------------------------------------------------------
//
// ��ɫ����״̬
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "CharacterState.h"
#include "WorldThread/WorldSocket.h"


// *-----------------------------------------------------------------
//
// ��״̬�´���Ŀͻ�����Ϣ��
#define CMSG_PING				476		// Ping��
#define CMSG_CHAR_CREATE		54		// ������ɫ
#define CMSG_CHAR_ENUM			55		// ��ɫ�б�
#define CMSG_CHAR_DELETE		56		// ɾ����ɫ

// ��״̬�·������ᷢ�͵���Ϣ��
#define SMSG_PONG				477		// Ping���ذ�
#define SMSG_CHAR_CREATE		58		// ������ɫ
#define SMSG_CHAR_ENUM			59		// ��ɫ�б�
#define SMSG_CHAR_DELETE		60		// ɾ����ɫ
//
// *-----------------------------------------------------------------


CharacterState::CharacterState(SocketService* sm)
: SocketState(sm)
{
}

bool CharacterState::process(void* arg)
{
	WorldPacket* packet = (WorldPacket*)arg;

	if (packet->getOpcode() == CMSG_PING)
		return pingRequest(*packet);
	else if (packet->getOpcode() == CMSG_CHAR_CREATE)
		return createCharacter(*packet);
	else if (packet->getOpcode() == CMSG_CHAR_ENUM)
		return enumCharacter(*packet);
	else if (packet->getOpcode() == CMSG_CHAR_DELETE)
		return deleteCharacter(*packet);
	else
	{
		assert(0 && "CharacterState::process unkown cmd.");
		return false;
	}

	return true;
}

// ÿ��״̬�¶���Ҫ�������Ϣ
// �ǲ������ָ�����ķ��������������Ϣ��?
bool CharacterState::pingRequest(WorldPacket& packet)
{
	u_int ping;
	packet >> ping;

	// ���������, ����ping֮��ļ��Ϊ30��
	// ���Դ����жϿͻ�����û���ü������

	WorldPacket pkt(SMSG_PONG, 4);
	pkt << ping;
	return sendData(pkt);
}

// ������ɫ
bool CharacterState::createCharacter(WorldPacket& packet)
{
	return true;
}

// ö�ٽ�ɫ
bool CharacterState::enumCharacter(WorldPacket& packet)
{
	WorldPacket data(SMSG_CHAR_ENUM, 100);

	u_char count = 0;
	data << count;
/*
	// ��ѯ��ɫ�б�
	ByteBuffer queryArg;
	queryArg << dynamic_cast<WorldClientService*>(m_socketService)->getAccountID();
	ByteBuffer queryRet;
	if (getDatabase()->query((u_short)DBWSID_GET_CHARACTERS, queryArg, queryRet))
	{
		queryRet >> count;
		assert(count == (queryRet.length() / sizeof(u_int)));

		for (u_char i = 0; i < count; ++i)
		{
			u_int charid;
			queryRet >> charid;

			Player* ply = new Player(dynamic_cast<WorldClientService*>(m_socketService), charid, true);
			delete ply;
		}
	}
*/
	data.put<u_char>(0, count);
	return sendData(data);
}

// ɾ����ɫ
bool CharacterState::deleteCharacter(WorldPacket& packet)
{
	return true;
}

bool CharacterState::sendData(WorldPacket& packet)
{
	return dynamic_cast<WorldSocket*>(m_socketService)->sendData(packet);
}
