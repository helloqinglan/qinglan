
// *-----------------------------------------------------------------
//
// 数据库数据列表示接口
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "Database/Field.h"

Field::Field()
: m_filedValue(0), m_filedName(0), m_filedType(DB_TYPE_UNKNOWN)
{
}

Field::Field(Field &f)
{
    const char *value, *name;

    value = f.getString();
    name = f.getName();

    if (value && (m_filedValue = new char[strlen(value) + 1]))
        strcpy_s(m_filedValue, strlen(value) + 1, value);
    else
        m_filedValue = 0;

    if (name && (m_filedName = new char[strlen(name) + 1]))
        strcpy_s(m_filedName, strlen(name) + 1, name);
    else
        m_filedName = 0;

    m_filedType = f.getType();
}

Field::Field(const char *value, const char *name, enum Field::DataTypes type)
: m_filedType(type)
{
    if (value && (m_filedValue = new char[strlen(value) + 1]))
        strcpy_s(m_filedValue, strlen(value) + 1, value);
    else
        m_filedValue = 0;

    if (name && (m_filedName = new char[strlen(name) + 1]))
        strcpy_s(m_filedName, strlen(name) + 1, name);
    else
        m_filedName = 0;
}

Field::~Field()
{
    delete [] m_filedValue;
    delete [] m_filedName;
}

void Field::setValue(const char *value)
{
    delete [] m_filedValue;

    if (value)
    {
        m_filedValue = new char[strlen(value) + 1];
        strcpy_s(m_filedValue, strlen(value) + 1, value);
    }
    else
        m_filedValue = 0;
}

void Field::getName(const char *name)
{
    delete [] m_filedName;

    if (name)
    {
        m_filedName = new char[strlen(name) + 1];
        strcpy_s(m_filedName, strlen(name) + 1, name);
    }
    else
        m_filedName = 0;
}
