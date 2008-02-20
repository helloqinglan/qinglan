
// *-----------------------------------------------------------------
//
// ��Ϸʵ�����ʵ��
//
// *-----------------------------------------------------------------

#pragma once

#include "Component/ComponentBase.h"

class Entity
{
public:
	// ʵ�����Ͷ���
	enum EntityType
	{
		Type_Unkown = 0,		// δ��ʼ��
		Type_Object	= 1,		// ***TODO*** �������, ��Ϸ�����������͵Ķ�����?
		Type_Unit = 8,
		Type_Player = 16,		// ���
	};

	friend class EntityManager;

public:
	Entity();
	~Entity();

	EntityType entityType() const;

	template<class T>
	T* getComponent(ComponentBase::ComopnentTypes typeID) const;

	const std::string& name() const { return m_entityName; }
	void name(const std::string& name) { m_entityName = name; }

	const u_int64& guid() const;
	const u_int& guidLow() const;
	const u_int& guidHigh() const;

private:
	// ***TODO*** �������ļ򵥴���, entity�д��������е�Component
	typedef std::vector<ComponentBase*> ComponentList;
	ComponentList m_componentList;

	std::string m_entityName;
};


template<class T>
T* Entity::getComponent(ComponentBase::ComopnentTypes typeID) const
{
	return static_cast<T*>(m_componentList[typeID]);
}
