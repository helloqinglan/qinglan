
// *-----------------------------------------------------------------
//
// 游戏实体对象实现
//
// *-----------------------------------------------------------------

#pragma once

#include "Component/ComponentBase.h"

class Entity
{
public:
	// 实体类型定义
	enum EntityType
	{
		Type_Unkown = 0,		// 未初始化
		Type_Object	= 1,		// ***TODO*** 对象基类, 游戏中有这种类型的对象吗?
		Type_Unit = 8,
		Type_Player = 16,		// 玩家
	};

	friend class EntityManager;

public:
	Entity();
	~Entity();

	EntityType entityType() const;

	template<class T>
	T* getComponent(ComponentBase::ComopnentTypes typeID) const;

	const std::string& name() const { return m_entityName; }
	void name(const std::string& name) { m_entityName = name; }

	const u_int64& guid() const;
	const u_int& guidLow() const;
	const u_int& guidHigh() const;

private:
	// ***TODO*** 现在做的简单处理, entity中创建了所有的Component
	typedef std::vector<ComponentBase*> ComponentList;
	ComponentList m_componentList;

	std::string m_entityName;
};


template<class T>
T* Entity::getComponent(ComponentBase::ComopnentTypes typeID) const
{
	return static_cast<T*>(m_componentList[typeID]);
}
