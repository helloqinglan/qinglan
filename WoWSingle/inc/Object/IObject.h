
// *-----------------------------------------------------------------
//
// ��Ϸʵ�����ʵ��
//
// *-----------------------------------------------------------------

#pragma once

#include "Component/ComponentBase.h"

class IObject
{
public:
	virtual ~IObject()
	{
		for (ComponentList::iterator itr = m_componentList.begin(); 
			itr != m_componentList.end(); ++itr)
			delete itr->second;
	}

	// ��ȡ����Ľӿ�
	template<class T>
	T* getComponent(ComponentBase::ComopnentTypes typeID) const;

	// GUID��ز�ѯ�ӿ�
	const u_int64& guid() const;
	const u_int& guidLow() const;
	const u_int& guidHigh() const;

protected:
	// ����б�
	typedef std::map<ComponentBase::ComopnentTypes, ComponentBase*> ComponentList;
	ComponentList m_componentList;
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
