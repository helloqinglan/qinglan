
// *-----------------------------------------------------------------
//
// 环形缓冲区
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

	// 返回总空间
	size_t totalSize() const { return m_totalSize; }

	// 返回剩余空间
	size_t space() const { return m_totalSize - m_dataSize; }

	// 返回当前已使用的字节数
	size_t dataSize() const { return m_dataSize; }

	// 写入数据
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
			// 需要绕回
			size_t rl = m_totalSize - m_writePos;		// 缓冲区写入点右边还余下的空间长度
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

	// 读取数据 读指针会移动
	bool read(char* dest, size_t len)
	{
		if (len > m_dataSize)
		{
			assert(0 && "CircularBuffer::read attempt to read beyond buffer.");
			return false;
		}

		if (m_readPos + len > m_totalSize)
		{
			// 需要绕回
			size_t rl = m_totalSize - m_readPos;		// 缓冲区读取点右边还余下的空间长度
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

	// 读取数据 读指针不会移动
	bool softRead(char* dest, size_t len)
	{
		if (len > m_dataSize)
		{
			assert(0 && "CircularBuffer::read attempt to read beyond buffer.");
			return false;
		}

		if (m_readPos + len > m_totalSize)
		{
			// 需要绕回
			size_t rl = m_totalSize - m_readPos;		// 缓冲区读取点右边还余下的空间长度
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

	// 删除数据
	bool remove(size_t len)
	{
		return read(0, len);
	}

private:
	CircularBuffer(const CircularBuffer&);
	CircularBuffer& operator=(const CircularBuffer&);

private:
	char* m_dataBuf;		// 数据缓冲区
	size_t m_totalSize;		// 空间大小
	size_t m_dataSize;		// 数据大小
	size_t m_writePos;		// 写入位置
	size_t m_readPos;		// 读取位置
};
