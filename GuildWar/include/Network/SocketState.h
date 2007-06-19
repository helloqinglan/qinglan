
// *-----------------------------------------------------------------
//
// socket������״̬����
//
// *-----------------------------------------------------------------

#pragma once

#include "GuildWarFwd.h"

class SocketState
{
public:
	SocketState(SocketService* sm);
	virtual ~SocketState();

	// ״̬��ʼʱ�Ļص�����
	virtual void enter() {}

	// ״̬����ʱ�Ļص�����
	virtual void leave() {}

	// ����ӿ� ��״̬���ж���ýӿڵ�����
	// ***TODO*** ��boost::any�������Ƿ��void*Ҫ��һЩ?
	virtual bool process(void* arg = 0) { return true; }

	// ��ͻ��˷�����
	bool sendData(const char* data, size_t len);

	// �任״̬
	bool transition(SocketState* state);

	// ***TODO*** �÷������������ڲ���˽�б���, �����Ϸ�װ��ԭ��
	CircularBuffer* dataBuffer() const;

protected:
	SocketService* m_socketService;	// �����ķ�������
};
