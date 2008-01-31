
// *-----------------------------------------------------------------
//
// 对象属性集字段定义
//
// *-----------------------------------------------------------------

#pragma once

enum EObjectFields
{
	OBJECT_FIELD_GUID					= 0x0000,	// 对象GUID, Size: 2, Type: LONG
	OBJECT_FIELD_TYPE					= 0x0002,	// ***TODO***, Size: 1, Type: INT
	OBJECT_FIELD_ENTRY					= 0x0003,	// ***TODO***, Size: 1, Type: INT
	OBJECT_FIELD_SCALE_X				= 0x0004,	// 对象的缩放值, Size: 1, Type: FLOAT
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
	UNIT_FIELD_LEVEL					= OBJECT_END + 0x001C,	// 角色等级, Size: 1, Type: INT

	UNIT_FIELD_BYTES_0					= OBJECT_END + 0x001E,	// 种族、职业、性别、能量类型, Size: 1, Type: INT
	UNIT_FIELD_FLAGS					= OBJECT_END + 0x0028,	// ***TODO***, Size: 1, Type: INT

	UNIT_FIELD_BOUNDINGRADIUS			= OBJECT_END + 0x0090,	// 包围球半径, Size: 1, Type: FLOAT
	UNIT_FIELD_COMBATREACH				= OBJECT_END + 0x0091,	// 战斗接触距离, Size: 1, Type: FLOAT
	UNIT_FIELD_DISPLAYID				= OBJECT_END + 0x0092,	// 当前的显示模型ID, Size: 1, Type: INT
	UNIT_FIELD_NATIVEDISPLAYID			= OBJECT_END + 0x0093,	// 原始的显示模型ID, Size: 1, Type: INT

	UNIT_FIELD_BYTES_1					= OBJECT_END + 0x0099,	// ***TODO***, Size: 1, Type: BYTES
	UNIT_MOD_CAST_SPEED					= OBJECT_END + 0x00A0,	// 施法速度, Size: 1, Type: FLOAT
	UNIT_FIELD_BYTES_2					= OBJECT_END + 0x00CB,	// ***TODO***, Size: 1, Type: BYTES

	UNIT_END							= OBJECT_END + 0x00E2,
};

enum EPlayerFields
{
	PLAYER_BYTES						= UNIT_END + 0x0005,	// 角色外观信息, Size: 1, Type: BYTES
	PLAYER_BYTES_2						= UNIT_END + 0x0006,	// 角色外观信息, Size: 1, Type: BYTES
	PLAYER_BYTES_3						= UNIT_END + 0x0007,	// 角色性别, Size: 1, Type: BYTES

	PLAYER_FIELD_WATCHED_FACTION_INDEX	= UNIT_END + 0x0485,	// ***TODO***, Size: 1, Type: INT

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
