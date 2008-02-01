
// *-----------------------------------------------------------------
//
// 游戏实体对象实现
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "Entity/UpdateFields.h"
#include "Component/PropertySetComp.h"
#include "Component/UnitInterfComp.h"
#include "Component/DataIOComp.h"
#include "Entity/Entity.h"

Entity::Entity()
{
}

Entity::~Entity()
{
	for (ComponentList::iterator itr = m_componentList.begin(); itr != m_componentList.end(); ++itr)
		delete *itr;
}

Entity::EntityType Entity::entityType() const
{
	PropertySetComp* propSet = getComponent<PropertySetComp>(ComponentBase::PropertySet);
	return (Entity::EntityType)propSet->getUintValue(OBJECT_FIELD_TYPE);
}

const u_int64& Entity::guid() const
{
	PropertySetComp* propSet = getComponent<PropertySetComp>(ComponentBase::PropertySet);
	return propSet->getUint64Value(OBJECT_FIELD_GUID);
}

const u_int& Entity::guidLow() const
{
	PropertySetComp* propSet = getComponent<PropertySetComp>(ComponentBase::PropertySet);
	return propSet->getUintValue(OBJECT_FIELD_GUID);
}

const u_int& Entity::guidHigh() const
{
	PropertySetComp* propSet = getComponent<PropertySetComp>(ComponentBase::PropertySet);
	return propSet->getUintValue(OBJECT_FIELD_GUID + 1);
}
