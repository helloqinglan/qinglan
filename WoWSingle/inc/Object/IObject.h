
// *-----------------------------------------------------------------
//
// 游戏实体对象实现
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

	// 获取组件的接口
	template<class T>
	T* getComponent(ComponentBase::ComopnentTypes typeID) const;

	// GUID相关查询接口
	const u_int64& guid() const;
	const u_int& guidLow() const;
	const u_int& guidHigh() const;

protected:
	// 组件列表
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
