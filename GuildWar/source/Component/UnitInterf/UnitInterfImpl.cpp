
// *-----------------------------------------------------------------
//
// 生物接口组件实现
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "Entity/entity.h"
#include "Entity/UpdateFields.h"
#include "UnitInterfImpl.h"
#include "Component/PropertySetComp.h"

UnitInterfImpl::UnitInterfImpl(Entity* entity)
: UnitInterfComp(entity), m_mapID(0), m_positionX(0), m_positionY(0), m_positionZ(0), m_orientation(0)
{
}

u_int UnitInterfImpl::level() const
{
	PropertySetComp* propSet = m_entityPtr->getComponent<PropertySetComp>(ComponentBase::PropertySet);
	return propSet->getUintValue(UNIT_FIELD_LEVEL);
}

u_char UnitInterfImpl::race() const
{
	PropertySetComp* propSet = m_entityPtr->getComponent<PropertySetComp>(ComponentBase::PropertySet);
	return (u_char)(propSet->getUintValue(UNIT_FIELD_BYTES_0) & 0xFF);
}

u_char UnitInterfImpl::cls() const
{
	PropertySetComp* propSet = m_entityPtr->getComponent<PropertySetComp>(ComponentBase::PropertySet);
	return (u_char)((propSet->getUintValue(UNIT_FIELD_BYTES_0) >> 8) & 0xFF);
}

u_char UnitInterfImpl::gender() const
{
	PropertySetComp* propSet = m_entityPtr->getComponent<PropertySetComp>(ComponentBase::PropertySet);
	return (u_char)((propSet->getUintValue(UNIT_FIELD_BYTES_0) >> 16) & 0xFF);
}

u_int UnitInterfImpl::zone() const
{
	// ***TODO*** 如何取zoneID
	return 3459;
}

u_int UnitInterfImpl::area() const
{
	// ***TODO*** 如何取areaID
	return 0;
}
