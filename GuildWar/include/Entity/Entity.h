
// *-----------------------------------------------------------------
//
// ��Ϸʵ�����ʵ��
//
// *-----------------------------------------------------------------

#pragma once

#include "Component/ComponentBase.h"

class WorldPacket;

class Entity
{
public:
	// ʵ�����Ͷ���
	enum EntityType
	{
		Type_Object	= 1,
		Type_Player = 16,		// ���
	};

	friend class EntityManager;

public:
	Entity(EntityType eType);
	~Entity();

	EntityType entityType() const { return m_entityType; }

	const std::string& name() const { return m_entityName; }
	void name(const std::string& name) { m_entityName = name; }

	const u_int64& guid() const;
	const u_int& guidLow() const;
	const u_int& guidHigh() const;

	// ***TODO*** ���ģ�巽��Ӧ������д, ���������о�T��typeID�ظ���һ��
	template<class T>
	T* getComponent(ComponentBase::ComopnentTypes typeID) const;

	void buildEnumPacket(WorldPacket& packet);

	// ***TODO*** ��һ�������Ĵ洢��� DataIOComp
	// �����ݿ�
	bool save();

	// �����ݿ����
	// ***TODO*** ��ͬ���͵�entity��ȡ�ı�᲻һ��, ������Ҹ�NPC, �������?
	bool load(u_int id);

private:
	// ***TODO*** �������ļ򵥴���, entity�д��������е�Component
	typedef std::vector<ComponentBase*> ComponentList;
	ComponentList m_componentList;

	// ***TODO*** entityType����������, ���Լ��м�¼��ʵ������
	// ������entity��PropertySet�������ʱ�ֿ�����Ҫ�������, ����ʱ���Ի�δ����
	// ��һ���õķ���������ì��, �������entityType����ȥ��
	EntityType m_entityType;
	std::string m_entityName;
};
