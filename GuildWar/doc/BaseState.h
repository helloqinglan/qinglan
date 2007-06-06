
// *-----------------------------------------------------------------
//
// ״̬����
//
// *-----------------------------------------------------------------

#pragma once

class BaseState
{
public:
	virtual ~BaseState() {}

	// ״̬��ʼʱ�Ļص�����
	virtual void enter() {}

	// ״̬����ʱ�Ļص�����
	virtual void leave() {}

	// ����ӿ� ��״̬���ж���ýӿڵ�����
	// ***TODO*** ��boost::any�������Ƿ��void*Ҫ��һЩ?
	virtual bool process(void* arg = 0) { return true; }
};
