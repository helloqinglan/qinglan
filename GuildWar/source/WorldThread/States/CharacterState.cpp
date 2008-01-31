
// *-----------------------------------------------------------------
//
// ��ɫ����״̬
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "CharacterState.h"
#include "Database/Database.h"
#include "WorldThread/WorldSocket.h"

#include "Entity/EntityDefines.h"
#include "Entity/EntityManager.h"


// *-----------------------------------------------------------------
//
// ��״̬�´���Ŀͻ�����Ϣ��
#define CMSG_PING						476		// Ping��
#define CMSG_CHAR_CREATE				54		// ������ɫ
#define CMSG_CHAR_ENUM					55		// ��ɫ�б�
#define CMSG_CHAR_DELETE				56		// ɾ����ɫ
#define CMSG_PLAYER_LOGIN				61		// ������Ϸ
#define CMSG_REALM_SPLIT_INFO_REQUEST	908		// ***TODO*** �ڽ�ɫѡ��������, ��֪����ʲô��

// ��״̬�·������ᷢ�͵���Ϣ��
#define SMSG_PONG						477		// Ping���ذ�
#define SMSG_CHAR_CREATE				58		// ������ɫ
#define SMSG_CHAR_ENUM					59		// ��ɫ�б�
#define SMSG_CHAR_DELETE				60		// ɾ����ɫ
#define SMSG_REALM_SPLIT_INFO_RESPONSE	907		// ��908��Ӧ��

// ��ɫ�����Ľ����
enum E_CHAR_CREATE_CODE
{
	CHAR_CREATE_SUCCESS	= 0x2E,	// �ɹ�
	CHAR_CREATE_ERROR	= 0x2F,	// ����
	CHAR_CREATE_FAILED	= 0x30,	// ʧ��
};

// ��ɫɾ���Ľ����
enum E_CHAR_DELETE_CODE
{
	CHAR_DELETE_SUCCESS	= 0x3A,	// �ɹ�
};
//
// *-----------------------------------------------------------------


CharacterState::CharacterState(SocketService* sm)
: SocketState(sm)
{
}

bool CharacterState::process(void* arg)
{
	// ***TODO*** ��ª��ǿ������ת��, �ĳ�ʹ��boost::any
	WorldPacket* packet = (WorldPacket*)arg;

	// ***TODO*** ��SocketState�������ṩ����Ϣע���¼�����ķ���
	//            ȥ����Щ�߳���if...else
	if (packet->getOpcode() == CMSG_PING)
		return pingRequest(*packet);
	else if (packet->getOpcode() == CMSG_CHAR_CREATE)
		return createCharacter(*packet);
	else if (packet->getOpcode() == CMSG_CHAR_ENUM)
		return enumCharacter(*packet);
	else if (packet->getOpcode() == CMSG_CHAR_DELETE)
		return deleteCharacter(*packet);
	else if (packet->getOpcode() == CMSG_PLAYER_LOGIN)
		return playerLogin(*packet);
	else if (packet->getOpcode() == CMSG_REALM_SPLIT_INFO_REQUEST)
		return realmSplitInfo(*packet);
	else
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("CharacterState::process δ֪����Ϣ��: %d.\n"), packet->getOpcode()));
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
	WorldPacket data(SMSG_CHAR_CREATE, 1);

	Entity* entity = ENTITYMANAGER->createEntity(packet);
	if (!entity)
	{
		data << (u_char)CHAR_CREATE_ERROR;
	}
	else
	{
		entity->save();
		data << (u_char)CHAR_CREATE_SUCCESS;
	}

	sendData(data);
	return true;
}

// ö�ٽ�ɫ
bool CharacterState::enumCharacter(WorldPacket& packet)
{
	WorldPacket data(SMSG_CHAR_ENUM, 100);

	u_char count = 0;
	data << count;

	// ��ѯ��ɫ�б�
	QueryResult* result = DATABASE->query("SELECT `id` FROM `characters`");
	if (result)
	{
		do
		{
			Field* field = result->fetch();
			u_int id = field[0].getUInt();

			Entity* entity = ENTITYMANAGER->createEntity(Entity::Type_Player);
			if (!entity->load(id))
				continue;

			entity->buildEnumPacket(data);

			delete entity;

			++count;
		}
		while(result->nextRow());

		delete result;
	}

	if (count)
		data.put<u_char>(0, count);

	return sendData(data);
}

// ɾ����ɫ
bool CharacterState::deleteCharacter(WorldPacket& packet)
{
	u_int64 guid;
	packet >> guid;

	// ***TODO*** ɾ����ý�ɫ��ص���������
	DATABASE->pexecute("DELETE FROM `characters` WHERE `id` = '%u'", GUID_LOPART(guid));

	WorldPacket pkt(SMSG_CHAR_DELETE, 1);
	pkt << (u_char)CHAR_DELETE_SUCCESS;
	sendData(pkt);

	return true;
}

// ������Ϸ
bool CharacterState::playerLogin(WorldPacket& packet)
{
/*	Entity* entity = ENTITYMANAGER->createEntity(Entity::Type_Player);
	if (!entity->load(id))
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("CharacterState::playerLogin �����ݿ���ؽ�ɫ����ʧ��: %d.\n"), packet->getOpcode()));
		return false;
	}
*/
	return true;
}

// ***TODO*** δ֪����
bool CharacterState::realmSplitInfo(WorldPacket& packet)
{
	u_int unk;
	packet >> unk;

	std::string split_data = "01/01/01";
	WorldPacket pkt(SMSG_REALM_SPLIT_INFO_RESPONSE, split_data.size() + 9);
	pkt << unk;
	pkt << (u_int)0;
	pkt << split_data;
	sendData(pkt);

	return true;
}

bool CharacterState::sendData(WorldPacket& packet)
{
	return dynamic_cast<WorldSocket*>(m_socketService)->sendData(packet);
}
