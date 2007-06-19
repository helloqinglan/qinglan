
// *-----------------------------------------------------------------
//
// ��Ϸ���紦���߳�
//
// *-----------------------------------------------------------------

#pragma once

class WorldThread : public ACE_Task_Base
{
	friend class ACE_Singleton<WorldThread, ACE_Recursive_Thread_Mutex>;

public:
	int svc();

	// �����͹ر��߳�
	int open();
	int close();

	// �Ƿ��������ѽ���
	bool hasConnection() const { return m_hasConnection; }

protected:
	WorldThread();
	~WorldThread();

private:
	bool m_running;
	bool m_hasConnection;
};

typedef ACE_Singleton<WorldThread, ACE_Recursive_Thread_Mutex> WorldThreadSingleton;
#define WORLD_THREAD WorldThreadSingleton::instance()
