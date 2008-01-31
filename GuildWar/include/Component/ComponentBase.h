
// *-----------------------------------------------------------------
//
// ʵ���������
//
// *-----------------------------------------------------------------

#pragma once

class Entity;

class ComponentBase
{
public:
	// ������Ͷ���
	enum ComopnentTypes
	{
		PropertySet,			// ���Լ�
		UnitInterf,				// ����ӿ�
	};

public:
	ComponentBase(ComopnentTypes ctype, Entity* entity)
		: m_componentType(ctype), m_entityPtr(entity)
	{
	}

	virtual ~ComponentBase() {}

	// ��ѯ�������
	ComopnentTypes componentType() const { return m_componentType; }

	// ��ѯ������ʵ��
	Entity* getEntityPtr() const { return m_entityPtr; }

protected:
	ComopnentTypes m_componentType;
	Entity* m_entityPtr;
};
