
// *-----------------------------------------------------------------
//
// 实体属性更新包构造
//
// *-----------------------------------------------------------------

#pragma once

class UpdateData
{
public:
	typedef std::set<u_int64> IDList;

	enum OBJECT_UPDATE_TYPE
	{
		UPDATETYPE_CREATE_OBJECT		=	2,	// 创建对象
		UPDATETYPE_CREATE_OBJECT2		=	3,	// ***TODO***
		UPDATETYPE_OUT_OF_RANGE_OBJECTS =	4,	// 离开视野的对象
	};

	enum OBJECT_UPDATE_FLAGS
	{
		UPDATEFLAG_NONE			= 0x00,
		UPDATEFLAG_SELF			= 0x01,			// 更新自己
		UPDATEFLAG_TRANSPORT	= 0x02,			// ***TODO***
		UPDATEFLAG_FULLGUID		= 0x04,			// ***TODO***
		UPDATEFLAG_LOWGUID		= 0x08,			// ***TODO***
		UPDATEFLAG_HIGHGUID		= 0x10,			// ***TODO***
		UPDATEFLAG_LIVING		= 0x20,			// ***TODO***
		UPDATEFLAG_HASPOSITION	= 0x40,			// ***TODO***
	};

public:
	UpdateData();

	void addGUID(const u_int64& id);
	void addGUID(const IDList& id);

	void addUpdateBlock(const ByteBuffer& block);

	bool hasData() const { return m_blockCount || !m_guidList.empty(); }

	void clear();

	bool buildPacket(WorldPacket& packet, bool hasTransport = false);

	IDList const& getIDList() const { return m_guidList; }

private:
	u_int m_blockCount;

	IDList m_guidList;
	ByteBuffer m_data;
};
