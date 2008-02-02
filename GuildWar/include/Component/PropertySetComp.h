
// *-----------------------------------------------------------------
//
// ���Լ��������
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

	// ��ʼ�����Լ���С
	virtual void initialize(u_int count) = 0;

	// ���Լ���С
	virtual u_short valueCount() const = 0;


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


	// ��ȡ���ڴ������Լ�����
	virtual std::string getPropertyString() const = 0;

	// �����ݿⱣ����ַ�����������ֵ����
	virtual bool loadValues(const std::string& data) = 0;
};
