
#pragma once

#include "WMOModelHeaders.h"
#include <string>

class WMOImporter;

class WMOGroupImpl
{
public:
	WMOGroupImpl(unsigned int num, WMOGroupInfo& info, WMOImporter* importer);
	~WMOGroupImpl();

private:
	void openGroupFile();
	void buildMaxData();

	INode* createGroupHeaderNode();

private:
	WMOImporter* m_wmoImporter;

	unsigned int m_number;
	int m_flags;
	Vec3D m_v[2];
	Vec3D m_boundingBox[2];

	unsigned int m_groupFileLength;
	unsigned char* m_groupFileData;			// ģ���ļ���������

	WMOGroupHeader* m_groupHeader;
	std::string m_groupName;
	std::string m_groupDesc;

	int m_triangleCount;					// �����εĲ�����Ϣ
	unsigned short* m_triangleMaterial;

	int m_vertexCount;
	Vec3D* m_vertexData;
	Vec3D* m_normalData;
	Vec2D* m_textureCoords;
	unsigned short* m_indexData;

	int m_batchCount;						// ��Ⱦ�� �����ʷ��������Ⱦ
	WMORenderBatch* m_batchData;

	// Geoset�б�
	typedef std::vector<INode*> GeosetNodeList;
	GeosetNodeList m_geosetNodeList;
};
