
// *-----------------------------------------------------------------
//
// 游戏实体对象管理器
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "Object/entity.h"
#include "Object/EntityManager.h"

// ***TODO*** 这样的路径包含太丑陋, 但确实又不想暴露到include目录
//            可考虑做个组件工厂, 工厂的实现在Component目录
#include "../Component/PropertySet/PropertySetImpl.h"

Entity* EntityManager::createEntity(Entity::EntityType eType)
{
	Entity* entity = new Entity(eType);

	// ***TODO*** 现在做的简单处理, entity中创建了所有的Component
	entity->m_componentList.push_back(new PropertySetImpl(entity));

	return 0;
}
