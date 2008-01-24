
// *-----------------------------------------------------------------
//
// ������ض���
//
// *-----------------------------------------------------------------

#pragma once


// *-----------------------------------------------------------------
// GUID��ض���

enum HighGuid
{
	HIGHGUID_PLAYER         = 0x00000000,		// ���
};

#define GUID_HIPART(x)   (u_int)(u_int64(x) >> 32)
#define GUID_LOPART(x)   (u_int)(u_int64(x) & 0xFFFFFFFFULL)
#define MAKE_GUID(l, h)  u_int64( u_int(l) | ( u_int64(h) << 32 ) )



// *-----------------------------------------------------------------
// ***TODO*** : ���Ӧ�÷�������?

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
