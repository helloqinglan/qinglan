
// *-----------------------------------------------------------------
//
// ���Լ����ʵ��
//
// *-----------------------------------------------------------------

#pragma once

#include "Component/PropertySetComp.h"

class PropertySetImpl : public PropertySetComp
{
public:
	PropertySetImpl(Entity* entity);
	~PropertySetImpl();

	// ��ʼ�����Լ���С
	void initialize(u_int count);

	// ���Լ���С
	u_short valueCount() const { return m_valuesCount; }


	// ��ȡ��������
	const int& getIntValue(u_short index) const { assert(index < m_valuesCount); return m_intValues[index]; }

	const u_int& getUintValue(u_short index) const { assert(index < m_valuesCount); return m_uintValues[index]; }

	const u_int64& getUint64Value(u_short index) const { assert(index < m_valuesCount); return *((u_int64*)&(m_uintValues[index])); }

	const float& getFloatValue(u_short index) const { assert(index < m_valuesCount); return m_floatValues[index]; }


	// ������������
	void setIntValue(u_short index, int value);

	void setUintValue(u_short index, u_int value);

	void setUint64Value(u_short index, u_int64 value);

	void setFloatValue(u_short index, float value);


	// ����λ����
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


	// ��ȡ���ڴ������Լ�����
	std::string getPropertyString() const;

	// �����ݿⱣ����ַ�����������ֵ����
	bool loadValues(const std::string& data);

private:
	// ���Լ��е�����ֻ��һ��, ���ǲ�ͬfield�е��������ͻ�������ͬ
	union
	{
		int* m_intValues;
		u_int* m_uintValues;
		float* m_floatValues;
	};

	u_int* m_uintValuesMirror;		// ���ݾ���

	u_short m_valuesCount;			// ���Ը���
};
