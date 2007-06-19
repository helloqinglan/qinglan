
// *-----------------------------------------------------------------
//
// 世界服务处理器
//
// *-----------------------------------------------------------------

#pragma once

#include "Network/SocketService.h"
#include "WorldThread/WorldPacket.h"
#include "WorldThread/CryptStrategy.h"

class WorldSocket : public SocketService
{
public:
	WorldSocket(ACE_SOCK_Stream* peer);
	~WorldSocket();

	// 连接建立
	void accepted();

	// 连接关闭
	void closed();

	// 接收数据 (需要重写以增加对解密的支持)
	bool recvData(char* data, size_t len);

	// 发送网络包 (不能再直接使用原来的sendData方法)
	bool sendData(const WorldPacket& packet);

	// 设置加密密钥
	void setCryptKey(u_char* data, size_t len);

private:
	bool processPacket();

private:
	CryptStrategy m_authCrypt;			// 加解密策略

	u_short m_recvRemain;				// 接收数据还剩余的不需加密字节数
	ByteBuffer m_tmpBuffer;				// 临时接收缓冲区
										// 用于临时保存接收到的数据, 当接收到一个完整包后再递交到m_dataBuffer
};
