
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
#include "WMOModelHeaders.h"
#include "WMOGroupImpl.h"

extern TCHAR* GetString(int id);

class WMOImporter : public SceneImport
{
	friend class WMOGroupImpl;

public:
	WMOImporter();
	virtual ~WMOImporter();

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
	void importPortals();

	Texmap* createTexture(const char* fileName);

	StdMat2* createMaterial();

	void createPlane(const WMOPortalVertex& portalVertex);

private:

	Interface* m_maxInterface;
	ImpInterface* m_impInterface;

	// Group�б�
	typedef std::vector<WMOGroupImpl*> GroupList;
	GroupList m_groupList;

	// Material�б�
	typedef std::vector<StdMat2*> MaterialList;
	MaterialList m_materialList;

	// Portal Vertex�б�
	typedef std::vector<WMOPortalVertex> PortalVertexList;
	PortalVertexList m_portalVertexList;

	// Portal ���ϵ�б�
	typedef std::vector<WMOPortalRelation> PortalRelations;
	PortalRelations m_portalRelations;

	std::string m_modelName;
	std::string m_modelPath;
	std::ofstream m_logStream;

	unsigned int m_wmoFileLength;
	unsigned char* m_wmoFileData;	// �ļ���������

	WMORootHeader* m_wmoRootHeader;
	const char* m_textureData;
	WMOMaterial* m_wmoMatData;
	const char* m_groupNameData;

	// ����ֵ����
	unsigned short m_indexCount;
};
