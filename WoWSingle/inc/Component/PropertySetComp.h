
// *-----------------------------------------------------------------
//
// ���Լ���� ����
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

	// ��ʼ�����Լ���С
	virtual void initialize(u_int count) = 0;

	// ���Լ���С
	virtual u_short valueCount() const = 0;

	// ���Լ��Ƿ��Ѹ���
	virtual bool updated() const = 0;

	virtual bool updated(u_short index) const = 0;


	// ��ȡ��������
	virtual const int& getIntValue(u_short index) const = 0;

	virtual const u_int& getUintValue(u_short index) const = 0;

	virtual const u_int64& getUint64Value(u_short index) const = 0;

	virtual const float& getFloatValue(u_short index) const = 0;


	// ������������
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


	// ������������
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


	// ����λ����
	virtual void setFlag(u_short index, u_int flag) = 0;

	virtual void removeFlag(u_short index, u_int flag) = 0;

	virtual void toggleFlag(u_short index, u_int flag) = 0;

	virtual bool hasFlag(u_short index, u_int flag) = 0;

	virtual void applyModFlag(u_short index, u_int flag, bool apply) = 0;


	// ���������Ϣ
	virtual void clearUpdateMask(bool remove) = 0;


	// ��ȡ���ڴ������Լ�����
	virtual std::string getPropertyString() const = 0;

	// �����ݿⱣ����ַ�����������ֵ����
	virtual bool loadValues(const std::string& data) = 0;
};
