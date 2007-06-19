
// *-----------------------------------------------------------------
//
// �Ự״̬, ��֤session key����Ч��, ��¼����
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "SessionState.h"
#include "CharacterState.h"
#include "RealmThread/RealmThread.h"
#include "WorldThread/WorldSocket.h"


// *-----------------------------------------------------------------
//
// ***TODO*** ��Щ���Ƿ�Ӧ�÷ŵ�һ�������Ķ����ļ���?
// ���ݰ���������
#define PACKET_CRYPT_SEED 0xDEADBABE

// ��״̬�´���Ŀͻ�����Ϣ��
#define CMSG_PING				476		// Ping��
#define CMSG_AUTH_SESSION		493		// ��֤����
#define SMSG_AUTH_RESPONSE		494		// ��֤��Ӧ

// ��״̬�·������ᷢ�͵���Ϣ��
#define SMSG_PONG				477		// Ping���ذ�

// ������
enum LoginErrorCode
{
	AUTH_OK					= 0x0C,
	AUTH_FAILED				= 0x0D,
	AUTH_UNKNOWN_ACCOUNT	= 0x15,
	AUTH_ALREADY_ONLINE		= 0x1D,
};
//
// *-----------------------------------------------------------------


SessionState::SessionState(SocketService* sm) : SocketState(sm)
{
}

bool SessionState::process(void* arg)
{
	// ***TODO*** ��ª��ǿ������ת��, �ĳ�ʹ��boost::any
	WorldPacket* packet = (WorldPacket*)arg;

	// ***TODO*** ��SocketState�������ṩ����Ϣע���¼�����ķ���
	//            ȥ����Щ�߳���if...else
	if (packet->getOpcode() == CMSG_PING)
		return pingRequest(*packet);
	else if (packet->getOpcode() == CMSG_AUTH_SESSION)
		return logonCheck(*packet);
	else
	{
		assert(0 && "SessionState::process unkown cmd.");
		return false;
	}

	return true;
}

bool SessionState::pingRequest(WorldPacket& packet)
{
	u_int ping;
	packet >> ping;

	// ���������, ����ping֮��ļ��Ϊ30��
	// ���Դ����жϿͻ�����û���ü������

	WorldPacket pkt(SMSG_PONG, 4);
	pkt << ping;
	return sendData(pkt);
}

bool SessionState::logonCheck(WorldPacket& packet)
{
	u_int tmp;
	packet >> tmp >> tmp;

	// �ʺ�
	std::string account;
	packet >> account;

	// �ͻ�������
	u_int clientSeed;
	packet >> clientSeed;

	// �ͻ�����Կ
	u_char digest[20];
	packet.read(digest, 20);

	// ����Ƿ�ͬһ���û�
	if (REALM_THREAD->accountName() != account)
	{
		WorldPacket pkt(SMSG_AUTH_RESPONSE, 1);
		pkt << (u_char) AUTH_UNKNOWN_ACCOUNT;
		return sendData(pkt);
	}

	Sha1Hash I, sha1;
	BigNumber v, s, g, N, x;
	BigNumber K;

	N.SetHexStr("894B645E89E1535BBDAD5B8B290650530801B18EBFBF5E8FAB3C82872A3E9BB7");
	g.SetDword(7);

	std::string sI = account + ":" + REALM_THREAD->accountPass();
	I.UpdateData(sI);
	I.Finalize();

	s = REALM_THREAD->sessions();
	sha1.UpdateData(s.AsByteArray(), s.GetNumBytes());
	sha1.UpdateData(I.GetDigest(), 20);
	sha1.Finalize();

	x.SetBinary(sha1.GetDigest(), sha1.GetLength());
	v = g.ModExp(x, N);

	// ������s���������v��ͻ��˷��������Ƿ�һ��
	// ***TODO*** ΪBigNumber����һ�� == ������, ��֧������BigNumber��ֱ�ӱȽ�
	BigNumber serverS = REALM_THREAD->sessionv();
	if (std::string(v.AsHexStr()) != std::string(serverS.AsHexStr()))
	{
		WorldPacket pkt(SMSG_AUTH_RESPONSE, 1);
		pkt << (u_char) AUTH_UNKNOWN_ACCOUNT;
		return sendData(pkt);
	}

	// �ȽϿͻ��˵�key�Ƿ���ȷ
	Sha1Hash sha;
	u_int t = 0;
	u_int seed = PACKET_CRYPT_SEED;

	K = REALM_THREAD->sessionK();

	sha.UpdateData(account);
	sha.UpdateData((u_char*)&t, 4);
	sha.UpdateData((u_char*)&clientSeed, 4);
	sha.UpdateData((u_char*)&seed, 4);
	sha.UpdateBigNumbers(&K, NULL);
	sha.Finalize();

	if (memcmp(sha.GetDigest(), digest, 20))
	{
		WorldPacket pkt(SMSG_AUTH_RESPONSE, 1);
		pkt << (u_char)AUTH_FAILED;
		return sendData(pkt);
	}

	// ���� ������Կ
	dynamic_cast<WorldSocket*>(m_socketService)->setCryptKey(K.AsByteArray(), 40);

	// ��֤�ɹ�
	WorldPacket pkt(SMSG_AUTH_RESPONSE, 1);
	pkt << (u_char)AUTH_OK;
	sendData(pkt);
/*
	// �ɷ� "�ʺ���֤�ɹ�" �¼�
	ByteBuffer eventArg;
	eventArg << accountid;
	S_EventServer->emit(EVENT_AUTH_SUCCESS, eventArg);
*/
	// ת����ɫ����״̬
	return transition(new CharacterState(m_socketService));
}

bool SessionState::sendData(WorldPacket& packet)
{
	return dynamic_cast<WorldSocket*>(m_socketService)->sendData(packet);
}
