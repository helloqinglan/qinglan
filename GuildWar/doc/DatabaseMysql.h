
// *-----------------------------------------------------------------
//
// MySQL数据库实现
//
// *-----------------------------------------------------------------

#pragma once

#include "GuildWarFwd.h"
#include "Database/Database.h"

class DatabaseMysql : public Database
{
public:
	DatabaseMysql();
	~DatabaseMysql();

	bool initialize(const char* infoString);

	QueryResult* query(const char* sql);
	QueryResult* pquery(const char* format, ...);

	bool execute(const char* sql);
	bool pexecute(const char* format, ...);

	operator bool () const 
	{
		return m_mysqlHandle != 0; 
	}

	unsigned long escape_string(char* to, const char* from, unsigned long length);
	using Database::escape_string;

private:
    MYSQL* m_mysqlHandle;
};
