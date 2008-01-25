
// *-----------------------------------------------------------------
//
// 属性集组件基类
//
// *-----------------------------------------------------------------

#pragma once

#include "ComponentBase.h"

class PropertySetComp : public ComponentBase
{
public:
	PropertySetComp(Entity* entity)
		: ComponentBase(ComponentBase::PropertySet, entity)
	{
	}
};
