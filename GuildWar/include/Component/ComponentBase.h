
// *-----------------------------------------------------------------
//
// 实体组件基类
//
// *-----------------------------------------------------------------

#pragma once

class Entity;

class ComponentBase
{
public:
	// 组件类型定义
	enum ComopnentTypes
	{
		PropertySet,			// 属性集
	};

public:
	ComponentBase(ComopnentTypes ctype, Entity* entity)
		: m_componentType(ctype), m_entityPtr(entity)
	{
	}

	// 查询组件类型
	ComopnentTypes componentType() const { return m_componentType; }

	// 查询所属的实体
	Entity* getEntityPtr() const { return m_entityPtr; }

protected:
	ComopnentTypes m_componentType;
	Entity* m_entityPtr;
};
