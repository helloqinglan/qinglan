
#include "PluginsDefine.h"
#include "WMOImport.h"

using namespace std;


void flipcc(char *fcc)
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
void WMOImporter::importGeomObject()
{
	unsigned int readLen = 0;
	char fourcc[5];
	unsigned int chunkSize = 0;

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

			m_logStream << "Root Header info:" << endl;
			m_logStream << "	Texture Count: " << m_wmoRootHeader->nTextures << endl;
			m_logStream << "	Groups Count: " << m_wmoRootHeader->nGroups << endl;
			m_logStream << "	Portal Count: " << m_wmoRootHeader->nPortals << endl;
		}
		else if (!strcmp(fourcc, "MOTX"))
		{
			// 贴图
			m_textureData = (char*)(m_wmoFileData + readLen);
		}
		else if (!strcmp(fourcc, "MOMT"))
		{
			// 材质数据
			m_wmoMatData = (WMOMaterial*)(m_wmoFileData + readLen);

			// 记录下该WMO文件用到的原始贴图文件名
			std::ofstream texOrigName("OrigTexNames.txt", std::ios::out | std::ios::trunc);

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
					m_logStream << "Load texture error. filename: " << fileName << endl;
				}
				else
				{
					++slashPos;
					std::string fixName = m_modelPath + "texture\\";
					fixName += fileName.substr(slashPos, dotPos - slashPos);
					fixName.append(".tga");

					StdMat2* material = createMaterial();

					// 根据混合属性决定加在第几层
					material->SetSubTexmap(ID_DI, createTexture(fixName.c_str()));
					material->SetTexmapAmt(ID_DI, 100.0f, 0);
					material->EnableMap(ID_DI, TRUE);
					//material->SetTwoSided(TRUE);		// 双面 设置了此标志的才打开

					m_maxInterface->GetMaterialLibrary().Add(material);
					m_materialList.push_back(material);
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
				WMOGroupImpl* groupImpl = new WMOGroupImpl(i, groupInfo[i], this);
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
}

StdMat2* WMOImporter::createMaterial()
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

Texmap* WMOImporter::createTexture(const char* fileName)
{
	BitmapManager* bmpMgr = TheManager;

	// 改成用系统的检查文件是否存在的API
	ifstream testStream(fileName, ios::binary | ios::in);
	if (testStream.fail())
	{
		m_logStream << "Load texture error. filename: " << fileName << endl;
	}
	else
		testStream.close();

	if (bmpMgr->CanImport(fileName))
	{
		BitmapTex* bmpTex = NewDefaultBitmapTex();
		bmpTex->SetName(fileName);
		bmpTex->SetMapName((TCHAR*)fileName);
		bmpTex->SetAlphaAsMono(TRUE);
		bmpTex->SetAlphaSource(ALPHA_FILE);

		return bmpTex;
	}

	return 0;
}
