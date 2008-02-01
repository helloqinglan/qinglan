
// *-----------------------------------------------------------------
//
// ���ݴ�ȡ���ʵ����
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "Component/PropertySetComp.h"
#include "Component/UnitInterfComp.h"
#include "WorldThread/WorldPacket.h"
#include "Entity/EntityDefines.h"
#include "Entity/UpdateFields.h"
#include "Entity/Entity.h"
#include "DataIOImpl.h"
#include "Database/Database.h"

DataIOImpl::DataIOImpl(Entity* entity)
: DataIOComp(entity)
{
}

bool DataIOImpl::save()
{
	// ��ɾ���ٲ���, ����Ҫ����������¼�¼���Ǹ������м�¼
	DATABASE->pexecute("DELETE FROM `characters` WHERE `id` = '%u'", m_entityPtr->guidLow());

	PropertySetComp* propSet = m_entityPtr->getComponent<PropertySetComp>(ComponentBase::PropertySet);
	UnitInterfComp* unitInterf = m_entityPtr->getComponent<UnitInterfComp>(ComponentBase::UnitInterf);

	std::ostringstream ss;
	ss << "INSERT INTO `characters` (`id`,`name`,`pos_x`,`pos_y`,`pos_z`,`map`,`data`) VALUES (";
	ss << m_entityPtr->guidLow();
	ss << ",'";
	ss << m_entityPtr->name();
	ss << "',";
	ss << unitInterf->posX();
	ss << ",";
	ss << unitInterf->posY();
	ss << ",";
	ss << unitInterf->posZ();
	ss << ",";
	ss << unitInterf->map();
	ss << ",'";
	ss << propSet->getPropertyString();
	ss << "')";

	DATABASE->execute(ss.str().c_str());

	return true;
}

bool DataIOImpl::miniLoad(u_int id)
{
	QueryResult* result = DATABASE->pquery(
		"SELECT `name`, `pos_x`, `pos_y`, `pos_z`, `map`, `data` FROM `characters` WHERE `id`='%u'", id);
	if (!result)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("Entity::miniLoad ���ݿ���û�н�ɫIDΪ%u������.\n"), id));
		return false;
	}

	Field* field = result->fetch();
	PropertySetComp* propSet = m_entityPtr->getComponent<PropertySetComp>(ComponentBase::PropertySet);
	UnitInterfComp* unitInterf = m_entityPtr->getComponent<UnitInterfComp>(ComponentBase::UnitInterf);

	if (!propSet->loadValues(field[5].getString()))
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("Entity::miniLoad ���Լ����ݴ���, ��ɫ%u����ʧ��.\n"), id));

		delete result;
		return false;
	}

	m_entityPtr->name(field[0].getString());
	unitInterf->posX(field[1].getFloat());
	unitInterf->posY(field[2].getFloat());
	unitInterf->posZ(field[3].getFloat());
	unitInterf->map(field[4].getUInt());

	delete result;
	return true;
}

bool DataIOImpl::load(u_int id)
{
	if (!miniLoad(id))
		return false;

	PropertySetComp* propSet = m_entityPtr->getComponent<PropertySetComp>(ComponentBase::PropertySet);

	// ������������

	// ***TODO*** ����ǲ���Ӧ���ڼ�����ص��������?
	// ���ͨ�������������
	propSet->setUint64Value(UNIT_FIELD_CHANNEL_OBJECT, 0);
	propSet->setUintValue(UNIT_CHANNEL_SPELL, 0);

	// ��� charm/summon �������
	propSet->setUint64Value(UNIT_FIELD_CHARM, 0);
	propSet->setUint64Value(UNIT_FIELD_SUMMON, 0);
	propSet->setUint64Value(UNIT_FIELD_CHARMEDBY, 0);
	propSet->setUint64Value(UNIT_FIELD_SUMMONEDBY, 0);
	propSet->setUint64Value(UNIT_FIELD_CREATEDBY, 0);

	return true;
}

void DataIOImpl::buildEnumPacket(WorldPacket& packet)
{
	PropertySetComp* propSet = m_entityPtr->getComponent<PropertySetComp>(ComponentBase::PropertySet);
	UnitInterfComp* unitInterf = m_entityPtr->getComponent<UnitInterfComp>(ComponentBase::UnitInterf);

	packet << m_entityPtr->guid();				// GUID
	packet << m_entityPtr->name();				// ��ɫ��

	packet << unitInterf->race();				// ����
	packet << unitInterf->cls();				// ְҵ
	packet << unitInterf->gender();				// �Ա�

	u_int bytes = propSet->getUintValue(PLAYER_BYTES);				// PLAYER_BYTES
	packet << u_char(bytes);					// ���˳���ǲ��Ƿ�������? ���û��ֱ������u_int
	packet << u_char(bytes >> 8);
	packet << u_char(bytes >> 16);
	packet << u_char(bytes >> 24);
	packet << (u_char)propSet->getUintValue(PLAYER_BYTES_2);	// PLAYER_BYTES_2

	packet << (u_char)unitInterf->level();		// �ȼ�
	packet << unitInterf->zone();				// ����ID
	packet << unitInterf->map();				// ��ͼID

	packet << unitInterf->posX();				// ����
	packet << unitInterf->posY();
	packet << unitInterf->posZ();

	packet << (u_int)0;							// ***TODO*** : ����ID

	packet << (u_char)0;						// ***TODO*** : δ֪ (different values on off, looks like flags)
	packet << (u_char)0;						// ***TODO*** : ��ʾ��־ (��Player.cpp 1123��)
	packet << (u_char)0xa0;						// ***TODO*** : δ֪
	packet << (u_char)0;						// ***TODO*** : δ֪
	packet << (u_char)1;						// ***TODO*** : δ֪

	packet << (u_int)0;							// ***TODO*** : ������Ϣ
	packet << (u_int)0;
	packet << (u_int)0;

	// ***TODO*** : װ����Ϣ
	for (int i = 0; i < EQUIPMENT_SLOT_END; ++i)
	{
		packet << (u_int)0;						// ***TODO*** : ��ʾID
		packet << (u_char)0;					// ***TODO*** : InventoryType (ʲô��˼?)
	}

	packet << (u_int)0;							// ***TODO*** : δ֪ (first bag display id)
	packet << (u_char)0;						// ***TODO*** : δ֪ (first bag inventory type)
}
