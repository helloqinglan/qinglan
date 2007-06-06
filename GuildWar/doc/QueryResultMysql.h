
// *-----------------------------------------------------------------
//
// MySQL查询结果集实现
//
// *-----------------------------------------------------------------

#pragma once

#include "GuildWarFwd.h"
#include "Database/QueryResult.h"

class QueryResultMysql : public QueryResult
{
public:
    QueryResultMysql(MYSQL_RES* result, u_int64 rowCount, u_int fieldCount);
    ~QueryResultMysql();

    bool nextRow();

private:
    enum Field::DataTypes ConvertNativeType(enum_field_types mysqlType) const;
    void EndQuery();

    MYSQL_RES *mResult;
};
