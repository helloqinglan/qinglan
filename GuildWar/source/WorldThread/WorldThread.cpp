
// *-----------------------------------------------------------------
//
// ��Ϸ���紦���߳�
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "WorldThread/WorldThread.h"
#include "WorldThread/WorldSocket.h"
#include "States/SessionState.h"
#include "Object/EntityManager.h"

// ***TODO*** �����������˿�Ӧ�����õ����ݿ���, ��¼����ķ������б�Ҳ�����ݿ���ȡ
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
		ACE_ERROR ((GAME_ERROR ACE_TEXT("WorldThread::svc ����ָ���˿�ʧ��.\n")));
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
			// �����ӽ���, ��¼���񽫲��ٽ����µĵ�¼����
			m_hasConnection = true;

			WorldSocket clientService(&peer);
			clientService.transition(new SessionState(&clientService));

			ACE_TCHAR peer_name[MAXHOSTNAMELEN];
			peer_addr.addr_to_string(peer_name, MAXHOSTNAMELEN);
			ACE_DEBUG ((GAME_DEBUG
				ACE_TEXT ("WorldThread::svc Connection from %s\n"),
				peer_name));

			clientService.accepted();

			// ***TODO*** ȥ��4096���magic number
			char buffer[4096];

			ssize_t bytes_received;
			while (m_running)
			{
				// recv����-1��ʾ����ʧ�� ����0��ʾ�Զ��ѹر�����
				// ***TODO*** ����Ҫ����һ����ʱ, �����ⲿ�߳��޷������������whileѭ��
				//            �����˳�ʱ����������ǳ�ʱ���ػ��Ǵ��󷵻�?
				bytes_received = peer.recv(buffer, 4096);
				if (bytes_received <= 0)
					break;

				clientService.recvData(buffer, bytes_received);
			}

			clientService.closed();

			// �����ѹر�, �����ٽ����µĵ�¼����
			m_hasConnection = false;
		}
	}

	if (acceptor.close() == -1)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("WorldThread::svc ֹͣWorldThread��������ʧ��.\n")));
		return -1;
	}

	return 0;
}

int WorldThread::open()
{
	if (!ENTITYMANAGER->initialize())
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("WorldThread::open ʵ���������ʼ��ʧ��.\n")));
		return -1;
	}

	if (activate(THR_NEW_LWP) == -1)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("WorldThread::open ��¼�����߳�����ʧ��.\n")));
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
