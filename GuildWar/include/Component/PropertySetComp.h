
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

	// 初始化属性集大小
	virtual void initialize(u_int count) = 0;

	// 属性集大小
	virtual u_short valueCount() const = 0;


	// 获取属性数据
	virtual const int& getIntValue(u_short index) const = 0;

	virtual const u_int& getUintValue(u_short index) const = 0;

	virtual const u_int64& getUint64Value(u_short index) const = 0;

	virtual const float& getFloatValue(u_short index) const = 0;


	// 设置属性数据
	virtual void setIntValue(u_short index, int value) = 0;

	virtual void setUintValue(u_short index, u_int value) = 0;

	virtual void setUint64Value(u_short index, u_int64 value) = 0;

	virtual void setFloatValue(u_short index, float value) = 0;


	// 获取用于存库的属性集数据
	virtual std::string getPropertyString() const = 0;

	// 从数据库保存的字符串加载属性值数据
	virtual bool loadValues(const std::string& data) = 0;
};
