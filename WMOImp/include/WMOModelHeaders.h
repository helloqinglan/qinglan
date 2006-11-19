
#ifndef __WMO_MODEL_HEADERS_H__
#define __WMO_MODEL_HEADERS_H__

#include "M2ModelHeaders.h"

#pragma pack(push,1)

// WMO Rootͷ�ṹ
struct WMORootHeader
{
	uint32 nTextures;
	uint32 nGroups;
	uint32 nPortals;
	uint32 nLights;
	uint32 nModels;
	uint32 nDoodadDefs;
	uint32 nDoodadSets;
	uint32 ambColor;
	uint32 wmoID;
	Vec3D boxA;
	Vec3D boxB;
	uint32 unk1;
};

// ������Ϣ
struct WMOMaterial
{
	uint32 flags;
	uint32 specular;
	uint32 transparent;
	uint32 nameStart;
	uint32 col1;
	uint32 d3;
	uint32 nameEnd;
	uint32 col2;
	uint32 d4;
	float f1;
	float f2;
	int dx[5];
};

// Portal������Ϣ
struct WMOPortalVertex
{
	Vec3D a, b, c, d;
};

// Portal���ϵ
struct WMOPortalRelation
{
	short portal;
	short group;
	short dir;
	short reserved;
};

// Group Info
struct WMOGroupInfo
{
	int flags;
	Vec3D boxA;
	Vec3D boxB;
	int nameOfs;
};

// Group Header
struct WMOGroupHeader
{
	int nameStart;
	int nameStart2;
	int flags;
	Vec3D boxA;
	Vec3D boxB;
	short portalStart;
	short portalCount;
	short batches[4];
	uint8 fogs[4];
	int unk1, id, unk2, unk3;
};

// Render Batch
struct WMORenderBatch 
{
	char bytes[12];
	unsigned int indexStart;		// ��������
	unsigned short indexCount;
	unsigned short vertexStart;		// ��������
	unsigned short vertexEnd;
	unsigned char flags;
	unsigned char texture;			// ��ͼ
};

#pragma pack(pop)

#endif // __WMO_MODEL_HEADERS_H__
