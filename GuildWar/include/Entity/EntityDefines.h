
// *-----------------------------------------------------------------
//
// 对象相关定义
//
// *-----------------------------------------------------------------

#pragma once


// *-----------------------------------------------------------------
// GUID相关定义

enum HighGuid
{
	HIGHGUID_PLAYER         = 0x00000000,		// 玩家
};

#define GUID_HIPART(x)   (u_int)(u_int64(x) >> 32)
#define GUID_LOPART(x)   (u_int)(u_int64(x) & 0xFFFFFFFFULL)
#define MAKE_GUID(l, h)  u_int64( u_int(l) | ( u_int64(h) << 32 ) )



// *-----------------------------------------------------------------

// 玩家的装备槽位定义
enum EquipmentSlots
{
    EQUIPMENT_SLOT_START		= 0,

    EQUIPMENT_SLOT_HEAD			= 0,
    EQUIPMENT_SLOT_NECK			= 1,
    EQUIPMENT_SLOT_SHOULDERS	= 2,
    EQUIPMENT_SLOT_BODY			= 3,
    EQUIPMENT_SLOT_CHEST		= 4,
    EQUIPMENT_SLOT_WAIST		= 5,
    EQUIPMENT_SLOT_LEGS			= 6,
    EQUIPMENT_SLOT_FEET			= 7,
    EQUIPMENT_SLOT_WRISTS		= 8,
    EQUIPMENT_SLOT_HANDS		= 9,
    EQUIPMENT_SLOT_FINGER1		= 10,
    EQUIPMENT_SLOT_FINGER2		= 11,
    EQUIPMENT_SLOT_TRINKET1		= 12,
    EQUIPMENT_SLOT_TRINKET2		= 13,
    EQUIPMENT_SLOT_BACK			= 14,
    EQUIPMENT_SLOT_MAINHAND		= 15,
    EQUIPMENT_SLOT_OFFHAND		= 16,
    EQUIPMENT_SLOT_RANGED		= 17,
    EQUIPMENT_SLOT_TABARD		= 18,

    EQUIPMENT_SLOT_END			= 19
};


// *-----------------------------------------------------------------

// Unit的Stat类型定义 (5项基本属性)
enum UnitStats
{
	STAT_STRENGTH,
	STAT_AGILITY,
	STAT_STAMINA,
	STAT_INTELLECT,
	STAT_SPIRIT,

	STATS_MAX,
};

// UNIT_FIELD_FLAGS中设置的掩码
enum UnitFlags
{
	UNIT_FLAG_UNKNOWN1	= 0x00000008,
};


// *-----------------------------------------------------------------

// 法术类型定义
enum SpellSchools
{
	SPELL_SCHOOL_NORMAL,
	SPELL_SCHOOL_HOLY,
	SPELL_SCHOOL_FIRE,
	SPELL_SCHOOL_NATURE,
	SPELL_SCHOOL_FROST,
	SPELL_SCHOOL_SHADOW,
	SPELL_SCHOOL_ARCANE,

	MAX_SPELL_SCHOOL,
};
