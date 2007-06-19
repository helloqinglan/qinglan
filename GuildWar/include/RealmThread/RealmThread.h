
// *-----------------------------------------------------------------
//
// ��¼�����߳�
//
// *-----------------------------------------------------------------

#pragma once

class RealmThread : public ACE_Task_Base
{
	friend class ACE_Singleton<RealmThread, ACE_Recursive_Thread_Mutex>;

public:
	int svc();

	// �����͹ر��߳�
	int open();
	int close();

	// ��ѯ�ʺż�����
	const std::string& accountName() const { return m_accountName; }
	const std::string& accountPass() const { return m_accountPass; }

	// ��ѯ�ѵ�¼�ʺŵ�session key
	const BigNumber& sessionK() const { return K; }
	const BigNumber& sessionv() const { return v; }
	const BigNumber& sessions() const { return s; }

	void sessionK(const BigNumber& val) { K = val; }
	void sessionv(const BigNumber& val) { v = val; }
	void sessions(const BigNumber& val) { s = val; }

protected:
	RealmThread();
	~RealmThread();

	bool loadAccounts();

private:
	bool m_running;

	// ***TODO*** ���ʺŻ��浥����һ��ģ��, ����ֻ֧��һ���ʺ�, ���Լ򵥴�����
	std::string m_accountName;	// �ʺ��û��� ���������
	std::string m_accountPass;	// �ʺŵ�����

	BigNumber K, v, s;			// Session Key���
};

typedef ACE_Singleton<RealmThread, ACE_Recursive_Thread_Mutex> RealmThreadSingleton;
#define REALM_THREAD RealmThreadSingleton::instance()
