
// *-----------------------------------------------------------------
//
// MySQL数据库实现
//
// *-----------------------------------------------------------------

#pragma once

#include "Database/QueryResult.h"

class Database
{
	friend class ACE_Singleton<Database, ACE_Recursive_Thread_Mutex>;

public:
    bool initialize(const char *infoString);

	// 查询
    QueryResult* query(const char *sql);
    QueryResult* pquery(const char *format, ...);

	// 执行
    bool execute(const char *sql);
    bool pexecute(const char *format, ...);

	operator bool () const 
	{
		return m_mysqlHandle != 0; 
	}

	// 格式化字符串
    unsigned long escape_string(char *to, const char *from, unsigned long length);
    void escape_string(std::string& str);

protected:
	Database();
	~Database();

private:
	MYSQL* m_mysqlHandle;

	// ***TODO*** RealmThread对数据库的使用已经限制在系统启动时, 加锁操作应该可以取消
	ACE_Thread_Mutex m_mysqlMutex;
};

typedef ACE_Singleton<Database, ACE_Recursive_Thread_Mutex> DatabaseSingleton;
#define DATABASE DatabaseSingleton::instance()
