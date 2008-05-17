
// *-----------------------------------------------------------------
//
// 属性集组件 基类
//
// *-----------------------------------------------------------------

#pragma once

#include "Component/ComponentBase.h"
#include <string>

class PropertySetComp : public ComponentBase
{
public:
	PropertySetComp(IObject* obj)
		: ComponentBase(ComponentBase::PropertySet, obj)
	{
	}

	// 初始化属性集大小
	virtual void initialize(u_int count) = 0;

	// 属性集大小
	virtual u_short valueCount() const = 0;

	// 属性集是否已更新
	virtual bool updated() const = 0;

	virtual bool updated(u_short index) const = 0;


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

	void setStatFloatValue(u_short index, float value)
	{
		if (value < 0)
			value = 0.0f;

		setFloatValue(index, value);
	}

	void setStatIntValue(u_short index, int value)
	{
		if (value < 0)
			value = 0;

		setUintValue(index, (u_int)value);
	}


	// 附加属性数据
	virtual void applyModUintValue(u_short index, int val, bool apply) = 0;

	virtual void applyModIntValue(u_short index, int val, bool apply) = 0;

	virtual void applyModPositiveFloatValue(u_short index, float val, bool apply) = 0;

	virtual void applyModSignedFloatValue(u_short index, float val, bool apply) = 0;

	void applyPercentModFloatValue(u_short index, float val, bool apply)
	{
		val = val != -100.0f ? val : -99.9f ;
		float applyVal = apply ? (100.0f + val) / 100.0f : 100.0f / (100.0f + val);
		setFloatValue(index, getFloatValue(index) * applyVal);
	}


	// 属性位操作
	virtual void setFlag(u_short index, u_int flag) = 0;

	virtual void removeFlag(u_short index, u_int flag) = 0;

	virtual void toggleFlag(u_short index, u_int flag) = 0;

	virtual bool hasFlag(u_short index, u_int flag) = 0;

	virtual void applyModFlag(u_short index, u_int flag, bool apply) = 0;


	// 清除更新信息
	virtual void clearUpdateMask(bool remove) = 0;


	// 获取用于存库的属性集数据
	virtual std::string getPropertyString() const = 0;

	// 从数据库保存的字符串加载属性值数据
	virtual bool loadValues(const std::string& data) = 0;
};
