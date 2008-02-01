
// *-----------------------------------------------------------------
//
// 角色管理状态
//
// *-----------------------------------------------------------------

#pragma once

#include "GuildWarFwd.h"
#include "NetWork/SocketState.h"

class CharacterState : public SocketState
{
public:
	CharacterState(SocketService* sm);

	// 处理数据
	bool process(void* arg = 0);

private:
	// 处理来自客户端的ping包
	bool pingRequest(WorldPacket& packet);

	// 创建角色
	bool createCharacter(WorldPacket& packet);

	// 枚举角色
	bool enumCharacter(WorldPacket& packet);

	// 删除角色
	bool deleteCharacter(WorldPacket& packet);

	// 进入游戏
	bool playerLogin(WorldPacket& packet);

	// ***TODO*** 未知请求
	bool realmSplitInfo(WorldPacket& packet);

	bool sendData(WorldPacket& packet);

private:
};
