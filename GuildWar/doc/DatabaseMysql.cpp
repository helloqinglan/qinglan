
// *-----------------------------------------------------------------
//
// MySQL���ݿ�ʵ��
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "DatabaseMysql.h"
#include "QueryResultMysql.h"
#include "Util/Util.h"

using namespace std;

DatabaseMysql::DatabaseMysql()
: Database(), m_mysqlHandle(0)
{
	mysql_library_init(-1, NULL, NULL);
}

DatabaseMysql::~DatabaseMysql()
{
	if (m_mysqlHandle)
		mysql_close(m_mysqlHandle);
    mysql_library_end();
}

bool DatabaseMysql::initialize(const char *infoString)
{
	// ��ʼ��Mysql�����ӵ�������
	// infoString�ĸ�ʽ: hostname;port;username;password;database

	MYSQL *mysqlInit = mysql_init(NULL);
	if (!mysqlInit)
	{
        ACE_ERROR ((GAME_ERROR ACE_TEXT("DatabaseMysql::Initialize ��ʼ��mysqlʧ��.\n")));
        return false;
	}

	// ***TODO*** �����boost����û���ֳɵĺ���, ��Ҫ�Լ�д
    vector<string> tokens = strSplit(infoString, ";");
	assert(tokens.size() == 5);
	if (tokens.size() != 5)
	{
		ACE_ERROR ((GAME_ERROR 
			ACE_TEXT("DatabaseMysql::Initialize ���ݿ����������, ��ʽ����Ϊhostname;port;username;password;database.\n")));
		return false;
	}

	// mysql���ݿ�ʹ��utf8�ַ���
    mysql_options(mysqlInit, MYSQL_SET_CHARSET_NAME, "utf8");

	// ***TODO*** ��Ϊʹ��boost::lexcial_cast����ת��
	int port = atoi(tokens[1].c_str());
	m_mysqlHandle = mysql_real_connect(mysqlInit, tokens[0].c_str(), tokens[2].c_str(),
		tokens[3].c_str(), tokens[4].c_str(), port, 0, 0);

	if (!m_mysqlHandle)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("DatabaseMysql::Initialize ���ӵ����ݿ�%sʧ��, ԭ��%s.\n"), 
			tokens[0].c_str(), mysql_error(mysqlInit)));
		mysql_close(mysqlInit);

        return false;
    }

	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("DatabaseMysql::Initialize ���ӵ����ݿ�%s�ɹ�.\n"), tokens[0].c_str()));

	return true;
}

QueryResult* DatabaseMysql::pquery(const char *format, ...)
{
	if (!format) 
		return 0;

	// ***TODO*** ��1024���magic number�ø������滻, ��øĳ��ǿ�֧�����ⳤ�ȵ��ַ���
	va_list ap;
	char szQuery[1024];
	va_start(ap, format);
	int res = ACE_OS::vsnprintf( szQuery, 1024, format, ap );
	va_end(ap);

    if (res == -1)
    {
		ACE_ERROR ((GAME_ERROR ACE_TEXT("DatabaseMysql::pquery ��Ч�ĸ�ʽ���ַ���: %s.\n"), format));
        return 0;
    }

	return query(szQuery);
}

QueryResult* DatabaseMysql::query(const char* sql)
{
	if (!m_mysqlHandle)
		return 0;

	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("DatabaseMysql::query SQL: %s.\n"), sql));

    if(mysql_query(m_mysqlHandle, sql))
    {
		ACE_ERROR ((GAME_ERROR ACE_TEXT("DatabaseMysql::query ��ѯʧ��, ԭ��: %s.\n"), mysql_error(m_mysqlHandle)));
		return 0;
    }

	MYSQL_RES* result = mysql_store_result(m_mysqlHandle);
	my_ulonglong rowCount = mysql_affected_rows(m_mysqlHandle);
	unsigned int fieldCount = mysql_field_count(m_mysqlHandle);

	if (!result )
		return 0;

	if (!rowCount)
	{
		mysql_free_result(result);
		return 0;
    }

	QueryResultMysql* queryResult = new QueryResultMysql(result, rowCount, fieldCount);
	queryResult->nextRow();

	return queryResult;
}

bool DatabaseMysql::execute(const char* sql)
{
	if (!m_mysqlHandle)
		return false;

	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("DatabaseMysql::execute SQL: %s.\n"), sql));

    if(mysql_query(m_mysqlHandle, sql))
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("DatabaseMysql::execute ִ��ʧ��, ԭ��: %s.\n"), mysql_error(m_mysqlHandle)));
		return false;
	}

	return true;
}

bool DatabaseMysql::pexecute(const char* format, ...)
{
	if (!format)
		return false;

	// ***TODO*** ��1024���magic number�ø������滻, ��øĳ��ǿ�֧�����ⳤ�ȵ��ַ���
	va_list ap;
	char szQuery[1024];
	va_start(ap, format);
	int res = ACE_OS::vsnprintf( szQuery, 1024, format, ap );
	va_end(ap);

	if (res == -1)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("DatabaseMysql::pexecute ��Ч�ĸ�ʽ���ַ���: %s.\n"), format));
		return false;
	}

	return execute(szQuery);
}

unsigned long DatabaseMysql::escape_string(char* to, const char* from, unsigned long length)
{
	if (!m_mysqlHandle || !to || !from || !length)
		return 0;

	return mysql_real_escape_string(m_mysqlHandle, to, from, length);
}
