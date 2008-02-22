
// *-----------------------------------------------------------------
//
// �������� ����
//
// *-----------------------------------------------------------------

#pragma once

class IObject;

class ComponentBase
{
public:
	// ������Ͷ���
	enum ComopnentTypes
	{
		PropertySet,			// ���Լ�
	};

public:
	ComponentBase(ComopnentTypes ctype, IObject* obj)
		: m_componentType(ctype), m_objectPtr(obj)
	{
	}

	virtual ~ComponentBase() {}

	// ��ѯ�������
	ComopnentTypes componentType() const { return m_componentType; }

	// ��ѯ��������Ϸ����
	IObject* getObjectPtr() const { return m_objectPtr; }

protected:
	ComopnentTypes m_componentType;
	IObject* m_objectPtr;
};
