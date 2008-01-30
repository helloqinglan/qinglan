
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
		Type_Object	= 1,
		Type_Player = 16,		// 玩家
	};

	friend class EntityManager;

public:
	Entity(EntityType eType);
	~Entity();

	EntityType entityType() const { return m_entityType; }

	const std::string& name() const { return m_entityName; }
	void name(const std::string& name) { m_entityName = name; }

	const u_int64& guid() const;
	const u_int& guidLow() const;
	const u_int& guidHigh() const;

	// ***TODO*** 这个模板方法应该怎样写, 现在这样感觉T和typeID重复了一次
	template<class T>
	T* getComponent(ComponentBase::ComopnentTypes typeID) const;

	// 存数据库
	bool save();

private:
	// ***TODO*** 现在做的简单处理, entity中创建了所有的Component
	typedef std::vector<ComponentBase*> ComponentList;
	ComponentList m_componentList;

	EntityType m_entityType;
	std::string m_entityName;

	// ***TODO*** 这些属性不是所有的entity都有的
	u_int m_entityRace;
	u_int m_entityClass;

	u_int m_mapID;
	float m_positionX;
	float m_positionY;
	float m_positionZ;
	float m_orientation;
};
