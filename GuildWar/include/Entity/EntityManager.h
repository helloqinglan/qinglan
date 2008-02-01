
// *-----------------------------------------------------------------
//
// 游戏实体对象管理器
//
// *-----------------------------------------------------------------

#pragma once

#include "Entity/Entity.h"

class EntityManager
{
	friend class ACE_Singleton<EntityManager, ACE_Recursive_Thread_Mutex>;

public:
	bool initialize();

	// 创建未初始化的实体对象
	Entity* createEntity();

	// 使用客户端数据包新建角色对象
	Entity* createEntity(WorldPacket& packet);

private:
	u_int m_hiCharGuid;			// 最大GUID
};

typedef ACE_Singleton<EntityManager, ACE_Recursive_Thread_Mutex> EntityManagerSingleton;
#define ENTITYMANAGER EntityManagerSingleton::instance()
