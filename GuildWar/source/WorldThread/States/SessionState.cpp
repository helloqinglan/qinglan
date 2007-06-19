
// *-----------------------------------------------------------------
//
// 会话状态, 验证session key的有效性, 登录处理
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "SessionState.h"
#include "CharacterState.h"
#include "RealmThread/RealmThread.h"
#include "WorldThread/WorldSocket.h"


// *-----------------------------------------------------------------
//
// ***TODO*** 这些宏是否应该放到一个独立的定义文件中?
// 数据包加密种子
#define PACKET_CRYPT_SEED 0xDEADBABE

// 该状态下处理的客户端消息码
#define CMSG_PING				476		// Ping包
#define CMSG_AUTH_SESSION		493		// 验证请求
#define SMSG_AUTH_RESPONSE		494		// 认证响应

// 该状态下服务器会发送的消息码
#define SMSG_PONG				477		// Ping返回包

// 错误码
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
	// ***TODO*** 丑陋的强制类型转换, 改成使用boost::any
	WorldPacket* packet = (WorldPacket*)arg;

	// ***TODO*** 在SocketState基类中提供对消息注册事件处理的方法
	//            去掉这些冗长的if...else
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

	// 正常情况下, 两次ping之间的间隔为30秒
	// 可以此来判断客户端有没有用加速外挂

	WorldPacket pkt(SMSG_PONG, 4);
	pkt << ping;
	return sendData(pkt);
}

bool SessionState::logonCheck(WorldPacket& packet)
{
	u_int tmp;
	packet >> tmp >> tmp;

	// 帐号
	std::string account;
	packet >> account;

	// 客户端种子
	u_int clientSeed;
	packet >> clientSeed;

	// 客户端密钥
	u_char digest[20];
	packet.read(digest, 20);

	// 检查是否同一个用户
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

	// 检查根据s计算出来的v与客户端发上来的是否一致
	// ***TODO*** 为BigNumber重载一下 == 操作符, 以支持两个BigNumber的直接比较
	BigNumber serverS = REALM_THREAD->sessionv();
	if (std::string(v.AsHexStr()) != std::string(serverS.AsHexStr()))
	{
		WorldPacket pkt(SMSG_AUTH_RESPONSE, 1);
		pkt << (u_char) AUTH_UNKNOWN_ACCOUNT;
		return sendData(pkt);
	}

	// 比较客户端的key是否正确
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

	// 设置 加密密钥
	dynamic_cast<WorldSocket*>(m_socketService)->setCryptKey(K.AsByteArray(), 40);

	// 认证成功
	WorldPacket pkt(SMSG_AUTH_RESPONSE, 1);
	pkt << (u_char)AUTH_OK;
	sendData(pkt);
/*
	// 派发 "帐号验证成功" 事件
	ByteBuffer eventArg;
	eventArg << accountid;
	S_EventServer->emit(EVENT_AUTH_SUCCESS, eventArg);
*/
	// 转到角色管理状态
	return transition(new CharacterState(m_socketService));
}

bool SessionState::sendData(WorldPacket& packet)
{
	return dynamic_cast<WorldSocket*>(m_socketService)->sendData(packet);
}
