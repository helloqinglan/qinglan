
// *-----------------------------------------------------------------
//
// 属性集组件实现
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "Entity/entity.h"
#include "Entity/UpdateFields.h"
#include "PropertySetImpl.h"
#include "Util/Util.h"

PropertySetImpl::PropertySetImpl(Entity* entity)
: PropertySetComp(entity), m_uintValues(0), m_uintValuesMirror(0), m_valuesCount(0)
{
}

PropertySetImpl::~PropertySetImpl()
{
	delete m_uintValues;
	delete m_uintValuesMirror;
}

void PropertySetImpl::initialize(u_int count)
{
	if (m_valuesCount)
	{
		delete m_uintValues;
		delete m_uintValuesMirror;
	}

	m_valuesCount = count;

	m_uintValues = new u_int[m_valuesCount];
	memset(m_uintValues, 0, sizeof(u_int)*m_valuesCount);

	m_uintValuesMirror = new u_int[m_valuesCount];
	memset(m_uintValuesMirror, 0, sizeof(u_int)*m_valuesCount);
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

void PropertySetImpl::setFlag(u_short index, u_int flag)
{
	u_int oldVal = m_uintValues[index];
	u_int newVal = oldVal | flag;

	if (oldVal != newVal)
		m_uintValues[index] = newVal;
}

void PropertySetImpl::removeFlag(u_short index, u_int flag)
{
	u_int oldVal = m_uintValues[index];
	u_int newVal = oldVal & ~flag;

	if (oldVal != newVal)
		m_uintValues[index] = newVal;
}

bool PropertySetImpl::hasFlag(u_short index, u_int flag)
{
	return ((m_uintValues[index] & flag) != 0);
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

	if (!m_valuesCount)
		initialize((u_int)propData.size());
	else if (propData.size() != m_valuesCount)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("PropertySetImpl::loadValues 属性集数据错误, %u - %u.\n"), 
			propData.size(), m_valuesCount));
		return false;
	}

	int index;
	Tokens::iterator itr;
	for (itr = propData.begin(), index = 0; itr != propData.end(); ++itr, ++index)
	{
		m_uintValues[index] = atol((*itr).c_str());
	}

	return true;
}
