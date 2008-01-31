
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
#define CMSG_PING				476		// Ping包
#define CMSG_CHAR_CREATE		54		// 创建角色
#define CMSG_CHAR_ENUM			55		// 角色列表
#define CMSG_CHAR_DELETE		56		// 删除角色

// 该状态下服务器会发送的消息码
#define SMSG_PONG				477		// Ping返回包
#define SMSG_CHAR_CREATE		58		// 创建角色
#define SMSG_CHAR_ENUM			59		// 角色列表
#define SMSG_CHAR_DELETE		60		// 删除角色

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
	else
	{
		assert(0 && "CharacterState::process unkown cmd.");
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

			// 发送的数据

			data << MAKE_GUID(id, HIGHGUID_PLAYER);		// GUID
			data << name;								// 角色名

			data << (u_char)0;							// ***TODO*** : 种族
			data << (u_char)0;							// ***TODO*** : 职业
			data << (u_char)0;							// ***TODO*** : 性别

			data << (u_int)0;							// ***TODO*** : PLAYER_BYTES
			data << (u_char)0;							// ***TODO*** : PLAYER_BYTES_2

			data << (u_char)1;							// ***TODO*** : 等级
			data << (u_int)0;							// ***TODO*** : ZoneId 区域ID
			data << map;								// ***TODO*** : 地图ID

			data << pos_x;								// 坐标
			data << pos_y;
			data << pos_z;

			data << (u_int)0;							// ***TODO*** : 公会ID

			data << (u_char)0;							// ***TODO*** : 未知 (different values on off, looks like flags)
			data << (u_char)0;							// ***TODO*** : 显示标志 (见Player.cpp 1123行)
			data << (u_char)0xa0;						// ***TODO*** : 未知
			data << (u_char)0;							// ***TODO*** : 未知
			data << (u_char)1;							// ***TODO*** : 未知

			data << (u_int)0;							// ***TODO*** : 宠物信息
			data << (u_int)0;
			data << (u_int)0;

			// ***TODO*** : 装备信息
			for (int i = 0; i < EQUIPMENT_SLOT_END; ++i)
			{
				data << (u_int)0;						// ***TODO*** : 显示ID
				data << (u_char)0;						// ***TODO*** : InventoryType (什么意思?)
			}

			data << (u_int)0;							// ***TODO*** : 未知 (first bag display id)
			data << (u_char)0;							// ***TODO*** : 未知 (first bag inventory type)

			++count;
		}
		while(result->nextRow());

		delete result;
	}

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

bool CharacterState::sendData(WorldPacket& packet)
{
	return dynamic_cast<WorldSocket*>(m_socketService)->sendData(packet);
}
