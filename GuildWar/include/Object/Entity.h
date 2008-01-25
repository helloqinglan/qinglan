
// *-----------------------------------------------------------------
//
// ��Ϸʵ�����ʵ��
//
// *-----------------------------------------------------------------

#pragma once

class ComponentBase;

class Entity
{
public:
	// ʵ�����Ͷ���
	enum EntityType
	{
		Type_Player,		// ���

		Type_MaxCount,
	};

	friend class EntityManager;

public:
	Entity(EntityType eType);
	~Entity();

private:
	// ***TODO*** �������ļ򵥴���, entity�д��������е�Component
	typedef std::vector<ComponentBase*> ComponentList;
	ComponentList m_componentList;

	EntityType m_entityType;
};
