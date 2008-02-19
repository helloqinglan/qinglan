
// *-----------------------------------------------------------------
//
// ʵ�����Լ��ֶζ���
//
// *-----------------------------------------------------------------

#pragma once

enum EObjectFields
{
	OBJECT_FIELD_GUID					= 0x0000,	// ����GUID, Size: 2, Type: LONG
	OBJECT_FIELD_TYPE					= 0x0002,	// ***TODO***, Size: 1, Type: INT
	OBJECT_FIELD_ENTRY					= 0x0003,	// ***TODO***, Size: 1, Type: INT
	OBJECT_FIELD_SCALE_X				= 0x0004,	// ���������ֵ, Size: 1, Type: FLOAT
	OBJECT_FIELD_PADDING				= 0x0005,	// ***TODO***, Size: 1, Type: INT
	OBJECT_END							= 0x0006,
};

enum EItemFields
{
	ITEM_END							= OBJECT_END + 0x0036,
};

enum EContainerFields
{
	CONTAINER_END						= ITEM_END + 0x004A,
};

enum EUnitFields
{
	UNIT_FIELD_CHARM					= OBJECT_END + 0x0000,	// ***TODO***, Size: 2, Type: LONG
	UNIT_FIELD_SUMMON					= OBJECT_END + 0x0002,	// ***TODO***, Size: 2, Type: LONG
	UNIT_FIELD_CHARMEDBY				= OBJECT_END + 0x0004,	// ***TODO***, Size: 2, Type: LONG
	UNIT_FIELD_SUMMONEDBY				= OBJECT_END + 0x0006,	// ***TODO***, Size: 2, Type: LONG
	UNIT_FIELD_CREATEDBY				= OBJECT_END + 0x0008,	// ***TODO***, Size: 2, Type: LONG
	UNIT_FIELD_TARGET					= OBJECT_END + 0x000A,	// ***TODO***, Size: 2, Type: LONG
	UNIT_FIELD_PERSUADED				= OBJECT_END + 0x000C,	// ***TODO***, Size: 2, Type: LONG
	UNIT_FIELD_CHANNEL_OBJECT			= OBJECT_END + 0x000E,	// ***TODO*** ͨ���༼�����, Size: 2, Type: LONG

	UNIT_FIELD_MAXHEALTH				= OBJECT_END + 0x0016,	// �������ֵ
	UNIT_FIELD_MAXPOWER1				= OBJECT_END + 0x0017,	// 5���������ֵ, Size: 1, Type: INT
	UNIT_FIELD_MAXPOWER2				= OBJECT_END + 0x0018,	//
	UNIT_FIELD_MAXPOWER3				= OBJECT_END + 0x0019,	//
	UNIT_FIELD_MAXPOWER4				= OBJECT_END + 0x001A,	//
	UNIT_FIELD_MAXPOWER5				= OBJECT_END + 0x001B,	//

	UNIT_FIELD_LEVEL					= OBJECT_END + 0x001C,	// ��ɫ�ȼ�, Size: 1, Type: INT

	UNIT_FIELD_BYTES_0					= OBJECT_END + 0x001E,	// ���塢ְҵ���Ա���������, Size: 1, Type: INT
	UNIT_FIELD_FLAGS					= OBJECT_END + 0x0028,	// ***TODO***, Size: 1, Type: INT

	UNIT_FIELD_BOUNDINGRADIUS			= OBJECT_END + 0x0090,	// ��Χ��뾶, Size: 1, Type: FLOAT
	UNIT_FIELD_COMBATREACH				= OBJECT_END + 0x0091,	// ս���Ӵ�����, Size: 1, Type: FLOAT
	UNIT_FIELD_DISPLAYID				= OBJECT_END + 0x0092,	// ��ǰ����ʾģ��ID, Size: 1, Type: INT
	UNIT_FIELD_NATIVEDISPLAYID			= OBJECT_END + 0x0093,	// ԭʼ����ʾģ��ID, Size: 1, Type: INT

	UNIT_FIELD_BYTES_1					= OBJECT_END + 0x0099,	// ***TODO***, Size: 1, Type: BYTES
	UNIT_CHANNEL_SPELL					= OBJECT_END + 0x009F,	// ***TODO*** ͨ���༼�����, Size: 1, Type: BYTES
	UNIT_MOD_CAST_SPEED					= OBJECT_END + 0x00A0,	// ʩ���ٶ�, Size: 1, Type: FLOAT

	UNIT_FIELD_STAT0					= OBJECT_END + 0x00A5,	// 5��������� ��, Size: 1, Type: INT
	UNIT_FIELD_STAT1					= OBJECT_END + 0x00A6,	//
	UNIT_FIELD_STAT2					= OBJECT_END + 0x00A7,	//
	UNIT_FIELD_STAT3					= OBJECT_END + 0x00A8,	//
	UNIT_FIELD_STAT4					= OBJECT_END + 0x00A9,	//

	UNIT_FIELD_BASE_MANA				= OBJECT_END + 0x00CA,	// ***TODO*** ��������ֵ, Size: 1, Type: INT
	UNIT_FIELD_BASE_HEALTH				= OBJECT_END + 0x00CA,	// ***TODO*** ��������ֵ, Size: 1, Type: INT
	UNIT_FIELD_BYTES_2					= OBJECT_END + 0x00CB,	// ***TODO***, Size: 1, Type: BYTES

	UNIT_END							= OBJECT_END + 0x00E2,
};

enum EPlayerFields
{
	PLAYER_BYTES						= UNIT_END + 0x0005,	// ��ɫ�����Ϣ, Size: 1, Type: BYTES
	PLAYER_BYTES_2						= UNIT_END + 0x0006,	// ��ɫ�����Ϣ, Size: 1, Type: BYTES
	PLAYER_BYTES_3						= UNIT_END + 0x0007,	// ��ɫ�Ա�, Size: 1, Type: BYTES

	PLAYER_NEXT_LEVEL_XP				= UNIT_END + 0x0277,	// ����һ�����辭��

	PLAYER_FIELD_WATCHED_FACTION_INDEX	= UNIT_END + 0x0485,	// ***TODO***, Size: 1, Type: INT

	PLAYER_FIELD_MAX_LEVEL				= UNIT_END + 0x04B4,	// ��ɫ��ߵȼ�

	PLAYER_END							= UNIT_END + 0x04C0,
};

enum EGameObjectFields
{
	GAMEOBJECT_END						= OBJECT_END + 0x0014,
};

enum EDynamicObjectFields
{
	DYNAMICOBJECT_END					= OBJECT_END + 0x000A,
};

enum ECorpseFields
{
	CORPSE_END							= OBJECT_END + 0x0020,
};
