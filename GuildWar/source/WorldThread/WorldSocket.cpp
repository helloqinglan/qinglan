
// *-----------------------------------------------------------------
//
// 世界服务处理器
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "WorldThread/WorldSocket.h"
#include "Network/SocketState.h"


// *-----------------------------------------------------------------
//
// ***TODO*** 这两个宏是否应该放到一个独立的定义文件中?
// 数据包加密种子
#define PACKET_CRYPT_SEED 0xDEADBABE

// 服务器消息 要求客户端验证
#define SMSG_AUTH_CHALLENGE 492


namespace
{
	// 客户端消息包头
	#pragma pack(push, 1)
	struct ClientPktHeader
	{
		u_short size;
		u_int cmd;
	};

	// 服务器消息包头
	struct ServerPktHeader
	{
		u_short size;
		u_short cmd;
	};
	#pragma pack(pop)
}
//
// *-----------------------------------------------------------------


WorldSocket::WorldSocket(ACE_SOCK_Stream* peer)
: SocketService(peer), m_recvRemain(0)
{
}

WorldSocket::~WorldSocket()
{
}


// 连接建立
void WorldSocket::accepted()
{
	// 发送种子
	WorldPacket packet;
	packet.initialize(SMSG_AUTH_CHALLENGE);
	packet << (u_int)PACKET_CRYPT_SEED;
	sendData(packet);
}

// 连接关闭
void WorldSocket::closed()
{
}


// *-----------------------------------------------------------------
// 数据收发
bool WorldSocket::recvData(char* data, size_t len)
{
	// ***TODO*** 这里有一次数据拷贝, 是否能避免?
	m_tmpBuffer.append(data, len);

	while (m_tmpBuffer.length())
	{
		if (!m_recvRemain)
		{
			// 不够解密的长度
			if (m_tmpBuffer.length() < CryptStrategy::CRYPTED_RECV_LEN)
				break;

			// 解密6字节
			ClientPktHeader hdrData;
			m_tmpBuffer.read((u_char*)&hdrData, CryptStrategy::CRYPTED_RECV_LEN);
			m_authCrypt.decryptRecv((u_char*)&hdrData, CryptStrategy::CRYPTED_RECV_LEN);
			m_recvRemain = hdrData.size = ntohs(hdrData.size) - 4;	// 原始包头中记录的长度包括了头结构中cmd的长度

			// 将头结构写入缓冲区
			dataBuffer()->write((char*)&hdrData, CryptStrategy::CRYPTED_RECV_LEN);
		}
		else
		{
			// 读取余下的数据 不需要解密
			u_short minLen = std::min(m_recvRemain, (u_short)m_tmpBuffer.length());

			// ***TODO*** 这里又有一次数据拷贝 是否可避免?
			u_char* tmpData = new u_char[minLen];
			m_tmpBuffer.read(tmpData, minLen);
			dataBuffer()->write((char*)tmpData, minLen);

			m_recvRemain -= minLen;
		}
	}

	// 重置一下缓冲区 否则该缓冲区会一直增长
	if (!m_tmpBuffer.size())
		m_tmpBuffer.clear();

	return processPacket();
}

bool WorldSocket::sendData(const WorldPacket& packet)
{
	// 发送数据时每次调用都是一个完整的数据包
	// 不会有半包和粘包的情况

	ServerPktHeader hdr;
	hdr.size = ntohs((u_short)packet.size() + 2);	// size包括了头结构中cmd的长度
	hdr.cmd = packet.getOpcode();

	// 加密包头
	m_authCrypt.encryptSend((u_char*)&hdr, CryptStrategy::CRYPTED_SEND_LEN);

	// 发送包头和包体到客户端
	SocketService::sendData((char*)&hdr, CryptStrategy::CRYPTED_SEND_LEN);
	if(packet.size()) 
		SocketService::sendData((char*)packet.contents(), packet.size());

	return true;
}

void WorldSocket::setCryptKey(u_char* data, size_t len)
{
	m_authCrypt.init(data, len);
}


bool WorldSocket::processPacket()
{
	if (dataBuffer()->dataSize() < sizeof(ClientPktHeader))
		return true;

	// 读包头
	ClientPktHeader hdrData;
	if (!dataBuffer()->softRead((char*)&hdrData, sizeof(ClientPktHeader)))
		return true;

	// 修正过的size长度不包括包头中的数据
	u_short dataLength = hdrData.size;
	if (dataBuffer()->dataSize() < dataLength + sizeof(ClientPktHeader))
		return true;

	// 读取消息数据
	dataBuffer()->remove(sizeof(ClientPktHeader));
	WorldPacket packet((u_short)hdrData.cmd, (size_t)dataLength);
	packet.resize(dataLength);

	if (dataLength)
		dataBuffer()->read((char*)packet.contents(), dataLength);

	return currentState()->process(&packet);
}
