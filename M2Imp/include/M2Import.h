
#pragma once

#include <max.h>
#include <MeshNormalSpec.h>
#include <stdmat.h>
#include <bmmlib.h>
#include <bitmap.h>
#include <dummy.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "M2ModelHeaders.h"

extern TCHAR* GetString(int id);

class M2Importer : public SceneImport
{
public:
	M2Importer();
	virtual ~M2Importer();

	// -----------------------------------------
	// SceneImport �Ľӿ�ʵ��

	int ExtCount();
	const TCHAR* Ext(int i);

	const TCHAR* LongDesc();
	const TCHAR* ShortDesc();

	const TCHAR* AuthorName();
	const TCHAR* CopyrightMessage();
	const TCHAR* OtherMessage1();
	const TCHAR* OtherMessage2();

	unsigned int Version();

	void ShowAbout(HWND hWnd);

	int DoImport(const TCHAR *name,ImpInterface *ii,Interface *i, BOOL suppressPrompts);

private:

	// ---------------------------------------------
	// ����ļ���ʵ��

	void importGeomObject();
	void importBoneObject();

	Texmap* createTexture(LPCTSTR fileName);

	StdMat2* createMaterial();

	// ���ı������ļ��л�ȡ���滻�����ļ���
	std::string getReplacableTexture(int id);

	INode* createGroupHeaderNode();

	// �������ԵĶ���������
	Control* createPositionController();
	Control* createRotationController();
	Control* createScaleController();

private:

	Interface* m_maxInterface;
	ImpInterface* m_impInterface;

	// Geoset�б�
	typedef std::vector<INode*> GeosetNodeList;
	GeosetNodeList m_geosetNodeList;

	// Material�б�
	typedef std::vector<StdMat2*> MaterialList;
	MaterialList m_materialList;

	// Bone�б�
	typedef std::vector<INode*> BoneNodeList;
	BoneNodeList m_boneNodeList;

	std::string m_modelName;
	std::ofstream m_logStream;

	unsigned int m_m2FileLength;
	unsigned char* m_m2FileData;	// ģ���ļ���������

	ModelHeader* m_modelHeader;
	ModelView* m_modelView;
	ModelGeoset* m_modelGeoset;
	ModelVertex* m_globalVertices;

	// ����ֵ����
	unsigned short m_indexCount;
};
