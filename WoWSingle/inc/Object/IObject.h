
// *-----------------------------------------------------------------
//
// 游戏实体对象实现
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

	// 获取组件的接口
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

	// GUID相关查询接口
	const u_int64& guid() const;
	const u_int& guidLow() const;
	const u_int& guidHigh() const;

	// 对象是否已加入到游戏世界中
	virtual bool isInWorld() const = 0;

protected:
	// 组件列表
	typedef std::map<ComponentBase::ComopnentTypes, ComponentBase*> ComponentList;
	ComponentList m_componentList;

	// 固定组件
	// 所有类型的对象都有的组件, 减少查询次数
	PropertySetComp* m_propertySet;				// 属性集
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
