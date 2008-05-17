
// *-----------------------------------------------------------------
//
// ��Ϸʵ�����ʵ��
//
// *-----------------------------------------------------------------

#pragma once

#include "Component/PropertySetComp.h"
#include <map>

class IObject
{
public:
	IObject()
	: m_propertySet(0)
	{
	}

	virtual ~IObject()
	{
		for (ComponentList::iterator itr = m_componentList.begin(); 
			itr != m_componentList.end(); ++itr)
			delete itr->second;
	}

	// ��ȡ����Ľӿ�
	template<class T>
	T* getComponent(ComponentBase::ComopnentTypes typeID) const;

	void pushComponent(ComponentBase* comp)
	{
		ComponentList::iterator itr = m_componentList.find(comp->componentType());
		if (itr != m_componentList.end())
		{
			// sLog.outError();
			delete itr->second;
			m_componentList.erase(itr);
		}

		m_componentList[comp->componentType()] = comp;
	}

	// GUID��ز�ѯ�ӿ�
	const u_int64& guid() const;
	const u_int& guidLow() const;
	const u_int& guidHigh() const;

	// �����Ƿ��Ѽ��뵽��Ϸ������
	virtual bool isInWorld() const = 0;

protected:
	// ����б�
	typedef std::map<ComponentBase::ComopnentTypes, ComponentBase*> ComponentList;
	ComponentList m_componentList;

	// �̶����
	// �������͵Ķ����е����, ���ٲ�ѯ����
	PropertySetComp* m_propertySet;				// ���Լ�
};

template<class T>
T* IObject::getComponent(ComponentBase::ComopnentTypes typeID) const
{
	ComponentBase* comp = 0;

	ComponentList::iterator itr = m_componentList.find(typeID);
	if (itr != 0)
		comp = itr->second;

	return static_cast<T*>(comp);
}
