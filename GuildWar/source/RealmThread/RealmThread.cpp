
#include "GuildWarPch.h"
#include "RealmThread/RealmThread.h"

// ***TODO*** �������ŵ�һ��������ͷ�ļ���
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
		if (0)
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
				ACE_DEBUG ((GAME_DEBUG,
							ACE_TEXT ("RealmThread::svc Interrupted while ")
							ACE_TEXT ("waiting for connection\n")));
			else
				if (ACE_OS::last_error() == ETIMEDOUT)
					ACE_DEBUG ((GAME_DEBUG,
								ACE_TEXT ("RealmThread::svc Timeout while ")
								ACE_TEXT ("waiting for connection\n")));
		}
		else
		{
/*			RealmClientService clientService(&peer);

			ACE_TCHAR peer_name[MAXHOSTNAMELEN];
			peer_addr.addr_to_string(peer_name, MAXHOSTNAMELEN);
			ACE_DEBUG ((GAME_DEBUG,
						ACE_TEXT ("RealmThread::svc Connection from %s\n"),
						peer_name));

			char buffer[4096];
			ssize_t bytes_received;

			clientService.accepted();

			// recv����-1��ʾ����ʧ�� ����0��ʾ�Զ��ѹر�����
			while ((bytes_received = peer.recv(buffer, sizeof(buffer))))
			{
				clientService.recvData(buffer, bytes_received);
			}

			clientService.closed();*/
		}
	}

	return 0;
}

int RealmThread::open()
{
	if (activate(THR_NEW_LWP) == -1)
	{
		ACE_ERROR ((LM_ERROR, "%N:%l:%p\n", "RealmThread:open: <activate> failed"));
		return -1;
	}

	m_running = true;

	return 0;
}

int RealmThread::close()
{
/*	if (m_running)
		return 0;

	if( this->proactor_->proactor_end_event_loop() == -1 )
		ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t):%p.\n", "RealmThread <Proactor::proactor_end_event_loop> failed"), -1);

	delete this->proactor_;
	this->proactor_ = 0;

	if( this->wait() == -1 )
		ACE_ERROR ((LM_ERROR, ACE_TEXT ("%p.\n"), ACE_TEXT ("RealmThread unable to stop thread pool")));

	this->running_ = false;
*/
	return 0;
}
