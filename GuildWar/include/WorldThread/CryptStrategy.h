
// *-----------------------------------------------------------------
//
// 数据包加解密策略
//
// *-----------------------------------------------------------------

#pragma once

class CryptStrategy
{
public:
	CryptStrategy() : m_initialized(false) {}

	// 只对数据包头进行加密 
	// 服务器发送的数据包头为4字节 接收的数据包头为6字节(客户端发送)
	const static size_t CRYPTED_SEND_LEN = 4;
	const static size_t CRYPTED_RECV_LEN = 6;

	// 设置密码串
	void init(u_char* data, size_t len)
	{
		m_keyData.resize(len);
		std::copy(data, data + len, m_keyData.begin());

		m_send_i = m_send_j = m_recv_i = m_recv_j = 0;
		m_initialized = true;
	}

	// 解密收到的数据包
	void decryptRecv(u_char* data, size_t len)
	{
		if (!m_initialized || len < CRYPTED_RECV_LEN)
			return;

		for (size_t i = 0; i < CRYPTED_RECV_LEN; ++i)
		{
			m_recv_i %= m_keyData.size();
			u_char x = (data[i] - m_recv_j) ^ m_keyData[m_recv_i];
			++m_recv_i;
			m_recv_j = data[i];
			data[i] = x;
		}
	}

	// 加密要发送的数据包
	void encryptSend(u_char* data, size_t len)
	{
		if (!m_initialized || len < CRYPTED_SEND_LEN)
			return;

		for (size_t i = 0; i < CRYPTED_SEND_LEN; ++i)
		{
			m_send_i %= m_keyData.size();
			u_char x = (data[i] ^ m_keyData[m_send_i]) + m_send_j;
			++m_send_i;
			data[i] = m_send_j = x;
		}
	}

private:
	typedef std::vector<u_char> KeyData;
	KeyData m_keyData;

	u_char m_send_i, m_send_j, m_recv_i, m_recv_j;
	bool m_initialized;
};
