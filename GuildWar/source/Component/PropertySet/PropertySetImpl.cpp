
// *-----------------------------------------------------------------
//
// 属性集组件实现
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "Object/entity.h"
#include "Object/UpdateFields.h"
#include "PropertySetImpl.h"
#include "Util/Util.h"
#include <sstream>

PropertySetImpl::PropertySetImpl(Entity* entity)
: PropertySetComp(entity)
{
	switch (entity->entityType())
	{
	case Entity::Type_Player:
		m_valuesCount = PLAYER_END;
		break;

	default:
		// 错误
		m_valuesCount = OBJECT_END;
		break;
	}

	m_uintValues = new u_int[m_valuesCount];
	memset(m_uintValues, 0, sizeof(u_int)*m_valuesCount);

	m_uintValuesMirror = new u_int[m_valuesCount];
	memset(m_uintValuesMirror, 0, sizeof(u_int)*m_valuesCount);
}

PropertySetImpl::~PropertySetImpl()
{
	delete m_uintValues;
	delete m_uintValuesMirror;
}

void PropertySetImpl::setIntValue(u_short index, int value)
{
	if (m_intValues[index] != value)
		m_intValues[index] = value;
}

void PropertySetImpl::setUintValue(u_short index, u_int value)
{
	if (m_uintValues[index] != value)
		m_uintValues[index] = value;
}

void PropertySetImpl::setUint64Value(u_short index, u_int64 value)
{
	if (*((u_int64*)&(m_uintValues[index])) != value)
	{
		m_uintValues[index] = *((u_int*)&value);
		m_uintValues[index + 1] = *(((u_int*)&value) + 1);
	}
}

void PropertySetImpl::setFloatValue(u_short index, float value)
{
	if (m_floatValues[index] != value)
		m_floatValues[index] = value;
}

std::string PropertySetImpl::getPropertyString() const
{
	std::ostringstream ss;
	for (u_short i = 0; i < m_valuesCount; ++i)
		ss << getUintValue(i) << " ";

	return ss.str();
}

bool PropertySetImpl::loadValues(const std::string& data)
{
	typedef std::vector<std::string> Tokens;
	Tokens propData = strSplit(data, " ");

	if (propData.size() != m_valuesCount)
		return false;

	int index;
	Tokens::iterator itr;
	for (itr = propData.begin(), index = 0; itr != propData.end(); ++itr, ++index)
	{
		m_uintValues[index] = atol((*itr).c_str());
	}

	return true;
}
