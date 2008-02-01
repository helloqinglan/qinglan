
// *-----------------------------------------------------------------
//
// ʵ�����Լ����µ�����
//
// ʵ��ԭ��:
//     ʹ��bitλ��������¹���Fieldλ, index��0��ʼ
//     һ��u_char��8��bit, �ɱ���8�����ݵĸ���״̬
//     ���е�(index >> 3)��ʾ����8ȡ���Եõ���Ӧ��u_char
//     (1 << (index & 0x7))��ʾ����8ȡ���Եõ������u_char�ϵ��ĸ�λ
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

	// ��ȡ���������
	u_int count() const { return m_count; }

	// ��ȡmask���ݳ���, �ֽ�Ϊ��λ
	u_int length() const { return m_blocks << 2; }

	// ��ȡu_int���������
	u_int blocks() const { return m_blocks; }

	// ��ȡ����mask������
	u_char* maskData() const { return (u_char*)m_maskData; }


	// �������������
	void count(u_int count)
	{
		if (m_maskData)
			delete m_maskData;

		m_count = count;
		m_blocks = (count + 31) / 32;			// һ��int�ɱ���32��������

		m_maskData = new u_int[m_blocks];
		memset(m_maskData, 0, m_blocks << 2);	// <<2 ��ʾ *4, ��sizeof(u_int)
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
	u_int m_count;			// ���������
	u_int m_blocks;			// u_int����
	u_int* m_maskData;
}
