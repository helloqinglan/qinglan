
// *-----------------------------------------------------------------
//
// ����״̬״̬��(FSM)������
//   1. ������ʱ������ֻ��һ����ǰ״̬
//   2. �ù����಻��������״̬����Ĵ���������
//   ***TODO*** ���ڿ���, �����������û��ʲôʵ�����õ�
//
// *-----------------------------------------------------------------

#pragma once

#include "BaseState.h"

class StateMachine
{
public:
	StateMachine()
		: m_currentState(0)
	{
	}

	virtual ~StateMachine()
	{
	}

	// ���ص�ǰ״̬
	BaseState* currentState() const
	{
		assert(m_currentState && "StateMachine::currentState current state is null.");
		return m_currentState;
	}

	// ת��״̬
	bool transition(BaseState* state)
	{
		if (!state)
		{
			assert(state && "StateMachine::transition state is null.");
			return false;
		}

		BaseState* oldState = m_currentState;
		m_currentState = state;

		if (oldState)
			oldState->leave();
		m_currentState->enter();

		return true;
	}

protected:
	BaseState* m_currentState;
};
