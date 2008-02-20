
// *-----------------------------------------------------------------
//
// ��ɫ����״̬
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "CharacterState.h"

#include "Component/DataIOComp.h"
#include "Component/UnitInterfComp.h"
#include "Component/PropertySetComp.h"
#include "Entity/EntityDefines.h"
#include "Entity/EntityManager.h"
#include "Entity/Entity.h"

#include "WorldThread/WorldSocket.h"
#include "Database/Database.h"

// ***TODO*** ��Щ��Ӧ�÷�������
#include "Entity/UpdateData.h"
#include "Entity/UpdateMask.h"


// *-----------------------------------------------------------------
//
// ��״̬�´���Ŀͻ�����Ϣ��
#define CMSG_PING						476		// Ping��
#define CMSG_CHAR_CREATE				54		// ������ɫ
#define CMSG_CHAR_ENUM					55		// ��ɫ�б�
#define CMSG_CHAR_DELETE				56		// ɾ����ɫ
#define CMSG_PLAYER_LOGIN				61		// ������Ϸ
#define CMSG_REALM_SPLIT_INFO_REQUEST	908		// ***TODO*** �ڽ�ɫѡ��������

// ��״̬�·������ᷢ�͵���Ϣ��
#define SMSG_PONG						477		// Ping���ذ�
#define SMSG_CHAR_CREATE				58		// ������ɫ
#define SMSG_CHAR_ENUM					59		// ��ɫ�б�
#define SMSG_CHAR_DELETE				60		// ɾ����ɫ
#define	SMSG_LOGIN_SETTIMESPEED			66		// ������ʱ��
#define SMSG_FRIEND_LIST				103		// �����б�
#define SMSG_TUTORIAL_FLAGS				253		// �̳��б�
#define SMSG_INITIALIZE_FACTIONS		290		// ��Ӫ��Ϣ
#define SMSG_SET_PROFICIENCY			295		// ***TODO***
#define SMSG_ACTION_BUTTONS				297		// ������б�
#define SMSG_INITIAL_SPELLS				298		// �����б�
#define SMSG_UPDATE_AURA_DURATION		311		// ***TODO***
#define SMSG_BINDPOINTUPDATE			341		// �ù�λ�ø���
#define SMSG_QUERY_TIME_RESPONSE		463		// ***TODO***
#define SMSG_ACCOUNT_DATA_MD5			521		// �ʺ�MD5
#define SMSG_SET_REST_START				542		// ***TODO*** ������Ϸʱ����
#define SMSG_LOGIN_VERIFY_WORLD			566		// У��������Ϣ
#define SMSG_INIT_WORLD_STATES			706		// ����״̬��Ϣ
#define SMSG_WEATHER					756		// ������Ϣ
#define SMSG_SET_DUNGEON_DIFFICULTY		809		// �����Ѷ�
#define SMSG_MOTD						829		// ��ӭ��Ϣ
#define SMSG_REALM_SPLIT_INFO_RESPONSE	907		// ��908��Ӧ��
#define SMSG_VOICE_SYSTEM_STATUS		968		// ����ϵͳ״̬����

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
	m_packetProcess[CMSG_PING] = &CharacterState::pingRequest;
	m_packetProcess[CMSG_CHAR_CREATE] = &CharacterState::createCharacter;
	m_packetProcess[CMSG_CHAR_ENUM] = &CharacterState::enumCharacter;
	m_packetProcess[CMSG_CHAR_DELETE] = &CharacterState::deleteCharacter;
	m_packetProcess[CMSG_PLAYER_LOGIN] = &CharacterState::playerLogin;
	m_packetProcess[CMSG_REALM_SPLIT_INFO_REQUEST] = &CharacterState::realmSplitInfo;
}

bool CharacterState::process(void* arg)
{
	WorldPacket* packet = (WorldPacket*)arg;

	PacketProcessList::iterator itr = m_packetProcess.find(packet->getOpcode());
	if (itr != m_packetProcess.end())
		return (this->*(itr->second))(*packet);
	else
		ACE_ERROR ((GAME_ERROR ACE_TEXT("CharacterState::process δ֪����Ϣ��: %d.\n"), packet->getOpcode()));

	return false;
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
		DataIOComp* dataIO = entity->getComponent<DataIOComp>(ComponentBase::DataIO);
		dataIO->save();
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

			Entity* entity = ENTITYMANAGER->createEntity();
			DataIOComp* dataIO = entity->getComponent<DataIOComp>(ComponentBase::DataIO);

			if (!dataIO->miniLoad(id))
				continue;

			dataIO->buildEnumPacket(data);

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
	u_int64 guid;
	packet >> guid;

	Entity* entity = ENTITYMANAGER->createEntity();
	DataIOComp* dataIO = entity->getComponent<DataIOComp>(ComponentBase::DataIO);
	UnitInterfComp* unitInterf = entity->getComponent<UnitInterfComp>(ComponentBase::UnitInterf);
	PropertySetComp* propSet = entity->getComponent<PropertySetComp>(ComponentBase::PropertySet);

	if (!dataIO->load(GUID_LOPART(guid)))
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("CharacterState::playerLogin �����ݿ���ؽ�ɫ����ʧ��: %u.\n"), GUID_LOPART(guid)));
		return false;
	}

	// ***TODO***
	{
		WorldPacket pkt(SMSG_UPDATE_AURA_DURATION, 5);
		pkt << (u_char)0;
		pkt << (u_int)-1;
		sendData(pkt);
	}

	// ***TODO***
	{
		WorldPacket pkt(SMSG_SET_PROFICIENCY, 8);
		pkt << (u_int)2;
		pkt << (u_int)1;
		sendData(pkt);
	}

	// ***TODO***
	{
		WorldPacket pkt(SMSG_SET_PROFICIENCY, 8);
		pkt << (u_int)2;
		pkt << (u_int)17;
		sendData(pkt);
	}

	// ***TODO***
	{
		WorldPacket pkt(SMSG_SET_PROFICIENCY, 8);
		pkt << (u_int)2;
		pkt << (u_int)145;
		sendData(pkt);
	}

	// ***TODO***
	{
		WorldPacket pkt(SMSG_SET_PROFICIENCY, 8);
		pkt << (u_int)2;
		pkt << (u_int)16529;
		sendData(pkt);
	}

	// ***TODO***
	{
		WorldPacket pkt(SMSG_SET_PROFICIENCY, 8);
		pkt << (u_int)4;
		pkt << (u_int)8;
		sendData(pkt);
	}

	// ***TODO***
	{
		WorldPacket pkt(SMSG_SET_PROFICIENCY, 8);
		pkt << (u_int)4;
		pkt << (u_int)12;
		sendData(pkt);
	}

	// ***TODO***
	{
		WorldPacket pkt(SMSG_SET_PROFICIENCY, 8);
		pkt << (u_int)4;
		pkt << (u_int)14;
		sendData(pkt);
	}

	// ***TODO***
	{
		WorldPacket pkt(SMSG_SET_PROFICIENCY, 8);
		pkt << (u_int)4;
		pkt << (u_int)78;
		sendData(pkt);
	}

	// ***TODO***
	{
		WorldPacket pkt(SMSG_SET_PROFICIENCY, 8);
		pkt << (u_int)4;
		pkt << (u_int)79;
		sendData(pkt);
	}

	// ���͸����Ѷ�����
	{
		WorldPacket pkt(SMSG_SET_DUNGEON_DIFFICULTY, 12);
		pkt << (u_int)0;				// 0Ϊ��ͨ, 1ΪӢ��
		pkt << (u_int)1;
		pkt << (u_int)0;
		sendData(pkt);
	}

	// ����У��������Ϣ����
	{
		WorldPacket pkt(SMSG_LOGIN_VERIFY_WORLD, 20);
		pkt << unitInterf->map();
		pkt << unitInterf->posX();
		pkt << unitInterf->posY();
		pkt << unitInterf->posZ();
		pkt << unitInterf->orientation();
		sendData(pkt);
	}

	// �����ʺ�MD5����
	{
		WorldPacket pkt(SMSG_ACCOUNT_DATA_MD5, 128);
		for (int i = 0; i < 32; ++i)
			pkt << (u_int)0;
		sendData(pkt);
	}

	// �����б�
	{
		WorldPacket pkt(SMSG_FRIEND_LIST, 1);
		pkt << (u_char)0;
		sendData(pkt);
	}

	// ***TODO*** �����б�

	// ��������ϵͳ״̬��������
	{
		WorldPacket pkt(SMSG_VOICE_SYSTEM_STATUS, 2);
		pkt << (u_char)2;
		pkt << (u_char)0;		// 1Ϊ���ÿͻ��������������ý���
		sendData(pkt);
	}

	// ���ͻ�ӭ��Ϣ
	// ***TODO*** ���Ӧ���ɷ��¼�, �ɽű������ͻ�ӭ��Ϣ
	{
		WorldPacket pkt(SMSG_MOTD, 50);
		pkt << (u_int)1;			// ����
		pkt << "Hello World.";
		sendData(pkt);
	}

	// ***TODO*** ������Ϣ

	// Unkown
	{
		WorldPacket pkt(SMSG_SET_REST_START, 4);
		pkt << (u_int)0;
		sendData(pkt);
	}

	// �����ù�λ�ø�����Ϣ
	// ***TODO*** �������ڷ������Ч��, ����������λ��
	{
		WorldPacket pkt(SMSG_BINDPOINTUPDATE, 20);
		pkt << unitInterf->posX();
		pkt << unitInterf->posY();
		pkt << unitInterf->posZ();
		pkt << unitInterf->map();
		pkt << unitInterf->zone();
		sendData(pkt);
	}

	// ***TODO*** SMSG_TUTORIAL_FLAGS �̳��б�
	{
		WorldPacket pkt(SMSG_TUTORIAL_FLAGS, 32);
		for (int i = 0; i < 8; ++i)
			pkt << (u_int)0;
		sendData(pkt);
	}

	// ***TODO*** SMSG_INITIAL_SPELLS �����б�
	{
		WorldPacket pkt(SMSG_INITIAL_SPELLS, 5);
		pkt << (u_char)0;
		pkt << (u_short)0;
		pkt << (u_short)0;
		sendData(pkt);
	}

	// ***TODO*** SMSG_ACTION_BUTTONS ������б�
	{
		#define MAX_ACTION_BUTTONS 132
		WorldPacket pkt(SMSG_ACTION_BUTTONS, MAX_ACTION_BUTTONS * 4);
		for (int i = 0; i < MAX_ACTION_BUTTONS; ++i)
			pkt << (u_int)0;
		sendData(pkt);
	}

	// ***TODO*** SMSG_INITIALIZE_FACTIONS ��������
	{
		WorldPacket pkt(SMSG_INITIALIZE_FACTIONS, 4 + 128 * 5);
		pkt << (u_int)0x00000080;
		for (int i = 0; i < 128; ++i)
		{
			pkt << (u_char)0;
			pkt << (u_int)0;
		}
		sendData(pkt);
	}

	// ***TODO*** ������Ϣ
	{
		WorldPacket pkt(SMSG_WEATHER, 9);
		pkt << (u_int)0;
		pkt << (float)0.0f;
		pkt << (u_char)0;
		sendData(pkt);
	}

	// ***TODO*** SMSG_INIT_WORLD_STATES ����״̬, ��ͬzone/map���͵����ݲ�һ��
	// ÿ����һ���µ�mapʱ���ᷢ���������, ������Ҫ��������, ����ýű�, ע�����map���¼�
	{
		u_short numOfFields = 10;

		WorldPacket pkt(SMSG_INIT_WORLD_STATES, 14 + numOfFields * 8);
		pkt << unitInterf->map();
		pkt << unitInterf->zone();
		pkt << unitInterf->area();
		pkt << numOfFields;						// count of uint64 blocks
		pkt << (u_int)0x8d8 << (u_int)(0x0);	// 1
		pkt << (u_int)0x8d7 << (u_int)(0x0);	// 2
		pkt << (u_int)0x8d6 << (u_int)(0x0);	// 3
		pkt << (u_int)0x8d5 << (u_int)(0x0);	// 4
		pkt << (u_int)0x8d4 << (u_int)(0x0);	// 5
		pkt << (u_int)0x8d3 << (u_int)(0x0);	// 6
		pkt << (u_int)0x914 << (u_int)(0x0);	// 7
		pkt << (u_int)0x913 << (u_int)(0x0);	// 8
		pkt << (u_int)0x912 << (u_int)(0x0);	// 9
		pkt << (u_int)0x915 << (u_int)(0x0);	// 10
		sendData(pkt);
	}

	// ���ͷ�����ʱ��
	// ***TODO*** ������ʱ����Ҫһ�����������
	{
		time_t gameTime = time(0);
		struct tm lt;
		localtime_s(&lt, &gameTime);
		u_int xmitTime = (lt.tm_year - 100) << 24 | lt.tm_mon  << 20 |
			(lt.tm_mday - 1) << 14 | lt.tm_wday << 11 |
			lt.tm_hour << 6 | lt.tm_min;

		WorldPacket pkt(SMSG_LOGIN_SETTIMESPEED, 8);
		pkt << xmitTime;					// ������ʱ��
		pkt << (float)0.01666667f;			// ��Ϸ�ٶ�
		sendData(pkt);
	}

	// ***TODO*** SMSG_TRIGGER_CINEMATIC ��������, �ýű�����������Щ����

	// �������Ըı����ݵ��ͻ���
	// ***TODO*** ��δ��벻Ӧ�÷�������
	{
		u_char updateType = UpdateData::UPDATETYPE_CREATE_OBJECT2;
		u_char flags = UpdateData::UPDATEFLAG_HIGHGUID | UpdateData::UPDATEFLAG_LIVING | 
			UpdateData::UPDATEFLAG_HASPOSITION | UpdateData::UPDATEFLAG_SELF;
		u_int flags2 = 0;

		ByteBuffer buf(500);
		buf << updateType;
		buf << (u_char)0xFF << guid;
		buf << (u_char)4;			// ***TODO*** OBJECT_TYPEID, ���Ϊ4, Ҫ��entity���ṩ��ѯ�ӿ�

		buf << flags;
		buf << flags2;				// �ƶ�����
		buf << (u_char)0;
		buf << (u_int)GetTickCount();

		buf << unitInterf->posX();
		buf << unitInterf->posY();
		buf << unitInterf->posZ();
		buf << unitInterf->orientation();

		buf << (u_int)0;			// fall time

		buf << (float)1.0f;			// 8���ƶ��ٶ�
		buf << (float)1.0f;
		buf << (float)1.0f;
		buf << (float)1.0f;
		buf << (float)1.0f;
		buf << (float)1.0f;
		buf << (float)1.0f;
		buf << (float)1.0f;

		buf << (u_int)0;

		UpdateMask mask;
		mask.count(propSet->valueCount());
		for (u_short i = 0; i < propSet->valueCount(); ++i)
		{
			if (propSet->getUintValue(i))
				mask.setBit(i);
		}

		buf << (u_char)mask.blocks();
		buf.append(mask.maskData(), mask.length());

		for (u_short i = 0; i < propSet->valueCount(); ++i)
			buf << propSet->getUintValue(i);

		UpdateData data;
		data.addUpdateBlock(buf);

		WorldPacket pkt;
		data.buildPacket(pkt);
		sendData(pkt);
	}

	// ***TODO***
	{
		WorldPacket pkt(SMSG_QUERY_TIME_RESPONSE, 8);
		pkt << (u_int)time(0);
		pkt << (u_int)0;
		sendData(pkt);
	}

	// ***TODO*** ���ﻹ�и�ѹ���ĸ��°� 502
	// ***TODO*** ���ﻹ�и����°� 169

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
