
// *-----------------------------------------------------------------
//
// 游戏实体对象实现
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "Component/PropertySetComp.h"
#include "Object/UpdateFields.h"
#include "Object/Entity.h"
#include "Database/Database.h"
#include <sstream>

Entity::Entity(EntityType eType)
: m_entityType(eType)
{
}

Entity::~Entity()
{
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

template<class T>
T* Entity::getComponent(ComponentBase::ComopnentTypes typeID) const
{
	return (T*)m_componentList[typeID];
}

bool Entity::save()
{
	// 先删除再插入, 否则要区分是添加新记录还是更新现有记录
	DATABASE->pexecute("DELETE FROM `characters` WHERE `id` = '%u'", guidLow());

	PropertySetComp* propSet = getComponent<PropertySetComp>(ComponentBase::PropertySet);

	std::ostringstream ss;
	ss << "INSERT INTO `characters` (`id`,`name`,`pos_x`,`pos_y`,`pos_z`,`map`,`data`) VALUES (";
	ss << guidLow();
	ss << ",'";
	ss << m_entityName;
	ss << "',";
	ss << m_positionX;
	ss << ",";
	ss << m_positionY;
	ss << ",";
	ss << m_positionZ;
	ss << ",";
	ss << m_mapID;
	ss << ",'";
	ss << propSet->getPropertyString();
	ss << "')";
	ss << guidLow();

	DATABASE->execute(ss.str().c_str());

	return true;
}
