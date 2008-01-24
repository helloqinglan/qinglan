
// *-----------------------------------------------------------------
//
// MySQL���ݿ�ʵ��
//
// *-----------------------------------------------------------------

#pragma once

#include "Database/QueryResult.h"

class Database
{
	friend class ACE_Singleton<Database, ACE_Recursive_Thread_Mutex>;

public:
    bool initialize(const char *infoString);

	// ��ѯ
    QueryResult* query(const char *sql);
    QueryResult* pquery(const char *format, ...);

	// ִ��
    bool execute(const char *sql);
    bool pexecute(const char *format, ...);

	operator bool () const 
	{
		return m_mysqlHandle != 0; 
	}

	// ��ʽ���ַ���
    unsigned long escape_string(char *to, const char *from, unsigned long length);
    void escape_string(std::string& str);

protected:
	Database();
	~Database();

private:
	MYSQL* m_mysqlHandle;

	// ***TODO*** RealmThread�����ݿ��ʹ���Ѿ�������ϵͳ����ʱ, ��������Ӧ�ÿ���ȡ��
	ACE_Thread_Mutex m_mysqlMutex;
};

typedef ACE_Singleton<Database, ACE_Recursive_Thread_Mutex> DatabaseSingleton;
#define DATABASE DatabaseSingleton::instance()
