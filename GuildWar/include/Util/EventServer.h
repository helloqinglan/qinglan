
// *-----------------------------------------------------------------
//
// �򵥵��¼�������ʵ��
//
// ����FastDelegate, ֻʵ���˲���ί�й���:
//   1. ���������е�ί�к���ԭ��Ϊ bool func(const ByteBuffer& p1)
//   2. ֧�ֶಥί��, ����֧�����ȼ�����, ����bind�෴��˳��ص�
//   3. ��֧��const member function��ע��
//   4. �¼�δ���ּ�, ֻ֧��һ���¼�
//   5. ֧�ּ򵥵���Ϣ�ض�, ��һ���ص�����falseʱ�˳��ص�����
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
		// ***TODO*** ��boost::foreach��������������ѭ��
		DelegateMap::iterator itr;
		for (itr = m_delegateMap.begin(); itr != m_delegateMap.end(); ++itr)
		{
			delete itr->second;
		}
	}

	// ע��non-const member function
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

	// ע��non-const member function
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

	// ע��static function
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

	// ע��static function
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

	// �����¼�
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
