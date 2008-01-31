
// *-----------------------------------------------------------------
//
// 游戏实体对象实现
//
// *-----------------------------------------------------------------

#pragma once

#include "Component/ComponentBase.h"

class WorldPacket;

class Entity
{
public:
	// 实体类型定义
	enum EntityType
	{
		Type_Object	= 1,
		Type_Player = 16,		// 玩家
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

	// ***TODO*** 这个模板方法应该怎样写, 现在这样感觉T和typeID重复了一次
	template<class T>
	T* getComponent(ComponentBase::ComopnentTypes typeID) const;

	void buildEnumPacket(WorldPacket& packet);

	// ***TODO*** 做一个独立的存储组件 DataIOComp
	// 存数据库
	bool save();

	// 从数据库加载
	// ***TODO*** 不同类型的entity读取的表会不一样, 比如玩家跟NPC, 如何区分?
	bool load(u_int id);

private:
	// ***TODO*** 现在做的简单处理, entity中创建了所有的Component
	typedef std::vector<ComponentBase*> ComponentList;
	ComponentList m_componentList;

	// ***TODO*** entityType是冗余数据, 属性集中记录了实体类型
	// 但是在entity的PropertySet组件创建时又可能需要这个类型, 而此时属性还未创建
	// 想一个好的方法解决这个矛盾, 把这里的entityType变量去掉
	EntityType m_entityType;
	std::string m_entityName;
};
