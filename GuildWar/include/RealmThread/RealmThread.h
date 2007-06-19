
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

	// 开启和关闭线程
	int open();
	int close();

	// 查询帐号及密码
	const std::string& accountName() const { return m_accountName; }
	const std::string& accountPass() const { return m_accountPass; }

	// 查询已登录帐号的session key
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

	// ***TODO*** 将帐号缓存单独出一个模块, 现在只支持一个帐号, 所以简单处理了
	std::string m_accountName;	// 帐号用户名 缓存的数据
	std::string m_accountPass;	// 帐号的密码

	BigNumber K, v, s;			// Session Key相关
};

typedef ACE_Singleton<RealmThread, ACE_Recursive_Thread_Mutex> RealmThreadSingleton;
#define REALM_THREAD RealmThreadSingleton::instance()
