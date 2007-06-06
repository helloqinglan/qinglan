
// *-----------------------------------------------------------------
//
// MySQL查询结果集实现
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "QueryResultMysql.h"

QueryResultMysql::QueryResultMysql(MYSQL_RES *result, u_int64 rowCount, u_int fieldCount) :
QueryResult(rowCount, fieldCount), mResult(result)
{
    mCurrentRow = new Field[mFieldCount];
    assert(mCurrentRow);

    MYSQL_FIELD *fields = mysql_fetch_fields(mResult);
    for (u_int i = 0; i < mFieldCount; i++)
    {
        mCurrentRow[i].getName(fields[i].name);
        mCurrentRow[i].getType(ConvertNativeType(fields[i].type));
    }
}

QueryResultMysql::~QueryResultMysql()
{
    EndQuery();
}

bool QueryResultMysql::nextRow()
{
    MYSQL_ROW row;

    if (!mResult)
        return false;

    row = mysql_fetch_row(mResult);
    if (!row)
    {
        EndQuery();
        return false;
    }

    for (u_int i = 0; i < mFieldCount; i++)
        mCurrentRow[i].setValue(row[i]);

    return true;
}

void QueryResultMysql::EndQuery()
{
    if (mCurrentRow)
    {
        delete [] mCurrentRow;
        mCurrentRow = 0;
    }

    if (mResult)
    {
        mysql_free_result(mResult);
        mResult = 0;
    }
}

enum Field::DataTypes QueryResultMysql::ConvertNativeType(enum_field_types mysqlType) const
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
