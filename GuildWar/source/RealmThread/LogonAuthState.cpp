
// *-------------------------------------------------------
//
// 登录验证状态
//
// *-------------------------------------------------------

#include "GuildWarPch.h"
#include "LogonAuthState.h"
#include "RealmPacket.h"
#include "RealmListState.h"
#include "RealmThread/RealmThread.h"


// 该状态下处理的消息码
#define AUTH_LOGON_REQUEST		0x00		// 登录请求
#define AUTH_LOGON_CHECK		0x01		// 验证请求

// 登录验证的错误码
enum E_AUTH_ERROR_CODE
{
	CE_SUCCESS			= 0x00,		// 成功
	CE_IPBAN			= 0x01,		// 连接失败
	CE_ACCOUNT_CLOSED	= 0x03,		// 封号
	CE_NO_ACCOUNT		= 0x04,		// 密码错误
	CE_ACCOUNT_IN_USE	= 0x06		// 帐号已登录
};


LogonAuthState::LogonAuthState(SocketService* sm)
: SocketState(sm)
{
	N.SetHexStr("894B645E89E1535BBDAD5B8B290650530801B18EBFBF5E8FAB3C82872A3E9BB7");
	g.SetDword(7);
	s.SetRand(32 * 8);
}

LogonAuthState::~LogonAuthState()
{
}

bool LogonAuthState::process(void* arg)
{
	if (!dataBuffer()->dataSize())
		return true;

	u_char cmd;
	dataBuffer()->softRead((char*)&cmd, 1);

	if (cmd == AUTH_LOGON_REQUEST)
		return logonRequest();
	else if (cmd == AUTH_LOGON_CHECK)
		return logonCheck();
	else
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("LogonAuthState::process 未知的消息码: %d.\n"), cmd));
		return false;
	}

	return true;
}

bool LogonAuthState::logonRequest()
{
	std::vector<u_char> buf;
	buf.resize(4);

	if (dataBuffer()->dataSize() < 4)
		return true;

	// 读取头四个字节 包含该数据包的长度
	dataBuffer()->softRead((char*)&buf[0], 4);
	u_short remaining = ((stLogonRequest_C*)&buf[0])->size;

	// 数据包长度不够 则等待下次再处理
	if (remaining < sizeof(stLogonRequest_C) - buf.size() ||
		dataBuffer()->dataSize() - buf.size() < remaining)
		return true;
	dataBuffer()->remove(4);

	// 读取余下的数据
	buf.resize(remaining + buf.size() + 1);
	buf[buf.size() - 1] = 0;
	dataBuffer()->read((char*)&buf[4], remaining);
	stLogonRequest_C* reqData = (stLogonRequest_C*)&buf[0];

	// 获取用户名
	// ***TODO*** 把64这个magic number用宏代替
	char login_name[64];
	memset(login_name, 0, 64);
	strncpy_s(login_name, (const char *)reqData->I, reqData->I_len);
	std::string loginName = login_name;
	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("LogonAuthState::logonRequest 用户 %s 请求登录.\n"), login_name));

	// 比较用户名是否与数据库中的用户名相同
	if (REALM_THREAD->accountName() != loginName)
	{
		ByteBuffer pkt;
		pkt << (unsigned char) AUTH_LOGON_REQUEST;
		pkt << (unsigned char) 0x00;
		pkt << (unsigned char) CE_NO_ACCOUNT;
		return sendData((const char *)pkt.contents(), pkt.size());
	}

	// 计算hash值
	Sha1Hash I;
	std::string sI = loginName + ":" + REALM_THREAD->accountPass();
	I.UpdateData(sI);
	I.Finalize();

	Sha1Hash sha;
	sha.UpdateData(s.AsByteArray(), s.GetNumBytes());
	sha.UpdateData(I.GetDigest(), 20);
	sha.Finalize();

	BigNumber x;
	x.SetBinary(sha.GetDigest(), sha.GetLength());
	v = g.ModExp(x, N);
	b.SetRand(19 * 8);
	BigNumber gmod=g.ModExp(b, N);
	B = ((v * 3) + gmod) % N;
	assert(gmod.GetNumBytes() <= 32);

	BigNumber unk3;
	unk3.SetRand( 16 * 8 );

	// respond data
	stLogonRequest_S answer_data;
	answer_data.cmd		= AUTH_LOGON_REQUEST;
	answer_data.error	= 0x00;
	answer_data.unk2	= 0x00;
	memcpy(answer_data.B, B.AsByteArray(), 0x20);
	answer_data.g_len	= 0x01;
	memcpy(answer_data.g, g.AsByteArray(), 0x01);
	answer_data.N_len	= 0x20;
	memcpy(answer_data.N, N.AsByteArray(), 0x20);
	memcpy(answer_data.s, s.AsByteArray(), 0x20);
	memcpy(answer_data.unk3, unk3.AsByteArray(), 0x10);
	answer_data.unk4	= 0x00;
	return sendData((const char*)&answer_data, sizeof(answer_data));
}

bool LogonAuthState::logonCheck()
{
	// 数据包长度不够 则等待下次再处理
	if (dataBuffer()->dataSize() < sizeof(stLogonCheck_C))
		return true;

	stLogonCheck_C checkData;
	dataBuffer()->read((char*)&checkData, sizeof(stLogonCheck_C));

	BigNumber A;
	A.SetBinary(checkData.A, 32);

	Sha1Hash sha;
	sha.UpdateBigNumbers(&A, &B, 0);
	sha.Finalize();

	BigNumber u;
	u.SetBinary(sha.GetDigest(), 20);
	BigNumber S = (A * (v.ModExp(u, N))).ModExp(b, N);

	unsigned char t[32];
	unsigned char t1[16];
	unsigned char vK[40];
	unsigned char hash[20];

	memcpy(t, S.AsByteArray(), 32);
	for (int i = 0; i < 16; i++)
		t1[i] = t[i * 2];

	sha.Initialize();
	sha.UpdateData( t1, 16 );
	sha.Finalize();

	for (int i = 0; i < 20; i++)
		vK[i*2] = sha.GetDigest()[i];
	for (int i = 0; i < 16; i++)
		t1[i] = t[i*2 + 1];

	sha.Initialize();
	sha.UpdateData( t1, 16 );
	sha.Finalize();

	for (int i = 0; i < 20; i++)
		vK[i*2+1] = sha.GetDigest()[i];
	K.SetBinary(vK, 40);

	sha.Initialize();
	sha.UpdateBigNumbers(&N, 0);
	sha.Finalize();

	memcpy(hash, sha.GetDigest(), 20);

	sha.Initialize();
	sha.UpdateBigNumbers(&g, 0);
	sha.Finalize();

	for (int i = 0; i < 20; i++)
		hash[i] ^= sha.GetDigest()[i];

	BigNumber t3;
	t3.SetBinary(hash, 20);

	sha.Initialize();
	sha.UpdateData(REALM_THREAD->accountName());
	sha.Finalize();

	BigNumber t4;
	t4.SetBinary(sha.GetDigest(), 20);

	sha.Initialize();
	sha.UpdateBigNumbers(&t3, &t4, &s, &A, &B, &K, 0);
	sha.Finalize();

	BigNumber M;
	M.SetBinary(sha.GetDigest(), 20);

	if (!memcmp(M.AsByteArray(), checkData.M1, 20))
	{
		sha.Initialize();
		sha.UpdateBigNumbers(&A, &M, &K, 0);
		sha.Finalize();

		stLogonCheck_S proof;
		memcpy(proof.M2, sha.GetDigest(), 20);
		proof.cmd = AUTH_LOGON_CHECK;
		proof.error = 0;
		proof.unk2 = 0;
		proof.unk3 = 0;

		// 保存session key数据
		REALM_THREAD->sessionK(K);
		REALM_THREAD->sessionv(v);
		REALM_THREAD->sessions(s);

		sendData((const char*)&proof, sizeof(proof));

		ACE_DEBUG ((GAME_DEBUG ACE_TEXT("LogonAuthState::logonCheck 验证成功, 进入服务器列表状态.\n")));

		// 验证成功 转换到服务器列表状态
		transition(new RealmListState(m_socketService));
	}
	else
	{
		// 验证失败
		char data[4] = {AUTH_LOGON_CHECK, CE_NO_ACCOUNT, 3, 0};
		sendData(data, sizeof(data));

		ACE_DEBUG ((GAME_DEBUG ACE_TEXT("LogonAuthState::logonCheck 验证失败.\n")));
	}

	return true;
}
