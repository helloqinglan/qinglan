
#include "PluginsDefine.h"
#include "M2Import.h"

using namespace std;


// 1. 加载模型顶点数据
void M2Importer::ImportGeomObject()
{
	// Group Header Node
	INode* groupHeadNode = CreateDummyNode();
	groupHeadNode->SetGroupHead(TRUE);
	groupHeadNode->SetGroupMember(FALSE);

	if (m_modelHeader->nameLength > 1)
	{
		TCHAR* modelName = (TCHAR*)(m_m2FileData + m_modelHeader->nameOfs);
		groupHeadNode->SetName(modelName);
		m_logStream << "ModelName: " << modelName << endl;
	}
	else
		groupHeadNode->SetName("GeomGroup");


	// Geoset
	// 一个Geoset构造一个Node, 并且加入到组中

	unsigned short* verDataIndex = (unsigned short*)(m_m2FileData + m_modelView->ofsIndex);
	unsigned short* triData = (unsigned short*)(m_m2FileData + m_modelView->ofsTris);

	m_geosetNodeList.reserve(m_modelView->nSub);
	m_materialList.reserve(m_modelView->nSub);
	for (unsigned int i = 0; i < m_modelView->nSub; ++i)
		m_materialList.push_back(0);

	for (unsigned int i = 0; i < m_modelView->nSub; ++i)
	{
		ModelGeoset& geosetData = m_modelGeoset[i];

		// Triangle Mesh Object
		// 基本的三角形模型对象
		TriObject* triObject = CreateNewTriObject();

		// 创建Node, 并且设为Group Header Node的子节点
		ImpNode* tmpImpNode = m_impInterface->CreateNode();
		tmpImpNode->Reference(triObject);
		tmpImpNode->SetPivot(*(Point3*)&(geosetData.v));

		m_impInterface->AddNodeToScene(tmpImpNode);
		INode* realINode = tmpImpNode->GetINode();
		realINode->SetGroupHead(FALSE);
		realINode->SetGroupMember(TRUE);
		groupHeadNode->AttachChild(realINode);

		m_geosetNodeList.push_back(realINode);

		TCHAR nodeName[256];
		sprintf(nodeName, "GeosetPart_%d", i);
		realINode->SetName(nodeName);

		// mesh
		Mesh& mesh = triObject->GetMesh();
		mesh.setNumVerts(geosetData.vcount);
		mesh.setNumTVerts(geosetData.vcount, TRUE);

		unsigned int triangeCount = geosetData.icount / 3;
		mesh.setNumFaces(triangeCount);
		mesh.setNumTVFaces(triangeCount);

		m_logStream << "Model Geoset " << i << " Vertex Count: " << geosetData.vcount << endl;
		m_logStream << "Model Geoset " << i << " Index Count: " << triangeCount << endl;

		// 顶点坐标和UV
		for (unsigned int i = 0; i < geosetData.vcount; ++i)
		{
			ModelVertex& vertexData = m_globalVertices[ verDataIndex[geosetData.vstart + i] ];

			mesh.verts[i] = *(Point3*)(&vertexData.pos);

			// UV坐标反转
			mesh.tVerts[i].x = vertexData.texcoords.x;
			mesh.tVerts[i].y = 1.0f - vertexData.texcoords.y;
		}

		// 三角形
		for (unsigned int i = 0; i < triangeCount; ++i)
		{
			Face& face = mesh.faces[i];
			face.setVerts(triData[geosetData.istart + i*3] - m_indexCount, 
						  triData[geosetData.istart + i*3+1] - m_indexCount, 
						  triData[geosetData.istart + i*3+2] - m_indexCount);
			face.Show();
			face.setEdgeVisFlags(EDGE_VIS, EDGE_VIS, EDGE_VIS);

			TVFace& tface = mesh.tvFace[i];
			tface.setTVerts(triData[geosetData.istart + i*3] - m_indexCount, 
							triData[geosetData.istart + i*3+1] - m_indexCount, 
							triData[geosetData.istart + i*3+2] - m_indexCount);
		}

		// 法线
		mesh.SpecifyNormals();
		MeshNormalSpec *specNorms = mesh.GetSpecifiedNormals();
		if (specNorms)
		{
			specNorms->ClearAndFree();
			specNorms->SetNumFaces(triangeCount);
			specNorms->SetNumNormals(geosetData.vcount);

			Point3* norms = specNorms->GetNormalArray();
			for (unsigned int i = 0; i < geosetData.vcount; ++i)
			{
				ModelVertex& vertexData = m_globalVertices[ verDataIndex[geosetData.vstart + i] ];
				norms[i] = *(Point3*)(&vertexData.normal);
			}

			MeshNormalFace* pFaces = specNorms->GetFaceArray();
			for (unsigned int i = 0; i < triangeCount; ++i)
			{
				pFaces[i].SpecifyNormalID(0, triData[geosetData.istart + i*3] - m_indexCount);
				pFaces[i].SpecifyNormalID(1, triData[geosetData.istart + i*3+1] - m_indexCount);
				pFaces[i].SpecifyNormalID(2, triData[geosetData.istart + i*3+2] - m_indexCount);
			}

			specNorms->SetAllExplicit(true);
			specNorms->CheckNormals();
		}

		// 删除重复的和无效的面
		mesh.RemoveDegenerateFaces();
		mesh.RemoveIllegalFaces();

		realINode->BackCull(FALSE);			// 取消背面裁减 不是所有的Node都要取消背面裁减
		realINode->EvalWorldState(0);

		// 索引值修正
		m_indexCount += geosetData.vcount;
	}

	// 加载材质
	unsigned short* texLookupData = (unsigned short*)(m_m2FileData + m_modelHeader->ofsTexLookup);
	ModelTextureDef* texUnitDefData = (ModelTextureDef*)(m_m2FileData + m_modelHeader->ofsTextures);
	ModelTexUnit* texUnitData = (ModelTexUnit*)(m_m2FileData + m_modelView->ofsTex);

	for (unsigned int i = 0; i < m_modelView->nTex; ++i)
	{
		ModelTexUnit& texUnit = texUnitData[i];
		unsigned short textureID = texLookupData[texUnit.textureid];
		ModelTextureDef& texDef = texUnitDefData[textureID];

		string textureName;

		if (texDef.type == 0)
			textureName = (LPCSTR)(m_m2FileData + texDef.nameOfs);
		else
			textureName = GetReplacableTexture(texDef.type);

		StdMat2* material = m_materialList[texUnit.op];
		if (!material)
			material = CreateMaterial();

		// 根据混合属性决定加在第几层
		material->SetSubTexmap(ID_DI, CreateTexture(textureName.c_str()));
		material->SetTexmapAmt(ID_DI, 0.0f, 0);
		material->EnableMap(ID_DI, TRUE);
		material->SetTwoSided(TRUE);		// 双面 设置了此标志的才打开

		m_maxInterface->GetMaterialLibrary().Add(material);
		m_geosetNodeList[texUnit.op]->SetMtl(material);
	}

	m_maxInterface->RedrawViews(m_maxInterface->GetTime());
}

string M2Importer::GetReplacableTexture(int id)
{
	// 生物最多有三张可替换贴图 type分别为11 12 13
	// 角色最多有五张可替换贴图 type分别为1 6 2 8 11
	// 角色的11只有Goblin有 现在遇到11都当成是生物来处理

	size_t pathSlashPos = m_modelName.find_last_of('\\');
	string pathName = m_modelName.substr(0, pathSlashPos + 1);

	// 如果模型有可替换贴图 则在模型同一目录下应该有同名的贴图定义文件

	string texFileName = m_modelName;

	size_t dotPos = texFileName.find_last_of('.');
	if (dotPos != string::npos)
		texFileName = texFileName.substr(0, dotPos);

	texFileName.append(".txt");

	ifstream texDefFile(texFileName.c_str());
	if (texDefFile.fail())
	{
		m_logStream << "Texture Define File Name: " << texFileName << " Not exist." << endl;
		return 0;
	}

	vector<string> nameList;

	char lineData[512];
	while(!texDefFile.fail() && !texDefFile.eof())
	{
		texDefFile.getline(lineData, 512);
		nameList.push_back(lineData);
	}

	string retTexName;

	switch (id)
	{
	case 11:
	case 12:
	case 13:
		{
			id -= 11;
			if (id < nameList.size())
				retTexName = nameList[id];
		}
		break;
	}

	m_logStream << "Get Replacable Texture. File Name: " << retTexName << endl;

	return retTexName;
}

StdMat2* M2Importer::CreateMaterial()
{
	static int i = 1;

	TCHAR matName[128];
	sprintf(matName, "%02d - Default", i++);

	StdMat2* material = NewDefaultStdMat();
	material->SetName(matName);

	material->SetAmbient(Color(1.0f, 1.0f, 1.0f), 0);
	material->SetDiffuse(Color(1.0f, 1.0f, 1.0f), 0);
	material->SetSpecular(Color(1.0f, 1.0f, 1.0f), 0);
	material->SetShininess(0.5f, 0);
	material->SetShinStr(0.7f, 0);

	material->SetMtlFlag(MTL_DISPLAY_ENABLE_FLAGS, TRUE);

	return material;
}

Texmap* M2Importer::CreateTexture(LPCTSTR fileName)
{
	BitmapManager* bmpMgr = TheManager;

	size_t pathSlashPos = m_modelName.find_last_of('\\');
	string pathName = m_modelName.substr(0, pathSlashPos + 1);

	// 将贴图文件名改为当前目录下的png文件

	string origFileName = fileName;

	size_t dotPos = origFileName.find_last_of('.');
	if (dotPos != string::npos)
		origFileName = origFileName.substr(0, dotPos);

	size_t slashPos = origFileName.find_last_of('\\');
	if (slashPos != string::npos)
	{
		++slashPos;
		origFileName = origFileName.substr(slashPos, origFileName.length() - slashPos);
	}

	pathName += origFileName;
	pathName.append(".png");

	m_logStream << "Model Texture Name: " << pathName << endl;

	TSTR newFileName = pathName.c_str();
	if (bmpMgr->CanImport(newFileName))
	{
		BitmapTex* bmpTex = NewDefaultBitmapTex();
		bmpTex->SetName(newFileName);
		bmpTex->SetMapName(newFileName);
		bmpTex->SetAlphaAsMono(TRUE);
		bmpTex->SetAlphaSource(ALPHA_FILE);

		m_logStream << "Set Texture OK." << endl;

		return bmpTex;
	}

	return 0;
}

INode* M2Importer::CreateDummyNode()
{
	DummyObject* obj = (DummyObject*)CreateInstance(HELPER_CLASS_ID, Class_ID(DUMMY_CLASS_ID, 0));

	ImpNode* node = m_impInterface->CreateNode();
	node->Reference(obj);
	m_impInterface->AddNodeToScene(node);

	return node->GetINode();
}
