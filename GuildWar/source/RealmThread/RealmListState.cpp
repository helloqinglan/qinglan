
// *-------------------------------------------------------
//
// 服务器列表状态
//
// *-------------------------------------------------------

#include "GuildWarPch.h"
#include "RealmListState.h"
#include "RealmPacket.h"
#include "RealmThread/RealmThread.h"

// 该状态下处理的消息码
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
		assert(0 && "RealmListState::process unkown cmd.");
		return false;
	}

	return true;
}

bool RealmListState::requestRealmList()
{
	if (dataBuffer()->dataSize() < 5)
		return true;

	ByteBuffer pkt;
	pkt << (u_int) 0;
	pkt << (u_char) 1;

	// ***TODO*** 服务器名和世界服地址改为从数据库中读取
	pkt << (u_int) 1;
	pkt << (u_char) 0;
	pkt << (const char *)"WoWServer";
	char server_addr[32];
	sprintf( server_addr, "%s:%d", "127.0.0.1", 8025 );
	pkt << server_addr;
	pkt << (float) 1.6;
	pkt << (u_char) 0;
	pkt << (u_char) 1;
	pkt << (u_char) 0;

	pkt << (u_char) 0;                             
	pkt << (u_char) 0x2;                           

	ByteBuffer hdr;
	hdr << (u_char) AUTH_REALM_LIST;
	hdr << (u_short)pkt.size();
	hdr.append(pkt);

	return sendData((const char *)hdr.contents(), hdr.size());
}
