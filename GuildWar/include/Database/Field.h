
// *-----------------------------------------------------------------
//
// 数据库数据列表示接口
//
// *-----------------------------------------------------------------

#pragma once

class Field
{
public:
    enum DataTypes
    {
        DB_TYPE_UNKNOWN = 0x00,
        DB_TYPE_STRING  = 0x01,
        DB_TYPE_INTEGER = 0x02,
        DB_TYPE_FLOAT   = 0x03
    };

    Field();
    Field(Field &f);
    Field(const char *value, const char *name, enum DataTypes type);

    ~Field();

    const char* getName() const { return m_filedName; }
    enum DataTypes getType() const { return m_filedType; }

    const char* getString() const { return m_filedValue; }
    float getFloat() const { return m_filedValue ? static_cast<float>(atof(m_filedValue)) : 0; }
    bool getBool() const { return m_filedValue ? atoi(m_filedValue) > 0 : false; }
    int getInt() const { return m_filedValue ? static_cast<int>(atol(m_filedValue)) : 0; }
    unsigned char getUChar() const { return m_filedValue ? static_cast<unsigned char>(atol(m_filedValue)) : 0; }
    unsigned short getUShort() const { return m_filedValue ? static_cast<unsigned short>(atol(m_filedValue)) : 0; }
    u_int getUInt() const { return m_filedValue ? static_cast<u_int>(atol(m_filedValue)) : 0; }
    u_int64 getUInt64() const
    {
        if(m_filedValue)
        {
            u_int64 value;
            sscanf_s(m_filedValue, "%I64u", &value);
            return value;
        }
        else
            return 0;
    }

    void getName(const char* name);
    void getType(enum DataTypes type) { m_filedType = type; }

    void setValue(const char *value);

private:
    char* m_filedValue;
    char* m_filedName;
    enum DataTypes m_filedType;
};
