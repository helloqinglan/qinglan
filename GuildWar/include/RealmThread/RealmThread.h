
// *-----------------------------------------------------------------
//
// 登录处理线程
//
// *-----------------------------------------------------------------

#pragma once

class RealmThread : public ACE_Task_Base
{
	friend class ACE_Singleton<RealmThread, ACE_Recursive_Thread_Mutex>;

public:
	int svc();

	int open();
	int close();

protected:
	RealmThread();
	~RealmThread();

private:
	bool m_running;
};

typedef ACE_Singleton<RealmThread, ACE_Recursive_Thread_Mutex> RealmThreadSingleton;
#define REALM_THREAD RealmThreadSingleton::instance()
