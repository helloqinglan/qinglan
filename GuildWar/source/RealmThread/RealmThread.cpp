
// *-----------------------------------------------------------------
//
// ��¼�����߳�
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "RealmThread/RealmThread.h"
#include "WorldThread/WorldThread.h"
#include "Database/Database.h"
#include "Network/SocketService.h"
#include "LogonAuthState.h"

// ��¼�������˿�
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
		ACE_ERROR ((GAME_ERROR ACE_TEXT("RealmThread::svc ����ָ���˿�ʧ��.\n")));
		return -1;
	}

	while (m_running)
	{
		// ����Ѿ������ӽ�����WorldServer, ���ټ�������������
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
		}
	}

	if (acceptor.close() == -1)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("RealmThread::svc ֹͣRealmThread��������ʧ��.\n")));
		return -1;
	}

	return 0;
}

int RealmThread::open()
{
	if (!loadAccounts())
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("RealmThread::open �����ʺ�����ʧ��, ��¼�����߳�δ������.\n")));
		return -1;
	}

	if (activate(THR_NEW_LWP) == -1)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("RealmThread::open ��¼�����߳�����ʧ��.\n")));
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
	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("RealmThread::loadAccounts ��ʼ�����ʺ����ݵ��ڴ�...\n")));

	QueryResult* result = DATABASE->query("SELECT login, password FROM accounts");
	if (!result)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("RealmThread::loadAccounts accounts����δ�����ʺź�������Ϣ.\n")));
		return false;
	}

	Field * field = result->fetch();
	assert(field);

	// �û���
	std::string name = field[0].getString();
	transform(name.begin(), name.end(), name.begin(), towupper);
	m_accountName = name;

	// ����
	std::string pass = field[1].getString();
	transform(pass.begin(), pass.end(), pass.begin(), towupper);
	m_accountPass = pass;

	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("RealmThread::loadAccounts �ʺ����ݼ��سɹ�.\n")));
	delete result;

	return true;
}
