
// *-----------------------------------------------------------------
//
// MySQL���ݿ�ʵ��
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "Util/Util.h"
#include "Database/Database.h"
#include "Database/QueryResult.h"

using namespace std;

Database::Database() : m_mysqlHandle(0)
{
	mysql_library_init(-1, 0, 0);
}

Database::~Database()
{
	if (m_mysqlHandle)
		mysql_close(m_mysqlHandle);

	mysql_library_end();
}

bool Database::initialize(const char *infoString)
{
	ACE_Guard<ACE_Thread_Mutex> guard(m_mysqlMutex);

	// ��ʼ��Mysql�����ӵ�������
	// infoString�ĸ�ʽ: hostname;port;username;password;database

	MYSQL* mysqlInit = mysql_init(0);
	if (!mysqlInit)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("Database::Initialize ��ʼ��mysqlʧ��.\n")));
		return false;
	}

	// ***TODO*** �����boost����û���ֳɵĺ���, ��Ҫ�Լ�д
	vector<string> tokens = strSplit(infoString, ";");
	assert(tokens.size() == 5);
	if (tokens.size() != 5)
	{
		ACE_ERROR ((GAME_ERROR 
			ACE_TEXT("Database::Initialize ���ݿ����������, ��ʽ����Ϊhostname;port;username;password;database.\n")));
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
		ACE_ERROR ((GAME_ERROR ACE_TEXT("Database::Initialize ���ӵ����ݿ�%sʧ��, ԭ��%s.\n"), 
			tokens[0].c_str(), mysql_error(mysqlInit)));
		mysql_close(mysqlInit);

		return false;
	}

	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("Database::Initialize ���ӵ����ݿ�%s�ɹ�.\n"), tokens[0].c_str()));

	return true;
}

QueryResult* Database::pquery(const char *format, ...)
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
		ACE_ERROR ((GAME_ERROR ACE_TEXT("Database::pquery ��Ч�ĸ�ʽ���ַ���: %s.\n"), format));
		return 0;
	}

	return query(szQuery);
}

QueryResult* Database::query(const char* sql)
{
	ACE_Guard<ACE_Thread_Mutex> guard(m_mysqlMutex);

	if (!m_mysqlHandle)
		return 0;

	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("Database::query SQL: %s.\n"), sql));

	if(mysql_query(m_mysqlHandle, sql))
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("Database::query ��ѯʧ��, ԭ��: %s.\n"), mysql_error(m_mysqlHandle)));
		return 0;
	}

	MYSQL_RES* result = mysql_store_result(m_mysqlHandle);
	u_int64 rowCount = mysql_affected_rows(m_mysqlHandle);
	u_int fieldCount = mysql_field_count(m_mysqlHandle);

	if (!result )
		return 0;

	if (!rowCount)
	{
		mysql_free_result(result);
		return 0;
	}

	QueryResult* queryResult = new QueryResult(result, rowCount, fieldCount);
	queryResult->nextRow();

	return queryResult;
}

bool Database::execute(const char* sql)
{
	ACE_Guard<ACE_Thread_Mutex> guard(m_mysqlMutex);

	if (!m_mysqlHandle)
		return false;

	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("Database::execute SQL: %s.\n"), sql));

	if(mysql_query(m_mysqlHandle, sql))
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("Database::execute ִ��ʧ��, ԭ��: %s.\n"), mysql_error(m_mysqlHandle)));
		return false;
	}

	return true;
}

bool Database::pexecute(const char* format, ...)
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
		ACE_ERROR ((GAME_ERROR ACE_TEXT("Database::pexecute ��Ч�ĸ�ʽ���ַ���: %s.\n"), format));
		return false;
	}

	return execute(szQuery);
}

// ��ʽ���ַ���
unsigned long Database::escape_string(char* to, const char* from, unsigned long length)
{
	ACE_Guard<ACE_Thread_Mutex> guard(m_mysqlMutex);

	if (!m_mysqlHandle || !to || !from || !length)
		return 0;

	return mysql_real_escape_string(m_mysqlHandle, to, from, length);
}

void Database::escape_string(std::string& str)
{
	if (!str.size())
		return;

	char* buf = new char[str.size()*2 + 1];
	escape_string(buf, str.c_str(), (u_int)str.size());

	str = buf;
	delete[] buf;
}
