
// *-----------------------------------------------------------------
//
// MySQL查询结果集实现
//
// *-----------------------------------------------------------------

#pragma once

#include "GuildWarFwd.h"
#include "Database/Field.h"

class QueryResult
{
public:
    QueryResult(MYSQL_RES* result, u_int64 rowCount, u_int fieldCount);
    ~QueryResult();

	// 取下一行结果集
    bool nextRow();

    Field* fetch() const { return m_currentRow; }

    const Field& operator [] (int index) const { return m_currentRow[index]; }

    u_int getFieldCount() const { return m_fieldCount; }

    u_int64 getRowCount() const { return m_rowCount; }

protected:
	enum Field::DataTypes ConvertNativeType(enum_field_types mysqlType) const;
	void EndQuery();

private:
	MYSQL_RES *m_result;

    Field* m_currentRow;
    u_int m_fieldCount;
    u_int64 m_rowCount;
};
