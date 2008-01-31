
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
#include "zlib.h"


// *-----------------------------------------------------------------
//
// ***TODO*** 这些宏是否应该放到一个独立的定义文件中?
// 数据包加密种子
#define PACKET_CRYPT_SEED 0xDEADBABE

// 该状态下处理的客户端消息码
#define CMSG_PING				476		// Ping包
#define CMSG_AUTH_SESSION		493		// 验证请求

// 该状态下服务器会发送的消息码
#define SMSG_PONG				477		// Ping返回包
#define SMSG_AUTH_RESPONSE		494		// 认证响应
#define SMSG_ADDON_INFO			751		// 启用客户端Addons

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
		ACE_ERROR ((GAME_ERROR ACE_TEXT("SessionState::process 未知的消息码: %d.\n"), packet->getOpcode()));
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

	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("SessionState::logonCheck 帐号 %s 请求进入游戏世界.\n"), account.c_str()));

	// 客户端种子
	u_int clientSeed;
	packet >> clientSeed;

	// 客户端密钥
	u_char digest[20];
	packet.read(digest, 20);

	// 检查是否同一个用户
	if (REALM_THREAD->accountName() != account)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("SessionState::logonCheck 请求进入游戏世界的帐号 %s 与登录验证的帐号不一致.\n"), account.c_str()));

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
		ACE_ERROR ((GAME_ERROR ACE_TEXT("SessionState::logonCheck 帐号 %s 的v值错误, 不能进入游戏世界.\n"), account.c_str()));

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
		ACE_ERROR ((GAME_ERROR ACE_TEXT("SessionState::logonCheck 帐号 %s 的验证信息错误, 不能进入游戏世界.\n"), account.c_str()));

		WorldPacket pkt(SMSG_AUTH_RESPONSE, 1);
		pkt << (u_char)AUTH_FAILED;
		return sendData(pkt);
	}

	// 设置 加密密钥
	dynamic_cast<WorldSocket*>(m_socketService)->setCryptKey(K.AsByteArray(), 40);

	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("SessionState::logonCheck 帐号 %s 验证成功, 进入游戏世界.\n"), account.c_str()));

	// 认证成功
	WorldPacket pkt(SMSG_AUTH_RESPONSE, 11);
	pkt << (u_char)AUTH_OK;
	pkt << (u_int)0;		// 未知的随机数据
	pkt << (u_char)2;
	pkt << (u_int)0;
	pkt << (u_char)1;		// 是否开启TBC
	sendData(pkt);

	sendAddonPacket(packet);
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

void SessionState::sendAddonPacket(WorldPacket& packet)
{
	const static u_char tdata[257] =
	{
		0x01, 0xC3, 0x5B, 0x50, 0x84, 0xB9, 0x3E, 0x32, 0x42, 0x8C, 0xD0, 0xC7, 0x48, 0xFA, 0x0E, 0x5D,
		0x54, 0x5A, 0xA3, 0x0E, 0x14, 0xBA, 0x9E, 0x0D, 0xB9, 0x5D, 0x8B, 0xEE, 0xB6, 0x84, 0x93, 0x45,
		0x75, 0xFF, 0x31, 0xFE, 0x2F, 0x64, 0x3F, 0x3D, 0x6D, 0x07, 0xD9, 0x44, 0x9B, 0x40, 0x85, 0x59,
		0x34, 0x4E, 0x10, 0xE1, 0xE7, 0x43, 0x69, 0xEF, 0x7C, 0x16, 0xFC, 0xB4, 0xED, 0x1B, 0x95, 0x28,
		0xA8, 0x23, 0x76, 0x51, 0x31, 0x57, 0x30, 0x2B, 0x79, 0x08, 0x50, 0x10, 0x1C, 0x4A, 0x1A, 0x2C,
		0xC8, 0x8B, 0x8F, 0x05, 0x2D, 0x22, 0x3D, 0xDB, 0x5A, 0x24, 0x7A, 0x0F, 0x13, 0x50, 0x37, 0x8F,
		0x5A, 0xCC, 0x9E, 0x04, 0x44, 0x0E, 0x87, 0x01, 0xD4, 0xA3, 0x15, 0x94, 0x16, 0x34, 0xC6, 0xC2,
		0xC3, 0xFB, 0x49, 0xFE, 0xE1, 0xF9, 0xDA, 0x8C, 0x50, 0x3C, 0xBE, 0x2C, 0xBB, 0x57, 0xED, 0x46,
		0xB9, 0xAD, 0x8B, 0xC6, 0xDF, 0x0E, 0xD6, 0x0F, 0xBE, 0x80, 0xB3, 0x8B, 0x1E, 0x77, 0xCF, 0xAD,
		0x22, 0xCF, 0xB7, 0x4B, 0xCF, 0xFB, 0xF0, 0x6B, 0x11, 0x45, 0x2D, 0x7A, 0x81, 0x18, 0xF2, 0x92,
		0x7E, 0x98, 0x56, 0x5D, 0x5E, 0x69, 0x72, 0x0A, 0x0D, 0x03, 0x0A, 0x85, 0xA2, 0x85, 0x9C, 0xCB,
		0xFB, 0x56, 0x6E, 0x8F, 0x44, 0xBB, 0x8F, 0x02, 0x22, 0x68, 0x63, 0x97, 0xBC, 0x85, 0xBA, 0xA8,
		0xF7, 0xB5, 0x40, 0x68, 0x3C, 0x77, 0x86, 0x6F, 0x4B, 0xD7, 0x88, 0xCA, 0x8A, 0xD7, 0xCE, 0x36,
		0xF0, 0x45, 0x6E, 0xD5, 0x64, 0x79, 0x0F, 0x17, 0xFC, 0x64, 0xDD, 0x10, 0x6F, 0xF3, 0xF5, 0xE0,
		0xA6, 0xC3, 0xFB, 0x1B, 0x8C, 0x29, 0xEF, 0x8E, 0xE5, 0x34, 0xCB, 0xD1, 0x2A, 0xCE, 0x79, 0xC3,
		0x9A, 0x0D, 0x36, 0xEA, 0x01, 0xE0, 0xAA, 0x91, 0x20, 0x54, 0xF0, 0x72, 0xD8, 0x1E, 0xC7, 0x89,
		0xD2
	};

	u_int tmpVal;
	packet >> tmpVal;

	ByteBuffer addonPacked;
	addonPacked.resize(tmpVal);

	u_int currentPos = (u_int)packet.rpos();

	uLongf addonRealSize = tmpVal;
	int retVal = uncompress(const_cast<u_char*>(addonPacked.contents()), &addonRealSize,
		const_cast<u_char*>(packet.contents() + currentPos), (u_int)packet.size() - currentPos);
	if (retVal == Z_OK)
	{
		WorldPacket pkt;
		pkt.initialize(SMSG_ADDON_INFO);

		u_int i = 0;
		while (i != addonPacked.size())
		{
			std::string addonName;
			u_char unk6;
			u_int crc, unk7;

			addonPacked >> addonName >> crc >> unk7 >> unk6;

			if (crc == 0x1c776d01LL)
				pkt << (u_char)2 << (u_char)1 << (u_int)0 << (u_short)0;
			else
			{
				pkt << (u_char)2 << (u_char)1;
				pkt.append(tdata, sizeof(tdata));
				pkt << (u_int)0 << (u_char)0;
			}

			i += ((u_int)addonName.size() + 10);
		}

		sendData(pkt);

		ACE_DEBUG ((GAME_DEBUG ACE_TEXT("SessionState::sendAddonPacket 已发送AddonPacket.\n")));
	}
	else
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("SessionState::sendAddonPacket zlib解压失败, 错误码: %d.\n"), retVal));
	}
}
