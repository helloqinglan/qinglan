
// *-----------------------------------------------------------------
//
// 游戏实体对象管理器
//
// *-----------------------------------------------------------------

#pragma once

#include "Object/Entity.h"

class WorldPacket;

class EntityManager
{
	friend class ACE_Singleton<EntityManager, ACE_Recursive_Thread_Mutex>;

public:
	bool initialize();

	// 创建实体对象
	Entity* createEntity(Entity::EntityType eType);

	// 客户端创建角色
	Entity* createEntity(WorldPacket& packet);

private:
	u_int m_hiCharGuid;			// 最大GUID
};

typedef ACE_Singleton<EntityManager, ACE_Recursive_Thread_Mutex> EntityManagerSingleton;
#define ENTITYMANAGER EntityManagerSingleton::instance()
