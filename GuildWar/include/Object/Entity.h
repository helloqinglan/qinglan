
// *-----------------------------------------------------------------
//
// 游戏实体对象实现
//
// *-----------------------------------------------------------------

#pragma once

class ComponentBase;

class Entity
{
public:
	// 实体类型定义
	enum EntityType
	{
		Type_Player,		// 玩家

		Type_MaxCount,
	};

	friend class EntityManager;

public:
	Entity(EntityType eType);
	~Entity();

private:
	// ***TODO*** 现在做的简单处理, entity中创建了所有的Component
	typedef std::vector<ComponentBase*> ComponentList;
	ComponentList m_componentList;

	EntityType m_entityType;
};
