
// *-----------------------------------------------------------------
//
// �����������
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "WorldThread/WorldSocket.h"
#include "Network/SocketState.h"


// *-----------------------------------------------------------------
//
// ***TODO*** ���������Ƿ�Ӧ�÷ŵ�һ�������Ķ����ļ���?
// ���ݰ���������
#define PACKET_CRYPT_SEED 0xDEADBABE

// ��������Ϣ Ҫ��ͻ�����֤
#define SMSG_AUTH_CHALLENGE 492


namespace
{
	// �ͻ�����Ϣ��ͷ
	#pragma pack(push, 1)
	struct ClientPktHeader
	{
		u_short size;
		u_int cmd;
	};

	// ��������Ϣ��ͷ
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


// ���ӽ���
void WorldSocket::accepted()
{
	// ��������
	WorldPacket packet;
	packet.initialize(SMSG_AUTH_CHALLENGE);
	packet << (u_int)PACKET_CRYPT_SEED;
	sendData(packet);
}

// ���ӹر�
void WorldSocket::closed()
{
}


// *-----------------------------------------------------------------
// �����շ�
bool WorldSocket::recvData(char* data, size_t len)
{
	// ***TODO*** ������һ�����ݿ���, �Ƿ��ܱ���?
	m_tmpBuffer.append(data, len);

	while (m_tmpBuffer.length())
	{
		if (!m_recvRemain)
		{
			// �������ܵĳ���
			if (m_tmpBuffer.length() < CryptStrategy::CRYPTED_RECV_LEN)
				break;

			// ����6�ֽ�
			ClientPktHeader hdrData;
			m_tmpBuffer.read((u_char*)&hdrData, CryptStrategy::CRYPTED_RECV_LEN);
			m_authCrypt.decryptRecv((u_char*)&hdrData, CryptStrategy::CRYPTED_RECV_LEN);
			m_recvRemain = hdrData.size = ntohs(hdrData.size) - 4;	// ԭʼ��ͷ�м�¼�ĳ��Ȱ�����ͷ�ṹ��cmd�ĳ���

			// ��ͷ�ṹд�뻺����
			dataBuffer()->write((char*)&hdrData, CryptStrategy::CRYPTED_RECV_LEN);
		}
		else
		{
			// ��ȡ���µ����� ����Ҫ����
			u_short minLen = std::min(m_recvRemain, (u_short)m_tmpBuffer.length());

			// ***TODO*** ��������һ�����ݿ��� �Ƿ�ɱ���?
			u_char* tmpData = new u_char[minLen];
			m_tmpBuffer.read(tmpData, minLen);
			dataBuffer()->write((char*)tmpData, minLen);

			m_recvRemain -= minLen;
		}
	}

	// ����һ�»����� ����û�������һֱ����
	if (!m_tmpBuffer.size())
		m_tmpBuffer.clear();

	return processPacket();
}

bool WorldSocket::sendData(const WorldPacket& packet)
{
	// ��������ʱÿ�ε��ö���һ�����������ݰ�
	// �����а����ճ�������

	ServerPktHeader hdr;
	hdr.size = ntohs((u_short)packet.size() + 2);	// size������ͷ�ṹ��cmd�ĳ���
	hdr.cmd = packet.getOpcode();

	// ���ܰ�ͷ
	m_authCrypt.encryptSend((u_char*)&hdr, CryptStrategy::CRYPTED_SEND_LEN);

	// ���Ͱ�ͷ�Ͱ��嵽�ͻ���
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

	// ����ͷ
	ClientPktHeader hdrData;
	if (!dataBuffer()->softRead((char*)&hdrData, sizeof(ClientPktHeader)))
		return true;

	// ��������size���Ȳ�������ͷ�е�����
	u_short dataLength = hdrData.size;
	if (dataBuffer()->dataSize() < dataLength + sizeof(ClientPktHeader))
		return true;

	// ��ȡ��Ϣ����
	dataBuffer()->remove(sizeof(ClientPktHeader));
	WorldPacket packet((u_short)hdrData.cmd, (size_t)dataLength);
	packet.resize(dataLength);

	if (dataLength)
		dataBuffer()->read((char*)packet.contents(), dataLength);

	return currentState()->process(&packet);
}
