
// *-----------------------------------------------------------------
//
// ���λ�����
//
// *-----------------------------------------------------------------

#pragma once

class CircularBuffer
{
public:
	CircularBuffer(size_t count)
		: m_dataBuf(new char[count]), m_totalSize(count), m_dataSize(0), m_writePos(0), m_readPos(0)
	{
	}

	~CircularBuffer()
	{
		delete []m_dataBuf;
	}

	// �����ܿռ�
	size_t totalSize() const { return m_totalSize; }

	// ����ʣ��ռ�
	size_t space() const { return m_totalSize - m_dataSize; }

	// ���ص�ǰ��ʹ�õ��ֽ���
	size_t dataSize() const { return m_dataSize; }

	// д������
	bool write(const char* data, size_t len)
	{
		if (space() < len)
		{
			assert(0 && "CircularBuffer::write space is too short.");
			return false;
		}

		m_dataSize += len;
		if (m_writePos + len > m_totalSize)
		{
			// ��Ҫ�ƻ�
			size_t rl = m_totalSize - m_writePos;		// ������д����ұ߻����µĿռ䳤��
			memcpy(m_dataBuf + m_writePos, data, rl);
			memcpy(m_dataBuf, data + rl, len - rl);
			m_writePos = len - rl;
		}
		else
		{
			memcpy(m_dataBuf + m_writePos, data, len);
			m_writePos += len;
			assert(m_writePos <= m_totalSize && "CircularBuffer::write circular buffer overflow.");
		}

		return true;
	}

	// ��ȡ���� ��ָ����ƶ�
	bool read(char* dest, size_t len)
	{
		if (len > m_dataSize)
		{
			assert(0 && "CircularBuffer::read attempt to read beyond buffer.");
			return false;
		}

		if (m_readPos + len > m_totalSize)
		{
			// ��Ҫ�ƻ�
			size_t rl = m_totalSize - m_readPos;		// ��������ȡ���ұ߻����µĿռ䳤��
			if (dest)
			{
				memcpy(dest, m_dataBuf + m_readPos, rl);
				memcpy(dest + rl, m_dataBuf, len - rl);
			}
			m_readPos = m_totalSize - rl;
		}
		else
		{
			if (dest)
				memcpy(dest, m_dataBuf + m_readPos, len);
			m_readPos += len;
			assert(m_readPos <= m_totalSize && "CircularBuffer::read circular buffer overflow.");
		}

		m_dataSize -= len;
		if (!m_dataSize)
			m_readPos = m_writePos = 0;

		return true;
	}

	// ��ȡ���� ��ָ�벻���ƶ�
	bool softRead(char* dest, size_t len)
	{
		if (len > m_dataSize)
		{
			assert(0 && "CircularBuffer::read attempt to read beyond buffer.");
			return false;
		}

		if (m_readPos + len > m_totalSize)
		{
			// ��Ҫ�ƻ�
			size_t rl = m_totalSize - m_readPos;		// ��������ȡ���ұ߻����µĿռ䳤��
			if (dest)
			{
				memcpy(dest, m_dataBuf + m_readPos, rl);
				memcpy(dest + rl, m_dataBuf, len - rl);
			}
		}
		else
		{
			if (dest)
				memcpy(dest, m_dataBuf + m_readPos, len);
		}

		return true;
	}

	// ɾ������
	bool remove(size_t len)
	{
		return read(0, len);
	}

private:
	CircularBuffer(const CircularBuffer&);
	CircularBuffer& operator=(const CircularBuffer&);

private:
	char* m_dataBuf;		// ���ݻ�����
	size_t m_totalSize;		// �ռ��С
	size_t m_dataSize;		// ���ݴ�С
	size_t m_writePos;		// д��λ��
	size_t m_readPos;		// ��ȡλ��
};
