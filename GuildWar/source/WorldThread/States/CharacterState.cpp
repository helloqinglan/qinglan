
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
#define CMSG_PING				476		// Ping��
#define CMSG_CHAR_CREATE		54		// ������ɫ
#define CMSG_CHAR_ENUM			55		// ��ɫ�б�
#define CMSG_CHAR_DELETE		56		// ɾ����ɫ

// ��״̬�·������ᷢ�͵���Ϣ��
#define SMSG_PONG				477		// Ping���ذ�
#define SMSG_CHAR_CREATE		58		// ������ɫ
#define SMSG_CHAR_ENUM			59		// ��ɫ�б�
#define SMSG_CHAR_DELETE		60		// ɾ����ɫ

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
	QueryResult* result = DATABASE->query("SELECT id, name, pos_x, pos_y, pos_z, map, data FROM characters");
	if (result)
	{
		do
		{
			Field* field = result->fetch();
			assert(field);

			u_int id = field[0].getUInt();
			std::string name = field[1].getString();
			float pos_x = field[2].getFloat();
			float pos_y = field[3].getFloat();
			float pos_z = field[4].getFloat();
			int map = field[5].getInt();
			std::string datastr = field[6].getString();

			// ���͵�����

			data << MAKE_GUID(id, HIGHGUID_PLAYER);		// GUID
			data << name;								// ��ɫ��

			data << (u_char)0;							// ***TODO*** : ����
			data << (u_char)0;							// ***TODO*** : ְҵ
			data << (u_char)0;							// ***TODO*** : �Ա�

			data << (u_int)0;							// ***TODO*** : PLAYER_BYTES
			data << (u_char)0;							// ***TODO*** : PLAYER_BYTES_2

			data << (u_char)1;							// ***TODO*** : �ȼ�
			data << (u_int)0;							// ***TODO*** : ZoneId ����ID
			data << map;								// ***TODO*** : ��ͼID

			data << pos_x;								// ����
			data << pos_y;
			data << pos_z;

			data << (u_int)0;							// ***TODO*** : ����ID

			data << (u_char)0;							// ***TODO*** : δ֪ (different values on off, looks like flags)
			data << (u_char)0;							// ***TODO*** : ��ʾ��־ (��Player.cpp 1123��)
			data << (u_char)0xa0;						// ***TODO*** : δ֪
			data << (u_char)0;							// ***TODO*** : δ֪
			data << (u_char)1;							// ***TODO*** : δ֪

			data << (u_int)0;							// ***TODO*** : ������Ϣ
			data << (u_int)0;
			data << (u_int)0;

			// ***TODO*** : װ����Ϣ
			for (int i = 0; i < EQUIPMENT_SLOT_END; ++i)
			{
				data << (u_int)0;						// ***TODO*** : ��ʾID
				data << (u_char)0;						// ***TODO*** : InventoryType (ʲô��˼?)
			}

			data << (u_int)0;							// ***TODO*** : δ֪ (first bag display id)
			data << (u_char)0;							// ***TODO*** : δ֪ (first bag inventory type)

			++count;
		}
		while(result->nextRow());

		delete result;
	}

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

bool CharacterState::sendData(WorldPacket& packet)
{
	return dynamic_cast<WorldSocket*>(m_socketService)->sendData(packet);
}
