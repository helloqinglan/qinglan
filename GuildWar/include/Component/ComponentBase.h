
// *-----------------------------------------------------------------
//
// 实体组件基类
//
// *-----------------------------------------------------------------

#pragma once

#include "GuildWarFwd.h"

class ComponentBase
{
public:
	// 组件类型定义
	enum ComopnentTypes
	{
		PropertySet,			// 属性集
		UnitInterf,				// 生物接口
		DataIO,					// 数据存取
	};

public:
	ComponentBase(ComopnentTypes ctype, Entity* entity)
		: m_componentType(ctype), m_entityPtr(entity)
	{
	}

	virtual ~ComponentBase() {}

	// 查询组件类型
	ComopnentTypes componentType() const { return m_componentType; }

	// 查询所属的实体
	Entity* getEntityPtr() const { return m_entityPtr; }

protected:
	ComopnentTypes m_componentType;
	Entity* m_entityPtr;
};
