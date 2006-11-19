
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
	// SceneImport 的接口实现

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
	// 具体的加载实现

	void importGeomObject();
	void importBoneObject();

	Texmap* createTexture(LPCTSTR fileName);

	StdMat2* createMaterial();

	// 从文本定义文件中获取可替换纹理文件名
	std::string getReplacableTexture(int id);

	INode* createGroupHeaderNode();

	// 创建线性的动画控制器
	Control* createPositionController();
	Control* createRotationController();
	Control* createScaleController();

private:

	Interface* m_maxInterface;
	ImpInterface* m_impInterface;

	// Geoset列表
	typedef std::vector<INode*> GeosetNodeList;
	GeosetNodeList m_geosetNodeList;

	// Material列表
	typedef std::vector<StdMat2*> MaterialList;
	MaterialList m_materialList;

	// Bone列表
	typedef std::vector<INode*> BoneNodeList;
	BoneNodeList m_boneNodeList;

	std::string m_modelName;
	std::ofstream m_logStream;

	unsigned int m_m2FileLength;
	unsigned char* m_m2FileData;	// 模型文件数据内容

	ModelHeader* m_modelHeader;
	ModelView* m_modelView;
	ModelGeoset* m_modelGeoset;
	ModelVertex* m_globalVertices;

	// 索引值修正
	unsigned short m_indexCount;
};
