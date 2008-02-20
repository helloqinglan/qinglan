
// *-----------------------------------------------------------------
//
// 实体属性更新包构造
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "WorldThread/WorldPacket.h"
#include "Entity/UpdateData.h"
#include "zlib.h"


// 服务器发送的消息码
#define SMSG_UPDATE_OBJECT				169		// 对象数据更新
#define SMSG_COMPRESSED_UPDATE_OBJECT	502		// 对象数据更新压缩包


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

	// 大小超过50字节的数据包要压缩
	if (m_data.size() > 50)
	{
		u_int destSize = (u_int)buf.size() + buf.size() / 10 + 16;
		packet.resize(destSize);

		packet.put(0, (u_int)buf.size());

		compress(const_cast<u_char*>(packet.contents()) + sizeof(u_int), &destSize,
			(void*)buf.contents(), (u_int)buf.size());
		if (destSize == 0)
			return false;

		packet.resize(destSize + sizeof(u_int));
		packet.setOpcode(SMSG_COMPRESSED_UPDATE_OBJECT);
	}
	else
	{
		packet.append(buf);
		packet.setOpcode(SMSG_UPDATE_OBJECT);
	}

	return true;
}

void UpdateData::compress(void* dst, u_int* dstSize, void* src, u_int srcSize)
{
	z_stream c_stream;
	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	int z_res = deflateInit(&c_stream, Z_BEST_SPEED);
	if (z_res != Z_OK)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("UpdateData::compress (zlib: deflateInit): %i (%s).\n"), z_res, zError(z_res)));
		*dstSize = 0;
		return;
	}

	c_stream.next_out = (Bytef*)dst;
	c_stream.avail_out = *dstSize;
	c_stream.next_in = (Bytef*)src;
	c_stream.avail_in = (uInt)srcSize;

	z_res = deflate(&c_stream, Z_NO_FLUSH);
	if (z_res != Z_OK)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("UpdateData::compress (zlib: deflate): %i (%s).\n"), z_res, zError(z_res)));
		*dstSize = 0;
		return;
	}

	if (c_stream.avail_in != 0)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("UpdateData::compress (zlib: deflate) 未压缩完成.\n")));
		*dstSize = 0;
		return;
	}

	z_res = deflate(&c_stream, Z_FINISH);
	if (z_res != Z_STREAM_END)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("UpdateData::compress (zlib: deflate) 结果错误: %i (%s).\n"), z_res, zError(z_res)));
		*dstSize = 0;
		return;
	}

	z_res = deflateEnd(&c_stream);
	if (z_res != Z_OK)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("UpdateData::compress (zlib: deflateEnd): %i (%s).\n"), z_res, zError(z_res)));
		*dstSize = 0;
		return;
	}

	*dstSize = c_stream.total_out;
}
