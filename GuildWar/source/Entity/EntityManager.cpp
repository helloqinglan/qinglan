
// *-----------------------------------------------------------------
//
// ��Ϸʵ����������
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "WorldThread/WorldPacket.h"
#include "Entity/EntityManager.h"
#include "Entity/UpdateFields.h"
#include "Entity/EntityDefines.h"
#include "Database/Database.h"

// ***TODO*** ������·������̫��ª, ��ȷʵ�ֲ��뱩¶��includeĿ¼
//            �ɿ��������������, ������ʵ����ComponentĿ¼
#include "../Component/PropertySet/PropertySetImpl.h"


bool EntityManager::initialize()
{
	QueryResult* result = DATABASE->query("SELECT MAX(`id`) FROM `characters`");
	if (!result)
		return false;

	m_hiCharGuid = (*result)[0].getUInt() + 1;

	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("EntityManager::initialize ʵ���������ʼ���ɹ�.\n")));
	delete result;

	return true;
}

Entity* EntityManager::createEntity(Entity::EntityType eType)
{
	Entity* entity = new Entity(eType);

	// ***TODO*** �������ļ򵥴���, entity�д��������е�Component
	entity->m_componentList.push_back(new PropertySetImpl(entity));

	return entity;
}

Entity* EntityManager::createEntity(WorldPacket& packet)
{
	Entity* entity = createEntity(Entity::Type_Player);
	if (!entity)
		return 0;
	
	PropertySetComp* propSet = entity->getComponent<PropertySetComp>(ComponentBase::PropertySet);

	// ���û�������
	propSet->setUintValue(OBJECT_FIELD_GUID, m_hiCharGuid++);
	propSet->setUintValue(OBJECT_FIELD_GUID + 1, HIGHGUID_PLAYER);
	propSet->setUintValue(OBJECT_FIELD_TYPE, Entity::Type_Player);

	packet >> entity->m_entityName;

	// �����ְҵ
	u_char race,class_;
	packet >> race >> class_;
	entity->m_entityRace = race;
	entity->m_entityClass = class_;

	// ***TODO*** ��������Ҫ���������ְҵ�����
	entity->m_mapID = 0;
	entity->m_positionX = 0;
	entity->m_positionY = 0;
	entity->m_positionZ = 0;
	entity->m_orientation = 0;

	propSet->setFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 0.388999998569489f);
	propSet->setFloatValue(UNIT_FIELD_COMBATREACH, 1.5f);

	// ***TODO*** ģ��ID, ��ʱ�ȹ̶�һ��
	u_char gender;
	packet >> gender;
	propSet->setUintValue(UNIT_FIELD_DISPLAYID, 0);
	propSet->setUintValue(UNIT_FIELD_NATIVEDISPLAYID, 0);

	// ***TODO*** ��Ҫ���
	u_char powerType = 0;
	u_int unitfield = 0;
	propSet->setUintValue(UNIT_FIELD_BYTES_0, (race | (class_ << 8) | (gender << 16) | (powerType << 24)));
	propSet->setUintValue(UNIT_FIELD_BYTES_1, unitfield);
	propSet->setUintValue(UNIT_FIELD_BYTES_2, (0x28 << 8));		// players - 0x2800, 0x2801, units - 0x1001
	propSet->setUintValue(UNIT_FIELD_FLAGS, UNIT_FLAG_UNKNOWN1);
	propSet->setFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);

	u_char skin,face,hairStyle,hairColor,facialHair,outfitId;
	packet >> skin >> face;
	packet >> hairStyle >> hairColor >> facialHair >> outfitId;

	propSet->setUintValue(PLAYER_FIELD_WATCHED_FACTION_INDEX, u_int(-1));
	propSet->setUintValue(PLAYER_BYTES, (skin | (face << 8) | (hairStyle << 16) | (hairColor << 24)));
	propSet->setUintValue(PLAYER_BYTES_2, (facialHair | (0x00 << 8) | (0x00 << 16) | (0x02 << 24)));
	propSet->setUintValue(PLAYER_BYTES_3, gender);

	// ***TODO*** �½��Ľ�ɫ��Ϊ1��, ��Ϊ�����õ�
	propSet->setUintValue(UNIT_FIELD_LEVEL, 1);

	return entity;
}
