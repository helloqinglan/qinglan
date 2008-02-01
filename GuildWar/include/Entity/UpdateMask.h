
// *-----------------------------------------------------------------
//
// 实体属性集更新的掩码
//
// 实现原理:
//     使用bit位来保存更新过的Field位, index从0开始
//     一个u_char有8个bit, 可保存8个数据的更新状态
//     其中的(index >> 3)表示除以8取整以得到对应的u_char
//     (1 << (index & 0x7))表示除以8取余以得到在这个u_char上的哪个位
//
// *-----------------------------------------------------------------

#pragma once

class UpdateMask
{
public:
	UpdateMask()
		: m_count(0), m_blocks(0), m_maskData(0)
	{
	}

	UpdateMask(const UpdateMask& rhs)
		: m_maskData(0)
	{
		*this = rhs;
	}

	~UpdateMask()
	{
		delete m_maskData;
	}

	UpdateMask& operator = (const UpdateMask& rhs)
	{
		count(rhs.m_count);

		memcpy(m_maskData, rhs.m_maskData, m_blocks << 2);

		return *this;
	}

	void operator &= (const UpdateMask& rhs)
	{
		assert(rhs.m_count <= m_count);

		u_int minCount = std::min(rhs.m_blocks, m_blocks);
		for (u_int i = 0; i < minCount; ++i)
			m_maskData[i] &= rhs.m_maskData[i];
	}

	void operator |= (const UpdateMask& rhs)
	{
		assert(rhs.m_count <= m_count);

		u_int minCount = std::min(rhs.m_blocks, m_blocks);
		for (u_int i = 0; i < minCount; ++i)
			m_maskData[i] |= rhs.m_maskData[i];
	}

	UpdateMask operator & (const UpdateMask& rhs) const
	{
		assert(rhs.m_count <= m_count);

		UpdateMask newMask;
		newMask = *this;
		newMask &= rhs;

		return newMask;
	}

	UpdateMask operator | (const UpdateMask& rhs) const
	{
		assert(rhs.m_count <= m_count);

		UpdateMask newMask;
		newMask = *this;
		newMask |= rhs;

		return newMask;
	}

	// 获取数据项个数
	u_int count() const { return m_count; }

	// 获取mask数据长度, 字节为单位
	u_int length() const { return m_blocks << 2; }

	// 获取u_int数据项个数
	u_int blocks() const { return m_blocks; }

	// 获取保存mask的数据
	u_char* maskData() const { return (u_char*)m_maskData; }


	// 设置数据项个数
	void count(u_int count)
	{
		if (m_maskData)
			delete m_maskData;

		m_count = count;
		m_blocks = (count + 31) / 32;			// 一个int可保存32个数据项

		m_maskData = new u_int[m_blocks];
		memset(m_maskData, 0, m_blocks << 2);	// <<2 表示 *4, 即sizeof(u_int)
	}

	void clear()
	{
		if (m_maskData)
			memset(m_maskData, 0, m_blocks << 2);
	}

	void setBit(u_int index)
	{
		((u_char*)m_maskData)[index >> 3] |= (1 << (index & 0x7));
	}

	void unsetBit(u_int index)
	{
		((u_char*)m_maskData)[index >> 3] &= (0xFF ^ (1 << (index & 0x7)));
	}

	bool getBit(u_int index)
	{
		return (((u_char*)m_maskData)[index >> 3] & (1 << (index & 0x7)) != 0);
	}

private:
	u_int m_count;			// 数据项个数
	u_int m_blocks;			// u_int个数
	u_int* m_maskData;
}
