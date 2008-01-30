
// *-------------------------------------------------------
//
// �������б�״̬
//
// *-------------------------------------------------------

#include "GuildWarPch.h"
#include "RealmListState.h"
#include "RealmPacket.h"
#include "RealmThread/RealmThread.h"

// ��״̬�´������Ϣ��
#define AUTH_REALM_LIST			0x10

RealmListState::RealmListState(SocketService* sm)
: SocketState(sm)
{
}

RealmListState::~RealmListState()
{
}

bool RealmListState::process(void* arg)
{
	if (!dataBuffer()->dataSize())
		return true;

	u_char cmd;
	dataBuffer()->softRead((char*)&cmd, 1);

	if (cmd == AUTH_REALM_LIST)
		return requestRealmList();
	else
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("RealmListState::process δ֪����Ϣ��: %d.\n"), cmd));
		return false;
	}

	return true;
}

bool RealmListState::requestRealmList()
{
	if (dataBuffer()->dataSize() < 5)
	{
		ACE_DEBUG ((GAME_DEBUG ACE_TEXT("RealmListState::requestRealmList ���ݳ��Ȳ���, �����ȴ�.\n")));
		return true;
	}

	ByteBuffer pkt;
	pkt << (u_int)0;					// Unkown
	pkt << (u_short)1;					// ����������

	// ***TODO*** �����������������ַ��Ϊ�����ݿ��ж�ȡ
	pkt << (u_char)1;					// Icon
	pkt << (u_char)0;					// Lock
	pkt << (u_char)0;					// Color
	pkt << (const char *)"WoWServer";	// Name
	char server_addr[32];
	sprintf_s( server_addr, "%s:%d", "127.0.0.1", 8025 );
	pkt << server_addr;					// ��������ַ
	pkt << (float)1.6;					// Population Level
	pkt << (u_char)0;					// �ʺ��ڸ÷������ϵĽ�ɫ����
	pkt << (u_char)1;					// Time Zone
	pkt << (u_char)0x2C;				// Unkown

	pkt << (u_char)0x10;				// Unkown
	pkt << (u_char)0x00;				// Unkown

	ByteBuffer hdr;
	hdr << (u_char) AUTH_REALM_LIST;
	hdr << (u_short)pkt.size();
	hdr.append(pkt);

	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("RealmListState::requestRealmList ������������б�.\n")));

	return sendData((const char *)hdr.contents(), hdr.size());
}
