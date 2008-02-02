
// *-----------------------------------------------------------------
//
// 登录处理线程
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "RealmThread/RealmThread.h"
#include "WorldThread/WorldThread.h"
#include "Database/Database.h"
#include "Network/SocketService.h"
#include "LogonAuthState.h"

// 登录服监听端口
#define REALMS_LISTEN_PORT 3724

RealmThread::RealmThread() : ACE_Task_Base(), m_running(false)
{
}

RealmThread::~RealmThread()
{
	if (m_running)
		close();
}

int RealmThread::svc()
{
	ACE_INET_Addr port_to_listen(REALMS_LISTEN_PORT);
	ACE_SOCK_Acceptor acceptor;

	if (acceptor.open(port_to_listen, 1) == -1)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("RealmThread::svc 监听指定端口失败.\n")));
		return -1;
	}

	while (m_running)
	{
		// 如果已经有连接建立到WorldServer, 则不再继续接受新连接
		if (WORLD_THREAD->hasConnection())
		{
			ACE_Time_Value tv(0, 100);
			ACE_OS::sleep(tv);
			continue;
		}

		ACE_SOCK_Stream peer;
		ACE_INET_Addr peer_addr;

		if (acceptor.accept(peer, &peer_addr, 0, 0) == -1)
		{
			if (ACE_OS::last_error() == EINTR)
				ACE_DEBUG ((GAME_DEBUG
							ACE_TEXT ("RealmThread::svc Interrupted while ")
							ACE_TEXT ("waiting for connection\n")));
			else
				if (ACE_OS::last_error() == ETIMEDOUT)
					ACE_DEBUG ((GAME_DEBUG
								ACE_TEXT ("RealmThread::svc Timeout while ")
								ACE_TEXT ("waiting for connection\n")));
		}
		else
		{
			SocketService clientService(&peer);
			clientService.transition(new LogonAuthState(&clientService));

			ACE_TCHAR peer_name[MAXHOSTNAMELEN];
			peer_addr.addr_to_string(peer_name, MAXHOSTNAMELEN);
			ACE_DEBUG ((GAME_DEBUG
						ACE_TEXT ("RealmThread::svc Connection from %s\n"),
						peer_name));

			clientService.accepted();

			// ***TODO*** 去掉4096这个magic number
			char buffer[4096];

			ssize_t bytes_received;
			while (m_running)
			{
				// recv返回-1表示操作失败 返回0表示对端已关闭连接
				// ***TODO*** 这里要设置一个超时, 否则外部线程无法立即结束这个while循环
				//            设置了超时后如何区分是超时返回还是错误返回?
				bytes_received = peer.recv(buffer, 4096);
				if (bytes_received <= 0)
					break;

				clientService.recvData(buffer, bytes_received);
			}

			clientService.closed();
		}
	}

	if (acceptor.close() == -1)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("RealmThread::svc 停止RealmThread监听操作失败.\n")));
		return -1;
	}

	return 0;
}

int RealmThread::open()
{
	if (!loadAccounts())
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("RealmThread::open 加载帐号数据失败, 登录处理线程未能启动.\n")));
		return -1;
	}

	if (activate(THR_NEW_LWP) == -1)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("RealmThread::open 登录处理线程启动失败.\n")));
		return -1;
	}

	m_running = true;

	return 0;
}

int RealmThread::close()
{
	if (!m_running)
		return 0;

	m_running = false;

	if (wait() == -1)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT ("%p.\n"), ACE_TEXT ("RealmThread unable to stop.")));
		return -1;
	}

	return 0;
}

// *-----------------------------------------------------------------
bool RealmThread::loadAccounts()
{
	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("RealmThread::loadAccounts 开始加载帐号数据到内存...\n")));

	QueryResult* result = DATABASE->query("SELECT login, password FROM accounts");
	if (!result)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("RealmThread::loadAccounts accounts表中未配置帐号和密码信息.\n")));
		return false;
	}

	Field * field = result->fetch();
	assert(field);

	// 用户名
	std::string name = field[0].getString();
	transform(name.begin(), name.end(), name.begin(), towupper);
	m_accountName = name;

	// 密码
	std::string pass = field[1].getString();
	transform(pass.begin(), pass.end(), pass.begin(), towupper);
	m_accountPass = pass;

	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("RealmThread::loadAccounts 帐号数据加载成功.\n")));
	delete result;

	return true;
}
