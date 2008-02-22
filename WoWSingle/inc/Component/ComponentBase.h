
// *-----------------------------------------------------------------
//
// 对象的组件 基类
//
// *-----------------------------------------------------------------

#pragma once

class IObject;

class ComponentBase
{
public:
	// 组件类型定义
	enum ComopnentTypes
	{
		PropertySet,			// 属性集
	};

public:
	ComponentBase(ComopnentTypes ctype, IObject* obj)
		: m_componentType(ctype), m_objectPtr(obj)
	{
	}

	virtual ~ComponentBase() {}

	// 查询组件类型
	ComopnentTypes componentType() const { return m_componentType; }

	// 查询所属的游戏对象
	IObject* getObjectPtr() const { return m_objectPtr; }

protected:
	ComopnentTypes m_componentType;
	IObject* m_objectPtr;
};
