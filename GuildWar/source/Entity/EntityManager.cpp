
// *-----------------------------------------------------------------
//
// 游戏实体对象管理器
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "WorldThread/WorldPacket.h"
#include "Entity/EntityManager.h"
#include "Entity/UpdateFields.h"
#include "Entity/EntityDefines.h"
#include "Database/Database.h"

// ***TODO*** 这样的路径包含太丑陋, 但确实又不想暴露到include目录
//            可考虑做个组件工厂, 工厂的实现在Component目录
#include "../Component/PropertySet/PropertySetImpl.h"
#include "../Component/UnitInterf/UnitInterfImpl.h"
#include "../Component/DataIO/DataIOImpl.h"


bool EntityManager::initialize()
{
	QueryResult* result = DATABASE->query("SELECT MAX(`id`) FROM `characters`");
	if (!result)
		return false;

	m_hiCharGuid = (*result)[0].getUInt() + 1;

	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("EntityManager::initialize 实体管理器初始化成功.\n")));
	delete result;

	return true;
}

Entity* EntityManager::createEntity()
{
	Entity* entity = new Entity();

	// ***TODO*** 现在做的简单处理, entity中创建了所有的Component
	entity->m_componentList.push_back(new PropertySetImpl(entity));
	entity->m_componentList.push_back(new UnitInterfImpl(entity));
	entity->m_componentList.push_back(new DataIOImpl(entity));

	return entity;
}

Entity* EntityManager::createEntity(WorldPacket& packet)
{
	Entity* entity = createEntity();
	if (!entity)
		return 0;
	
	PropertySetComp* propSet = entity->getComponent<PropertySetComp>(ComponentBase::PropertySet);
	UnitInterfComp* unitInterf = entity->getComponent<UnitInterfComp>(ComponentBase::UnitInterf);

	// 设置基本属性
	propSet->initialize(PLAYER_END);
	propSet->setUintValue(OBJECT_FIELD_GUID, m_hiCharGuid++);
	propSet->setUintValue(OBJECT_FIELD_GUID + 1, HIGHGUID_PLAYER);
	propSet->setUintValue(OBJECT_FIELD_TYPE, Entity::Type_Object | Entity::Type_Unit | Entity::Type_Player);

	packet >> entity->m_entityName;

	// ***TODO*** 出生地需要根据种族和职业来查表
	unitInterf->map(0);
	unitInterf->posX(-8921.41f);
	unitInterf->posY(-118.494f);
	unitInterf->posZ(82.273f);
	unitInterf->orientation(0);

	propSet->setFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 0.388999998569489f);
	propSet->setFloatValue(UNIT_FIELD_COMBATREACH, 1.5f);

	// 种族和职业
	u_char race, cls, gender;
	packet >> race >> cls >> gender;

	// ***TODO*** 模型ID, 暂时先固定一个, 人类男性
	propSet->setUintValue(UNIT_FIELD_DISPLAYID, 49);
	propSet->setUintValue(UNIT_FIELD_NATIVEDISPLAYID, 49);

	propSet->setUintValue(UNIT_FIELD_FACTIONTEMPLATE, 1);

	// ***TODO*** 需要查表
	u_char powerType = 1;
	u_int unitfield = 0x00110000;
	propSet->setUintValue(UNIT_FIELD_BYTES_0, (race | (cls << 8) | (gender << 16) | (powerType << 24)));
	propSet->setUintValue(UNIT_FIELD_BYTES_1, unitfield);
	propSet->setUintValue(UNIT_FIELD_BYTES_2, (0x28 << 8));		// players - 0x2800, 0x2801, units - 0x1001
	propSet->setUintValue(UNIT_FIELD_FLAGS, UNIT_FLAG_UNKNOWN1);
	propSet->setFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);
	propSet->setFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);

	// 5项基本属性
	for (int i = STAT_STRENGTH; i < STATS_MAX; ++i)
	{
		propSet->setUintValue(UNIT_FIELD_STAT0 + i, 20);
		propSet->setFloatValue(UNIT_FIELD_POSSTAT0 + i, 0.0f);
		propSet->setFloatValue(UNIT_FIELD_NEGSTAT0 + i, 0.0f);
	}

	// ***TODO*** 生命值, 查表或配置
	propSet->setUintValue(UNIT_FIELD_BASE_HEALTH, 100);
	propSet->setUintValue(UNIT_FIELD_BASE_MANA, 0);
	propSet->setUintValue(UNIT_FIELD_MAXHEALTH, 100);
	propSet->setUintValue(UNIT_FIELD_MAXPOWER1, 0);
	propSet->setUintValue(UNIT_FIELD_MAXPOWER2, 1000);
	propSet->setUintValue(UNIT_FIELD_MAXPOWER3, 0);
	propSet->setUintValue(UNIT_FIELD_MAXPOWER4, 100);
	propSet->setUintValue(UNIT_FIELD_MAXPOWER5, 0);

	u_char skin,face,hairStyle,hairColor,facialHair,outfitId;
	packet >> skin >> face;
	packet >> hairStyle >> hairColor >> facialHair >> outfitId;

	propSet->setUintValue(PLAYER_FIELD_WATCHED_FACTION_INDEX, u_int(-1));
	propSet->setUintValue(PLAYER_BYTES, (skin | (face << 8) | (hairStyle << 16) | (hairColor << 24)));
	propSet->setUintValue(PLAYER_BYTES_2, (facialHair | (0x00 << 8) | (0x00 << 16) | (0x02 << 24)));
	propSet->setUintValue(PLAYER_BYTES_3, gender);

	// ***TODO*** 新建的角色都为1级, 改为可配置的
	propSet->setUintValue(UNIT_FIELD_LEVEL, 1);
	propSet->setUintValue(PLAYER_FIELD_MAX_LEVEL, 70);
	propSet->setUintValue(PLAYER_NEXT_LEVEL_XP, 100);

	propSet->setUintValue(UNIT_FIELD_RESISTANCES, 40);

	propSet->setUintValue(PLAYER_FIELD_MOD_HEALING_DONE_POS, 0);
	for (int i = 0; i < 7; ++i)
	{
		propSet->setUintValue(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + i, 0);
		propSet->setUintValue(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + i, 0);
		propSet->setFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + i, 1.0f);
	}

	propSet->setFloatValue(UNIT_FIELD_BASEATTACKTIME, 2000.0f);
	propSet->setFloatValue(UNIT_FIELD_OFFHANDATTACKTIME, 2000.0f);
	propSet->setFloatValue(UNIT_FIELD_RANGEDATTACKTIME, 2000.0f);

	propSet->setUintValue(PLAYER_CHARACTER_POINTS2, 2);

	return entity;
}
