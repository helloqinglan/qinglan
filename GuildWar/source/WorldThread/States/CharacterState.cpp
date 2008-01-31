
// *-----------------------------------------------------------------
//
// 角色管理状态
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
// 该状态下处理的客户端消息码
#define CMSG_PING						476		// Ping包
#define CMSG_CHAR_CREATE				54		// 创建角色
#define CMSG_CHAR_ENUM					55		// 角色列表
#define CMSG_CHAR_DELETE				56		// 删除角色
#define CMSG_PLAYER_LOGIN				61		// 进入游戏
#define CMSG_REALM_SPLIT_INFO_REQUEST	908		// ***TODO*** 在角色选择界面出现, 不知道做什么的

// 该状态下服务器会发送的消息码
#define SMSG_PONG						477		// Ping返回包
#define SMSG_CHAR_CREATE				58		// 创建角色
#define SMSG_CHAR_ENUM					59		// 角色列表
#define SMSG_CHAR_DELETE				60		// 删除角色
#define SMSG_REALM_SPLIT_INFO_RESPONSE	907		// 对908的应答

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
		entity->save();
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
/*	Entity* entity = ENTITYMANAGER->createEntity(Entity::Type_Player);
	if (!entity->load(id))
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("CharacterState::playerLogin 从数据库加载角色数据失败: %d.\n"), packet->getOpcode()));
		return false;
	}
*/
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
