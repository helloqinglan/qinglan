
// *-----------------------------------------------------------------
//
// ��Ϸʵ����������
//
// *-----------------------------------------------------------------

#pragma once

class EntityManager
{
	friend class ACE_Singleton<EntityManager, ACE_Recursive_Thread_Mutex>;

public:
};

typedef ACE_Singleton<EntityManager, ACE_Recursive_Thread_Mutex> EntityManagerSingleton;
#define ENTITYMANAGER EntityManagerSingleton::instance()
