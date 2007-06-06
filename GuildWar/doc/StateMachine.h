
// *-----------------------------------------------------------------
//
// 有限状态状态机(FSM)管理类
//   1. 在任意时刻有且只有一个当前状态
//   2. 该管理类不负责管理各状态对象的创建和销毁
//   ***TODO*** 现在看来, 这个管理类是没有什么实际作用的
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

	// 返回当前状态
	BaseState* currentState() const
	{
		assert(m_currentState && "StateMachine::currentState current state is null.");
		return m_currentState;
	}

	// 转换状态
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
