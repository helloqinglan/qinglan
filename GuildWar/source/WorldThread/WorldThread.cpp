
// *-----------------------------------------------------------------
//
// 游戏世界处理线程
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "WorldThread/WorldThread.h"
#include "WorldThread/WorldSocket.h"
#include "States/SessionState.h"
#include "Object/EntityManager.h"

// ***TODO*** 世界服务监听端口应该配置到数据库中, 登录服务的服务器列表也从数据库里取
#define WORLDS_LISTEN_PORT 8025

WorldThread::WorldThread() : ACE_Task_Base(), m_running(false), m_hasConnection(false)
{
}

WorldThread::~WorldThread()
{
}

int WorldThread::svc()
{
	ACE_INET_Addr port_to_listen(WORLDS_LISTEN_PORT);
	ACE_SOCK_Acceptor acceptor;

	if (acceptor.open(port_to_listen, 1) == -1)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("WorldThread::svc 监听指定端口失败.\n")));
		return -1;
	}

	while (m_running)
	{
		ACE_SOCK_Stream peer;
		ACE_INET_Addr peer_addr;

		if (acceptor.accept(peer, &peer_addr, 0, 0) == -1)
		{
			if (ACE_OS::last_error() == EINTR)
				ACE_DEBUG ((GAME_DEBUG
				ACE_TEXT ("WorldThread::svc Interrupted while ")
				ACE_TEXT ("waiting for connection\n")));
			else
				if (ACE_OS::last_error() == ETIMEDOUT)
					ACE_DEBUG ((GAME_DEBUG
					ACE_TEXT ("WorldThread::svc Timeout while ")
					ACE_TEXT ("waiting for connection\n")));
		}
		else
		{
			// 有连接建立, 登录服务将不再接受新的登录请求
			m_hasConnection = true;

			WorldSocket clientService(&peer);
			clientService.transition(new SessionState(&clientService));

			ACE_TCHAR peer_name[MAXHOSTNAMELEN];
			peer_addr.addr_to_string(peer_name, MAXHOSTNAMELEN);
			ACE_DEBUG ((GAME_DEBUG
				ACE_TEXT ("WorldThread::svc Connection from %s\n"),
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

			// 连接已关闭, 可以再接受新的登录请求
			m_hasConnection = false;
		}
	}

	if (acceptor.close() == -1)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("WorldThread::svc 停止WorldThread监听操作失败.\n")));
		return -1;
	}

	return 0;
}

int WorldThread::open()
{
	if (!ENTITYMANAGER->initialize())
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("WorldThread::open 实体管理器初始化失败.\n")));
		return -1;
	}

	if (activate(THR_NEW_LWP) == -1)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("WorldThread::open 登录处理线程启动失败.\n")));
		return -1;
	}

	m_running = true;

	return 0;
}

int WorldThread::close()
{
	if (!m_running)
		return 0;

	m_running = false;

	if (wait() == -1)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT ("%p.\n"), ACE_TEXT ("WorldThread unable to stop.")));
		return -1;
	}

	return 0;
}

// *-----------------------------------------------------------------
