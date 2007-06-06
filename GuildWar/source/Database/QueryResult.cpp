
// *-----------------------------------------------------------------
//
// MySQL查询结果集实现
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "Database/QueryResult.h"

QueryResult::QueryResult(MYSQL_RES *result, u_int64 rowCount, u_int fieldCount)
: m_fieldCount(fieldCount), m_rowCount(rowCount), m_result(result)
{
	m_currentRow = new Field[m_fieldCount];
	assert(m_currentRow);

	MYSQL_FIELD *fields = mysql_fetch_fields(m_result);
	for (u_int i = 0; i < m_fieldCount; i++)
	{
		m_currentRow[i].getName(fields[i].name);
		m_currentRow[i].getType(ConvertNativeType(fields[i].type));
	}
}

QueryResult::~QueryResult()
{
	EndQuery();
}

bool QueryResult::nextRow()
{
	MYSQL_ROW row;

	if (!m_result)
		return false;

	row = mysql_fetch_row(m_result);
	if (!row)
	{
		EndQuery();
		return false;
	}

	for (u_int i = 0; i < m_fieldCount; i++)
		m_currentRow[i].setValue(row[i]);

	return true;
}

void QueryResult::EndQuery()
{
	if (m_currentRow)
	{
		delete [] m_currentRow;
		m_currentRow = 0;
	}

	if (m_result)
	{
		mysql_free_result(m_result);
		m_result = 0;
	}
}

enum Field::DataTypes QueryResult::ConvertNativeType(enum_field_types mysqlType) const
{
	switch (mysqlType)
	{
	case FIELD_TYPE_TIMESTAMP:
	case FIELD_TYPE_DATE:
	case FIELD_TYPE_TIME:
	case FIELD_TYPE_DATETIME:
	case FIELD_TYPE_YEAR:
	case FIELD_TYPE_STRING:
	case FIELD_TYPE_VAR_STRING:
	case FIELD_TYPE_BLOB:
	case FIELD_TYPE_SET:
	case FIELD_TYPE_NULL:
		return Field::DB_TYPE_STRING;

	case FIELD_TYPE_TINY:
	case FIELD_TYPE_SHORT:
	case FIELD_TYPE_LONG:
	case FIELD_TYPE_INT24:
	case FIELD_TYPE_LONGLONG:
	case FIELD_TYPE_ENUM:
		return Field::DB_TYPE_INTEGER;

	case FIELD_TYPE_DECIMAL:
	case FIELD_TYPE_FLOAT:
	case FIELD_TYPE_DOUBLE:
		return Field::DB_TYPE_FLOAT;

	default:
		return Field::DB_TYPE_UNKNOWN;
	}
}
