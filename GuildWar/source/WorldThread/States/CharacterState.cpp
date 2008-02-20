
// *-----------------------------------------------------------------
//
// 角色管理状态
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

// ***TODO*** 这些不应该放在这里
#include "Entity/UpdateData.h"
#include "Entity/UpdateMask.h"


// *-----------------------------------------------------------------
//
// 该状态下处理的客户端消息码
#define CMSG_PING						476		// Ping包
#define CMSG_CHAR_CREATE				54		// 创建角色
#define CMSG_CHAR_ENUM					55		// 角色列表
#define CMSG_CHAR_DELETE				56		// 删除角色
#define CMSG_PLAYER_LOGIN				61		// 进入游戏
#define CMSG_REALM_SPLIT_INFO_REQUEST	908		// ***TODO*** 在角色选择界面出现

// 该状态下服务器会发送的消息码
#define SMSG_PONG						477		// Ping返回包
#define SMSG_CHAR_CREATE				58		// 创建角色
#define SMSG_CHAR_ENUM					59		// 角色列表
#define SMSG_CHAR_DELETE				60		// 删除角色
#define	SMSG_LOGIN_SETTIMESPEED			66		// 服务器时间
#define SMSG_FRIEND_LIST				103		// 好友列表
#define SMSG_TUTORIAL_FLAGS				253		// 教程列表
#define SMSG_INITIALIZE_FACTIONS		290		// 阵营信息
#define SMSG_SET_PROFICIENCY			295		// ***TODO***
#define SMSG_ACTION_BUTTONS				297		// 快捷栏列表
#define SMSG_INITIAL_SPELLS				298		// 技能列表
#define SMSG_UPDATE_AURA_DURATION		311		// ***TODO***
#define SMSG_BINDPOINTUPDATE			341		// 旅馆位置更新
#define SMSG_QUERY_TIME_RESPONSE		463		// ***TODO***
#define SMSG_ACCOUNT_DATA_MD5			521		// 帐号MD5
#define SMSG_SET_REST_START				542		// ***TODO*** 进入游戏时发送
#define SMSG_LOGIN_VERIFY_WORLD			566		// 校验坐标信息
#define SMSG_INIT_WORLD_STATES			706		// 世界状态信息
#define SMSG_WEATHER					756		// 天气信息
#define SMSG_SET_DUNGEON_DIFFICULTY		809		// 副本难度
#define SMSG_MOTD						829		// 欢迎信息
#define SMSG_REALM_SPLIT_INFO_RESPONSE	907		// 对908的应答
#define SMSG_VOICE_SYSTEM_STATUS		968		// 语音系统状态设置

// 角色创建的结果码
enum E_CHAR_CREATE_CODE
{
	CHAR_CREATE_SUCCESS	= 0x2E,	// 成功
	CHAR_CREATE_ERROR	= 0x2F,	// 错误
	CHAR_CREATE_FAILED	= 0x30,	// 失败
};

// 角色删除的结果码
enum E_CHAR_DELETE_CODE
{
	CHAR_DELETE_SUCCESS	= 0x3A,	// 成功
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
		ACE_ERROR ((GAME_ERROR ACE_TEXT("CharacterState::process 未知的消息码: %d.\n"), packet->getOpcode()));

	return false;
}

// 每种状态下都需要处理该消息
// 是不是找种更合理的方法来处理这个消息包?
bool CharacterState::pingRequest(WorldPacket& packet)
{
	u_int ping;
	packet >> ping;

	// 正常情况下, 两次ping之间的间隔为30秒
	// 可以此来判断客户端有没有用加速外挂

	WorldPacket pkt(SMSG_PONG, 4);
	pkt << ping;
	return sendData(pkt);
}

// 创建角色
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

// 枚举角色
bool CharacterState::enumCharacter(WorldPacket& packet)
{
	WorldPacket data(SMSG_CHAR_ENUM, 100);

	u_char count = 0;
	data << count;

	// 查询角色列表
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

// 删除角色
bool CharacterState::deleteCharacter(WorldPacket& packet)
{
	u_int64 guid;
	packet >> guid;

	// ***TODO*** 删除与该角色相关的所有数据
	DATABASE->pexecute("DELETE FROM `characters` WHERE `id` = '%u'", GUID_LOPART(guid));

	WorldPacket pkt(SMSG_CHAR_DELETE, 1);
	pkt << (u_char)CHAR_DELETE_SUCCESS;
	sendData(pkt);

	return true;
}

// 进入游戏
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
		ACE_ERROR ((GAME_ERROR ACE_TEXT("CharacterState::playerLogin 从数据库加载角色数据失败: %u.\n"), GUID_LOPART(guid)));
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

	// 发送副本难度数据
	{
		WorldPacket pkt(SMSG_SET_DUNGEON_DIFFICULTY, 12);
		pkt << (u_int)0;				// 0为普通, 1为英雄
		pkt << (u_int)1;
		pkt << (u_int)0;
		sendData(pkt);
	}

	// 发送校验坐标信息数据
	{
		WorldPacket pkt(SMSG_LOGIN_VERIFY_WORLD, 20);
		pkt << unitInterf->map();
		pkt << unitInterf->posX();
		pkt << unitInterf->posY();
		pkt << unitInterf->posZ();
		pkt << unitInterf->orientation();
		sendData(pkt);
	}

	// 发送帐号MD5数据
	{
		WorldPacket pkt(SMSG_ACCOUNT_DATA_MD5, 128);
		for (int i = 0; i < 32; ++i)
			pkt << (u_int)0;
		sendData(pkt);
	}

	// 好友列表
	{
		WorldPacket pkt(SMSG_FRIEND_LIST, 1);
		pkt << (u_char)0;
		sendData(pkt);
	}

	// ***TODO*** 屏蔽列表

	// 发送语音系统状态设置数据
	{
		WorldPacket pkt(SMSG_VOICE_SYSTEM_STATUS, 2);
		pkt << (u_char)2;
		pkt << (u_char)0;		// 1为启用客户端语言聊天设置界面
		sendData(pkt);
	}

	// 发送欢迎信息
	// ***TODO*** 这个应该派发事件, 由脚本来发送欢迎信息
	{
		WorldPacket pkt(SMSG_MOTD, 50);
		pkt << (u_int)1;			// 行数
		pkt << "Hello World.";
		sendData(pkt);
	}

	// ***TODO*** 公会信息

	// Unkown
	{
		WorldPacket pkt(SMSG_SET_REST_START, 4);
		pkt << (u_int)0;
		sendData(pkt);
	}

	// 发送旅馆位置更新信息
	// ***TODO*** 做类似于飞行棋的效果, 可随意设置位置
	{
		WorldPacket pkt(SMSG_BINDPOINTUPDATE, 20);
		pkt << unitInterf->posX();
		pkt << unitInterf->posY();
		pkt << unitInterf->posZ();
		pkt << unitInterf->map();
		pkt << unitInterf->zone();
		sendData(pkt);
	}

	// ***TODO*** SMSG_TUTORIAL_FLAGS 教程列表
	{
		WorldPacket pkt(SMSG_TUTORIAL_FLAGS, 32);
		for (int i = 0; i < 8; ++i)
			pkt << (u_int)0;
		sendData(pkt);
	}

	// ***TODO*** SMSG_INITIAL_SPELLS 技能列表
	{
		WorldPacket pkt(SMSG_INITIAL_SPELLS, 5);
		pkt << (u_char)0;
		pkt << (u_short)0;
		pkt << (u_short)0;
		sendData(pkt);
	}

	// ***TODO*** SMSG_ACTION_BUTTONS 快捷栏列表
	{
		#define MAX_ACTION_BUTTONS 132
		WorldPacket pkt(SMSG_ACTION_BUTTONS, MAX_ACTION_BUTTONS * 4);
		for (int i = 0; i < MAX_ACTION_BUTTONS; ++i)
			pkt << (u_int)0;
		sendData(pkt);
	}

	// ***TODO*** SMSG_INITIALIZE_FACTIONS 声望数据
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

	// ***TODO*** 天气信息
	{
		WorldPacket pkt(SMSG_WEATHER, 9);
		pkt << (u_int)0;
		pkt << (float)0.0f;
		pkt << (u_char)0;
		sendData(pkt);
	}

	// ***TODO*** SMSG_INIT_WORLD_STATES 世界状态, 不同zone/map发送的数据不一样
	// 每进入一个新的map时都会发送这个数据, 所以需要独立出来, 最好用脚本, 注册进入map的事件
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

	// 发送服务器时间
	// ***TODO*** 服务器时间需要一个独立的组件
	{
		time_t gameTime = time(0);
		struct tm lt;
		localtime_s(&lt, &gameTime);
		u_int xmitTime = (lt.tm_year - 100) << 24 | lt.tm_mon  << 20 |
			(lt.tm_mday - 1) << 14 | lt.tm_wday << 11 |
			lt.tm_hour << 6 | lt.tm_min;

		WorldPacket pkt(SMSG_LOGIN_SETTIMESPEED, 8);
		pkt << xmitTime;					// 服务器时间
		pkt << (float)0.01666667f;			// 游戏速度
		sendData(pkt);
	}

	// ***TODO*** SMSG_TRIGGER_CINEMATIC 触发动画, 用脚本看看都有哪些动画

	// 发送属性改变数据到客户端
	// ***TODO*** 这段代码不应该放在这里
	{
		u_char updateType = UpdateData::UPDATETYPE_CREATE_OBJECT2;
		u_char flags = UpdateData::UPDATEFLAG_HIGHGUID | UpdateData::UPDATEFLAG_LIVING | 
			UpdateData::UPDATEFLAG_HASPOSITION | UpdateData::UPDATEFLAG_SELF;
		u_int flags2 = 0;

		ByteBuffer buf(500);
		buf << updateType;
		buf << (u_char)0xFF << guid;
		buf << (u_char)4;			// ***TODO*** OBJECT_TYPEID, 玩家为4, 要在entity中提供查询接口

		buf << flags;
		buf << flags2;				// 移动掩码
		buf << (u_char)0;
		buf << (u_int)GetTickCount();

		buf << unitInterf->posX();
		buf << unitInterf->posY();
		buf << unitInterf->posZ();
		buf << unitInterf->orientation();

		buf << (u_int)0;			// fall time

		buf << (float)1.0f;			// 8种移动速度
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

	// ***TODO*** 这里还有个压缩的更新包 502
	// ***TODO*** 这里还有个更新包 169

	return true;
}

// ***TODO*** 未知请求
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
