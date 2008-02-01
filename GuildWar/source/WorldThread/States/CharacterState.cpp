
// *-----------------------------------------------------------------
//
// 角色管理状态
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "CharacterState.h"

#include "Component/DataIOComp.h"
#include "Component/UnitInterfComp.h"
#include "Entity/EntityDefines.h"
#include "Entity/EntityManager.h"
#include "Entity/Entity.h"

#include "WorldThread/WorldSocket.h"
#include "Database/Database.h"


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
#define SMSG_BINDPOINTUPDATE			341		// 旅馆位置更新
#define SMSG_ACCOUNT_DATA_MD5			521		// 帐号MD5
#define SMSG_SET_REST_START				542		// ***TODO*** 进入游戏时发送
#define SMSG_LOGIN_VERIFY_WORLD			566		// 校验坐标信息
#define SMSG_INIT_WORLD_STATES			706		// 世界状态信息
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
}

bool CharacterState::process(void* arg)
{
	// ***TODO*** 丑陋的强制类型转换, 改成使用boost::any
	WorldPacket* packet = (WorldPacket*)arg;

	// ***TODO*** 在SocketState基类中提供对消息注册事件处理的方法
	//            去掉这些冗长的if...else
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
		ACE_ERROR ((GAME_ERROR ACE_TEXT("CharacterState::process 未知的消息码: %d.\n"), packet->getOpcode()));
		return false;
	}

	return true;
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

	if (!dataIO->load(GUID_LOPART(guid)))
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("CharacterState::playerLogin 从数据库加载角色数据失败: %u.\n"), GUID_LOPART(guid)));
		return false;
	}

	// 发送副本难度数据
	{
		WorldPacket pkt(SMSG_LOGIN_VERIFY_WORLD, 20);
		pkt << (u_int)0;				// 0为普通, 1为英雄
		pkt << (u_int)1;
		pkt << (u_int)0;
		sendData(pkt);
	}

	// 发送校验坐标信息数据
	{
		WorldPacket pkt(SMSG_SET_DUNGEON_DIFFICULTY, 12);
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

	// ***TODO*** 好友列表
	// ***TODO*** 屏蔽列表

	// 发送语音系统状态设置数据
	{
		WorldPacket pkt(SMSG_VOICE_SYSTEM_STATUS, 2);
		pkt << (u_char)2;
		pkt << (u_char)0;		// 1为启用客户端聊天界面
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

	// ***TODO*** SMSG_TUTORIAL_FLAGS 是不是可以不发
	// ***TODO*** SMSG_INITIAL_SPELLS 技能列表
	// ***TODO*** SMSG_ACTION_BUTTONS 快捷栏列表
	// ***TODO*** SMSG_INITIALIZE_FACTIONS 声望数据

	// ***TODO*** SMSG_INIT_WORLD_STATES 世界状态, 不同zone/map发送的数据不一样
	// 每进入一个新的map时都会发送这个数据, 所以需要独立出来, 最好用脚本, 注册进入map的事件
	{
		WorldPacket pkt(SMSG_INIT_WORLD_STATES, 14);
		pkt << unitInterf->map();
		pkt << unitInterf->zone();
		pkt << unitInterf->area();
		pkt << (u_int)0;						// count of uint64 blocks
		pkt << (u_int)0x8d8 << (u_int)(0x0);	// 1
		pkt << (u_int)0x8d7 << (u_int)(0x0);	// 2
		pkt << (u_int)0x8d6 << (u_int)(0x0);	// 3
		pkt << (u_int)0x8d5 << (u_int)(0x0);	// 4
		pkt << (u_int)0x8d4 << (u_int)(0x0);	// 5
		pkt << (u_int)0x8d3 << (u_int)(0x0);	// 6
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
