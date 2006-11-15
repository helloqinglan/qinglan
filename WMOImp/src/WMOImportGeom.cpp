
#include "PluginsDefine.h"
#include "WMOImport.h"

using namespace std;


inline void flipcc(char *fcc)
{
	char t;
	t=fcc[0];
	fcc[0]=fcc[3];
	fcc[3]=t;
	t=fcc[1];
	fcc[1]=fcc[2];
	fcc[2]=t;
}


// 1. 加载模型顶点数据
void WMOImporter::ImportGeomObject()
{
	unsigned int readLen = 0;
	char fourcc[5];
	unsigned int chunkSize = 0;
	unsigned int nextChunkPos = 0;

	while (readLen < m_wmoFileLength)
	{
		strncpy(fourcc, (const char*)(m_wmoFileData + readLen), 4);
		readLen += 4;
		flipcc(fourcc);
		fourcc[4] = 0;

		chunkSize = *((unsigned int*)(m_wmoFileData + readLen));
		readLen += 4;

		if (!strcmp(fourcc, "MOHD"))
		{
			m_wmoRootHeader = (WMORootHeader*)(m_wmoFileData + readLen);
		}
		else if (!strcmp(fourcc, "MOTX"))
		{
			// 贴图
			m_textureData = (char*)(m_wmoFileData + readLen);
		}
		else if (!strcmp(fourcc, "MOMT"))
		{
			// 材质
			m_wmoMatData = (WMOMaterial*)(m_wmoFileData + readLen);

			// 记录下该WMO文件用到的原始贴图文件名
			std::ofstream texOrigName("OrigTexNames.txt", std::ios::out | std::ios::trunc);

			m_textureList.reserve(m_wmoRootHeader->nTextures);
			for (unsigned int i = 0; i < m_wmoRootHeader->nTextures; ++i)
			{
				std::string fileName(m_textureData + m_wmoMatData[i].nameStart);
				texOrigName << fileName << std::endl;

				// 贴图文件必须放到texture目录下，且必须将blp转为tga格式
				size_t slashPos = fileName.find_last_of('\\');
				size_t dotPos = fileName.find_last_of('.');

				if (slashPos == std::string::npos ||
					dotPos == std::string::npos ||
					slashPos >= dotPos)
				{
					m_textureList.push_back("");
				}
				else
				{
					++slashPos;
					std::string fixName = m_modelPath +"texture\\";
					fixName += fileName.substr(slashPos, dotPos - slashPos);
					fixName.append(".tga");

					m_textureList.push_back(fixName);
				}
			}

			texOrigName.close();
		}
		else if (!strcmp(fourcc, "MOGN"))
		{
			// WMO Group名
			// 这里的组名其实没什么用 可以直接生成 WMORootName_XXX.wmo XXX为编号
			m_groupNameData = (char*)(m_wmoFileData + readLen);
		}
		else if (!strcmp(fourcc, "MOGI"))
		{
			// WMO 组信息
			m_groupList.reserve(m_wmoRootHeader->nGroups);

			WMOGroupInfo* groupInfo = (WMOGroupInfo*)(m_wmoFileData + readLen);
			for (unsigned int i = 0; i < m_wmoRootHeader->nGroups; ++i)
			{
				WMOGroupImpl* groupImpl = new WMOGroupImpl(i, groupInfo[i], m_groupNameData);
				m_groupList.push_back(groupImpl);
			}
		}
		else if (!strcmp(fourcc, "MOSB"))
		{
			// 天空盒
		}
		else if (!strcmp(fourcc, "MOPV"))
		{
			// Portal 顶点信息
			WMOPortalVertex* portalVData = (WMOPortalVertex*)(m_wmoFileData + readLen);

			for (unsigned int i = 0; i < m_wmoRootHeader->nPortals; ++i)
			{
				m_portalVertexList.push_back(portalVData[i]);
			}
		}
		else if (!strcmp(fourcc, "MOPT"))
		{
			// Portal Information
		}
		else if (!strcmp(fourcc, "MOPR"))
		{
			// Portal组关系
			WMOPortalRelation* portalRData = (WMOPortalRelation*)(m_wmoFileData + readLen);

			for (unsigned int i = 0; i < 2 * m_wmoRootHeader->nPortals; ++i)
			{
				m_portalRelations.push_back(portalRData[i]);
			}
		}
		else if (!strcmp(fourcc, "MOVV"))
		{
			// Visible block vertices
		}
		else if (!strcmp(fourcc, "MOVB"))
		{
			// Visible block list
		}
		else if (!strcmp(fourcc, "MOLT"))
		{
			// 灯光
		}
		else if (!strcmp(fourcc, "MODS"))
		{
			// Doodad sets
		}
		else if (!strcmp(fourcc, "MODN"))
		{
			// 模型
		}
		else if (!strcmp(fourcc, "MODD"))
		{
			// Doodad instances information
			// 模型的位置及旋转信息
		}
		else if (!strcmp(fourcc, "MFOG"))
		{
			// Fog
		}
		else if (!strcmp(fourcc, "MCVP"))
		{
			// Convex Volumn Planes
			// 凸包信息
			// Contains blocks of floating-point numbers
		}

		readLen += chunkSize;
	}
/*
	// Group Header Node
	INode* groupHeadNode = CreateDummyNode();
	groupHeadNode->SetGroupHead(TRUE);
	groupHeadNode->SetGroupMember(FALSE);

	if (m_modelHeader->nameLength > 1)
	{
		TCHAR* modelName = (TCHAR*)(m_wmoFileData + m_modelHeader->nameOfs);
		groupHeadNode->SetName(modelName);
		m_logStream << "ModelName: " << modelName << endl;
	}
	else
		groupHeadNode->SetName("GeomGroup");


	// Geoset
	// 一个Geoset构造一个Node, 并且加入到组中

	unsigned short* verDataIndex = (unsigned short*)(m_wmoFileData + m_modelView->ofsIndex);
	unsigned short* triData = (unsigned short*)(m_wmoFileData + m_modelView->ofsTris);

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
	unsigned short* texLookupData = (unsigned short*)(m_wmoFileData + m_modelHeader->ofsTexLookup);
	ModelTextureDef* texUnitDefData = (ModelTextureDef*)(m_wmoFileData + m_modelHeader->ofsTextures);
	ModelTexUnit* texUnitData = (ModelTexUnit*)(m_wmoFileData + m_modelView->ofsTex);

	for (unsigned int i = 0; i < m_modelView->nTex; ++i)
	{
		ModelTexUnit& texUnit = texUnitData[i];
		unsigned short textureID = texLookupData[texUnit.textureid];
		ModelTextureDef& texDef = texUnitDefData[textureID];

		string textureName;

		if (texDef.type == 0)
			textureName = (LPCSTR)(m_wmoFileData + texDef.nameOfs);
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

	m_maxInterface->RedrawViews(m_maxInterface->GetTime());*/
}

StdMat2* WMOImporter::CreateMaterial()
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

Texmap* WMOImporter::CreateTexture(LPCTSTR fileName)
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

INode* WMOImporter::CreateDummyNode()
{
	DummyObject* obj = (DummyObject*)CreateInstance(HELPER_CLASS_ID, Class_ID(DUMMY_CLASS_ID, 0));

	ImpNode* node = m_impInterface->CreateNode();
	node->Reference(obj);
	m_impInterface->AddNodeToScene(node);

	return node->GetINode();
}
