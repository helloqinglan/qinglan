
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
#include "../Component/UnitInterf/UnitInterfImpl.h"
#include "../Component/DataIO/DataIOImpl.h"


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

Entity* EntityManager::createEntity()
{
	Entity* entity = new Entity();

	// ***TODO*** �������ļ򵥴���, entity�д��������е�Component
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

	// ���û�������
	propSet->initialize(PLAYER_END);
	propSet->setUintValue(OBJECT_FIELD_GUID, m_hiCharGuid++);
	propSet->setUintValue(OBJECT_FIELD_GUID + 1, HIGHGUID_PLAYER);
	propSet->setUintValue(OBJECT_FIELD_TYPE, Entity::Type_Player);

	packet >> entity->m_entityName;

	// ***TODO*** ��������Ҫ���������ְҵ�����
	unitInterf->map(0);
	unitInterf->posX(-8921.41f);
	unitInterf->posY(-118.494f);
	unitInterf->posZ(82.273f);
	unitInterf->orientation(0);

	propSet->setFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 0.388999998569489f);
	propSet->setFloatValue(UNIT_FIELD_COMBATREACH, 1.5f);

	// �����ְҵ
	u_char race, cls, gender;
	packet >> race >> cls >> gender;

	// ***TODO*** ģ��ID, ��ʱ�ȹ̶�һ��, ��������
	propSet->setUintValue(UNIT_FIELD_DISPLAYID, 49);
	propSet->setUintValue(UNIT_FIELD_NATIVEDISPLAYID, 49);

	// ***TODO*** ��Ҫ���
	u_char powerType = 1;
	u_int unitfield = 0x00110000;
	propSet->setUintValue(UNIT_FIELD_BYTES_0, (race | (cls << 8) | (gender << 16) | (powerType << 24)));
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
