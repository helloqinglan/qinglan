
// *-----------------------------------------------------------------
//
// 实体属性更新包构造
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "WorldThread/WorldPacket.h"
#include "Entity/UpdateData.h"


// 服务器发送的消息码
#define SMSG_UPDATE_OBJECT				169		// 对象数据更新


UpdateData::UpdateData()
: m_blockCount(0)
{
}

void UpdateData::addGUID(const u_int64& id)
{
	m_guidList.insert(id);
}

void UpdateData::addGUID(const IDList& id)
{
	m_guidList.insert(id.begin(), id.end());
}

void UpdateData::addUpdateBlock(const ByteBuffer& block)
{
	m_data.append(block);
	++m_blockCount;
}

void UpdateData::clear()
{
	m_data.clear();
	m_guidList.clear();
	m_blockCount = 0;
}

bool UpdateData::buildPacket(WorldPacket& packet, bool hasTransport)
{
	ByteBuffer buf(m_data.size() + 10 + m_guidList.size() * 8);

	buf << (u_int)(!m_guidList.empty() ? m_blockCount + 1 : m_blockCount);
	buf << (u_char)(hasTransport ? 1 : 0);

	if (!m_guidList.empty())
	{
		buf << (u_char)UPDATETYPE_OUT_OF_RANGE_OBJECTS;
		buf << (u_int)m_guidList.size();

		for (IDList::const_iterator itr = m_guidList.begin(); itr != m_guidList.end(); ++itr)
		{
			buf << (u_char)0xFF;
			buf << (u_int64)*itr;
		}
	}

	buf.append(m_data);

	packet.clear();

	// ***TODO*** 大小超过50字节的数据包要压缩, 不压缩会不会有问题
	packet.append(buf);
	packet.setOpcode(SMSG_UPDATE_OBJECT);

	return true;
}
