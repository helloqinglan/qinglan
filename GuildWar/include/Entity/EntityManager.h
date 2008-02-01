
// *-----------------------------------------------------------------
//
// ��Ϸʵ����������
//
// *-----------------------------------------------------------------

#pragma once

#include "Entity/Entity.h"

class EntityManager
{
	friend class ACE_Singleton<EntityManager, ACE_Recursive_Thread_Mutex>;

public:
	bool initialize();

	// ����δ��ʼ����ʵ�����
	Entity* createEntity();

	// ʹ�ÿͻ������ݰ��½���ɫ����
	Entity* createEntity(WorldPacket& packet);

private:
	u_int m_hiCharGuid;			// ���GUID
};

typedef ACE_Singleton<EntityManager, ACE_Recursive_Thread_Mutex> EntityManagerSingleton;
#define ENTITYMANAGER EntityManagerSingleton::instance()
