
// *-----------------------------------------------------------------
//
// 属性集组件实现
//
// *-----------------------------------------------------------------

#pragma once

#include "Component/PropertySetComp.h"

class PropertySetImpl : public PropertySetComp
{
public:
	PropertySetImpl(Entity* entity);
	~PropertySetImpl();

	// 初始化属性集大小
	void initialize(u_int count);

	// 属性集大小
	u_short valueCount() const { return m_valuesCount; }


	// 获取属性数据
	const int& getIntValue(u_short index) const { assert(index < m_valuesCount); return m_intValues[index]; }

	const u_int& getUintValue(u_short index) const { assert(index < m_valuesCount); return m_uintValues[index]; }

	const u_int64& getUint64Value(u_short index) const { assert(index < m_valuesCount); return *((u_int64*)&(m_uintValues[index])); }

	const float& getFloatValue(u_short index) const { assert(index < m_valuesCount); return m_floatValues[index]; }


	// 设置属性数据
	void setIntValue(u_short index, int value);

	void setUintValue(u_short index, u_int value);

	void setUint64Value(u_short index, u_int64 value);

	void setFloatValue(u_short index, float value);


	// 属性位操作
	void setFlag(u_short index, u_int flag);

	void removeFlag(u_short index, u_int flag);

	void toggleFlag(u_short index, u_int flag)
	{
		if (hasFlag(index, flag))
			removeFlag(index, flag);
		else
			setFlag(index, flag);
	}

	bool hasFlag(u_short index, u_int flag);

	void applyModFlag(u_short index, u_int flag, bool apply)
	{
		if (apply)
			setFlag(index, flag);
		else
			removeFlag(index, flag);
	}


	// 获取用于存库的属性集数据
	std::string getPropertyString() const;

	// 从数据库保存的字符串加载属性值数据
	bool loadValues(const std::string& data);

private:
	// 属性集中的数据只有一份, 但是不同field中的数据类型会有所不同
	union
	{
		int* m_intValues;
		u_int* m_uintValues;
		float* m_floatValues;
	};

	u_int* m_uintValuesMirror;		// 数据镜象

	u_short m_valuesCount;			// 属性个数
};
