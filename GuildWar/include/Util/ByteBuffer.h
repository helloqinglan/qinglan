
// *-----------------------------------------------------------------
//
// 字节缓冲区
//
// *-----------------------------------------------------------------

#pragma once

class ByteBuffer
{
public:
	const static size_t DEFAULT_SIZE = 0x1000;

	ByteBuffer()
		: m_readPos(0), m_writePos(0)
	{
		m_dataStorage.reserve(DEFAULT_SIZE);
	}

	ByteBuffer(size_t size)
		: m_readPos(0), m_writePos(0)
	{
		m_dataStorage.reserve(size);
	}

	ByteBuffer(const ByteBuffer& rh)
		: m_readPos(rh.m_readPos), m_writePos(rh.m_writePos), m_dataStorage(rh.m_dataStorage)
	{
	}

	void clear()
	{
		m_dataStorage.clear();
		m_readPos = m_writePos = 0;
	}

	template<typename T>
	void append(T value)
	{
		append((u_char*)&value, sizeof(value));
	}

	template<typename T>
	void put(size_t pos, T value)
	{
		put(pos, (u_char*)&value, sizeof(value));
	}

	ByteBuffer& operator <<(bool value)
	{
		append<char>((char)value);
		return *this;
	}

	// unsigned
	ByteBuffer& operator <<(u_char value)
	{
		append<u_char>(value);
		return *this;
	}

	ByteBuffer& operator <<(u_short value)
	{
		append<u_short>(value);
		return *this;
	}

	ByteBuffer& operator <<(u_int value)
	{
		append<u_int>(value);
		return *this;
	}

	ByteBuffer& operator <<(u_int64 value)
	{
		append<u_int64>(value);
		return *this;
	}

	// signed
	ByteBuffer& operator <<(char value)
	{
		append<char>(value);
		return *this;
	}

	ByteBuffer& operator <<(short value)
	{
		append<short>(value);
		return *this;
	}

	ByteBuffer& operator <<(int value)
	{
		append<int>(value);
		return *this;
	}

	ByteBuffer& operator <<(__int64 value)
	{
		append<__int64>(value);
		return *this;
	}

	// floating points
	ByteBuffer& operator <<(float value)
	{
		append<float>(value);
		return *this;
	}

	ByteBuffer& operator <<(double value)
	{
		append<double>(value);
		return *this;
	}

	// string
	ByteBuffer& operator <<(const std::string& value)
	{
		append((u_char*)value.c_str(), value.length());
		append((u_char)0);
		return *this;
	}

	ByteBuffer& operator <<(const char* str)
	{
		append((u_char*)str, str ? strlen(str) : 0);
		append((u_char)0);
		return *this;
	}

	ByteBuffer& operator >>(bool& value)
	{
		value = read<char>() > 0 ? true : false;
		return *this;
	}

	// unsigned
	ByteBuffer& operator >>(u_char& value)
	{
		value = read<u_char>();
		return *this;
	}

	ByteBuffer& operator >>(u_short& value)
	{
		value = read<u_short>();
		return *this;
	}

	ByteBuffer& operator >>(u_int& value)
	{
		value = read<u_int>();
		return *this;
	}

	ByteBuffer& operator >>(u_int64& value)
	{
		value = read<u_int64>();
		return *this;
	}

	// signed
	ByteBuffer& operator >>(char& value)
	{
		value = read<char>();
		return *this;
	}

	ByteBuffer& operator >>(short& value)
	{
		value = read<short>();
		return *this;
	}

	ByteBuffer& operator >>(int& value)
	{
		value = read<int>();
		return *this;
	}

	ByteBuffer& operator >>(__int64& value)
	{
		value = read<__int64>();
		return *this;
	}

	ByteBuffer& operator >>(float& value)
	{
		value = read<float>();
		return *this;
	}

	ByteBuffer& operator >>(double& value)
	{
		value = read<double>();
		return *this;
	}

	ByteBuffer& operator >>(std::string& value)
	{
		value.clear();
		while (m_readPos < size())
		{
			char c = read<char>();
			if (c == 0)
				break;
			value += c;
		}

		return *this;
	}

	u_char operator [](size_t pos)
	{
		return read<u_char>(pos);
	}

	size_t rpos() const { return m_readPos; }

	size_t rpos(size_t rpos)
	{
		m_readPos = rpos;
		return m_readPos;
	}

	size_t wpos() const { return m_writePos; }

	size_t wpos(size_t wpos)
	{
		m_writePos = wpos;
		return m_writePos;
	}

	// 按顺序读取数据 并移动读指针
	template<typename T>
	T read()
	{
		T r = read<T>(m_readPos);
		m_readPos += sizeof(T);
		return r;
	}

	// 读取指定位置的数据 不移动读指针
	template<typename T>
	T read(size_t pos) const
	{
		assert(pos + sizeof(T) <= size());
		return *((T*)&m_dataStorage[pos]);
	}

	// 按顺序读取指定长度的数据 会移动读指针
	void read(u_char* dest, size_t len)
	{
		assert(m_readPos + len <= size());
		memcpy(dest, &m_dataStorage[m_readPos], len);
		m_readPos += len;
	}

	const u_char* contents() const { return &m_dataStorage[0]; }

	// 返回数据总长度, 包括已读取的数据
	inline size_t size() const { return /*m_dataStorage.size()*/m_writePos; }

	// 返回还未读取的数据长度
	inline size_t length() const { return m_writePos - m_readPos; }

	// 重置缓冲区大小, 会清空缓冲区内的已有数据
	void resize(size_t newsize)
	{
		m_dataStorage.resize(newsize);
		m_readPos = 0;
		m_writePos = size();
	}

	void reserve(size_t ressize)
	{
		if (ressize > size())
			m_dataStorage.reserve(ressize);
	}

	void append(const std::string& str)
	{
		append((u_char*)str.c_str(), str.size() + 1);
	}

	void append(const char* str, size_t cnt)
	{
		return append((const u_char*)str, cnt);
	}

	void append(const u_char* src, size_t cnt)
	{
		if (!cnt)
			return;

		if (m_dataStorage.size() < m_writePos + cnt)
			m_dataStorage.resize(m_writePos + cnt);

		memcpy(&m_dataStorage[m_writePos], src, cnt);
		m_writePos += cnt;
	}

	void append(const ByteBuffer& buffer)
	{
		if (buffer.size())
			append(buffer.contents(), buffer.size());
	}

	// 改写指定位置的数据 不会移动读写指针
	void put(size_t pos, const u_char* src, size_t cnt)
	{
		assert(pos + cnt <= size());
		memcpy(&m_dataStorage[pos], src, cnt);
	}

protected:
	typedef std::vector<u_char> StorageType;
	StorageType m_dataStorage;

	size_t m_readPos;
	size_t m_writePos;
};


// 将vector数据放入ByteBuffer中
// 注意生成的ByteBuffer必须要用对应的 >> 方法来还原成vector
template<typename T>
ByteBuffer& operator <<(ByteBuffer& b, std::vector<T> v)
{
	b << (u_int)v.size();
	for (typename std::vector<T>::iterator i = v.begin(); i != v.end(); ++i)
		b << *i;
	return b;
}

template<typename T>
ByteBuffer& operator >>(ByteBuffer& b, std::vector<T>& v)
{
	u_int vsize;
	b >> vsize;
	v.clear();
	while (vsize--)
	{
		T t;
		b >> t;
		v.push_back(t);
	}
	return b;
}

// 将list数据放入ByteBuffer中
// 注意生成的ByteBuffer必须要用对应的 >> 方法来还原成list
template<typename T>
ByteBuffer& operator <<(ByteBuffer& b, std::list<T> v)
{
	b << (u_int)v.size();
	for (typename std::list<T>::iterator i = v.begin(); i != v.end(); ++i)
		b << *i;
	return b;
}

template<typename T>
ByteBuffer& operator >>(ByteBuffer& b, std::list<T>& v)
{
	u_int vsize;
	b >> vsize;
	v.clear();
	while (vsize--)
	{
		T t;
		b >> t;
		v.push_back(t);
	}
	return b;
}

// 将map数据放入ByteBuffer中
// 注意生成的ByteBuffer必须要用对应的 >> 方法来还原成map
template<typename K, typename V>
ByteBuffer& operator <<(ByteBuffer& b, std::map<K, V>& m)
{
	b << (u_int)m.size();
	for (typename std::map<K, V>::iterator i = m.begin(); i != m.end(); ++i)
		b << i->_Myfirst << i->second;
	return b;
}

template<typename K, typename V>
ByteBuffer& operator >>(ByteBuffer& b, std::map<K, V>& m)
{
	u_int vsize;
	b >> vsize;
	m.clear();
	while (vsize--)
	{
		K k;
		V v;
		b >> k >> v;
		m.insert(make_pair(k, v));
	}
	return b;
}
