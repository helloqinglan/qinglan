
// *-----------------------------------------------------------------
//
// 简单的事件服务器实现
//
// 基于FastDelegate, 只实现了部分委托功能:
//   1. 限制了所有的委托函数原型为 bool func(const ByteBuffer& p1)
//   2. 支持多播委托, 但不支持优先级设置, 按与bind相反的顺序回调
//   3. 不支持const member function的注册
//   4. 事件未做分级, 只支持一级事件
//   5. 支持简单的消息截断, 当一个回调返回false时退出回调过程
//
// *-----------------------------------------------------------------

#pragma once

#include "FastDelegate.h"
#include "ByteBuffer.h"

class EventServer
{
public:
	EventServer()
	{
	}

	~EventServer()
	{
		// ***TODO*** 用boost::foreach宏来处理这样的循环
		DelegateMap::iterator itr;
		for (itr = m_delegateMap.begin(); itr != m_delegateMap.end(); ++itr)
		{
			delete itr->second;
		}
	}

	// 注册non-const member function
	template< class X, class Y >
	void bind(unsigned int id, Y* pthis, bool (X::* function_to_bind)(const ByteBuffer& p1))
	{
		DelegateMap::iterator itr = m_delegateMap.find(id);
		if (itr == m_delegateMap.end())
		{
			DelegateList* li = new DelegateList();
			m_delegateMap[id] = li;
			itr = m_delegateMap.find(id);
		}

		DelegateList& delegateLi = *(itr->second);
		MyDelegate delgate(pthis, function_to_bind);
		delegateLi.push_back(delgate);
	}

	// 注销non-const member function
	template< class X, class Y >
	void unbind(unsigned int id, Y* pthis, bool (X::* function_to_bind)(const ByteBuffer& p1))
	{
		DelegateMap::iterator itr = m_delegateMap.find(id);
		if (itr == m_delegateMap.end())
			return;

		MyDelegate delgate(pthis, function_to_bind);

		DelegateList& delegateLi = *(itr->second);
		DelegateList::iterator func_itr;
		for (func_itr = delegateLi.begin(); func_itr != delegateLi.end(); ++func_itr)
		{
			if (*func_itr == delgate)
			{
				delegateLi.erase(func_itr);
				break;
			}
		}

		if (!delegateLi.size())
			m_delegateMap.erase(itr);
	}

	// 注册static function
	void bind(unsigned int id, bool (*function_to_bind)(const ByteBuffer& p1))
	{
		DelegateMap::iterator itr = m_delegateMap.find(id);
		if (itr == m_delegateMap.end())
		{
			DelegateList* li = new DelegateList();
			m_delegateMap[id] = li;
			itr = m_delegateMap.find(id);
		}

		DelegateList& delegateLi = *(itr->second);
		MyDelegate delgate(function_to_bind);
		delegateLi.push_back(delgate);
	}

	// 注销static function
	void unbind(unsigned int id, bool (function_to_bind)(const ByteBuffer& p1))
	{
		DelegateMap::iterator itr = m_delegateMap.find(id);
		if (itr == m_delegateMap.end())
			return;

		MyDelegate delgate(function_to_bind);

		DelegateList& delegateLi = *(itr->second);
		DelegateList::iterator func_itr;
		for (func_itr = delegateLi.begin(); func_itr != delegateLi.end(); ++func_itr)
		{
			if (*func_itr == delgate)
			{
				delegateLi.erase(func_itr);
				break;
			}
		}

		if (!delegateLi.size())
			m_delegateMap.erase(itr);
	}

	// 激发事件
	void emit(unsigned int id, const ByteBuffer& p1)
	{
		DelegateMap::iterator itr = m_delegateMap.find(id);
		if (itr == m_delegateMap.end())
			return;

		DelegateList& delegateLi = *(itr->second);
		DelegateList::reverse_iterator func_itr;
		for (func_itr = delegateLi.rbegin(); func_itr != delegateLi.rend(); ++func_itr)
		{
			if (!(*func_itr)(p1))
				break;
		}
	}

private:
	typedef fastdelegate::FastDelegate1<const ByteBuffer&, bool> MyDelegate;
	typedef std::vector<MyDelegate> DelegateList;
	typedef std::map<unsigned int, DelegateList*> DelegateMap;
	DelegateMap m_delegateMap;
};

typedef ACE_Singleton<EventServer, ACE_Null_Mutex> EventServerSingleton;
#define S_EventServer EventServerSingleton::instance()
