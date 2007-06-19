
// *-----------------------------------------------------------------
//
// 角色管理状态
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "CharacterState.h"
#include "WorldThread/WorldSocket.h"


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
	return true;
}

// 枚举角色
bool CharacterState::enumCharacter(WorldPacket& packet)
{
	WorldPacket data(SMSG_CHAR_ENUM, 100);

	u_char count = 0;
	data << count;
/*
	// 查询角色列表
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

// 删除角色
bool CharacterState::deleteCharacter(WorldPacket& packet)
{
	return true;
}

bool CharacterState::sendData(WorldPacket& packet)
{
	return dynamic_cast<WorldSocket*>(m_socketService)->sendData(packet);
}
