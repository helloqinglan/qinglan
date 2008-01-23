
// *-----------------------------------------------------------------
//
// ����GUID��ض���
//
// *-----------------------------------------------------------------

#pragma once

enum HighGuid
{
	HIGHGUID_PLAYER         = 0x00000000,		// ���
};

#define GUID_HIPART(x)   (u_int)(u_int64(x) >> 32)
#define GUID_LOPART(x)   (u_int)(u_int64(x) & 0xFFFFFFFFULL)
#define MAKE_GUID(l, h)  u_int64( u_int(l) | ( u_int64(h) << 32 ) )
