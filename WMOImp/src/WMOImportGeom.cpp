
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


// 1. ����ģ�Ͷ�������
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
			// ��ͼ
			m_textureData = (char*)(m_wmoFileData + readLen);
		}
		else if (!strcmp(fourcc, "MOMT"))
		{
			// ��������
			m_wmoMatData = (WMOMaterial*)(m_wmoFileData + readLen);

			// ��¼�¸�WMO�ļ��õ���ԭʼ��ͼ�ļ���
			std::ofstream texOrigName("OrigTexNames.txt", std::ios::out | std::ios::trunc);

			for (unsigned int i = 0; i < m_wmoRootHeader->nTextures; ++i)
			{
				std::string fileName(m_textureData + m_wmoMatData[i].nameStart);
				texOrigName << fileName << std::endl;

				// ��ͼ�ļ�����ŵ�textureĿ¼�£��ұ��뽫blpתΪtga��ʽ
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

					// ���ݻ�����Ծ������ڵڼ���
					material->SetSubTexmap(ID_DI, createTexture(fixName.c_str()));
					material->SetTexmapAmt(ID_DI, 100.0f, 0);
					material->EnableMap(ID_DI, TRUE);
					//material->SetTwoSided(TRUE);		// ˫�� �����˴˱�־�ĲŴ�

					m_maxInterface->GetMaterialLibrary().Add(material);
					m_materialList.push_back(material);
				}
			}

			texOrigName.close();
		}
		else if (!strcmp(fourcc, "MOGN"))
		{
			// WMO Group��
			// �����������ʵûʲô�� ����ֱ������ WMORootName_XXX.wmo XXXΪ���
			m_groupNameData = (char*)(m_wmoFileData + readLen);
		}
		else if (!strcmp(fourcc, "MOGI"))
		{
			// WMO ����Ϣ
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
			// ��պ�
		}
		else if (!strcmp(fourcc, "MOPV"))
		{
			// Portal ������Ϣ
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
			// Portal���ϵ
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
			// �ƹ�
		}
		else if (!strcmp(fourcc, "MODS"))
		{
			// Doodad sets
		}
		else if (!strcmp(fourcc, "MODN"))
		{
			// ģ��
		}
		else if (!strcmp(fourcc, "MODD"))
		{
			// Doodad instances information
			// ģ�͵�λ�ü���ת��Ϣ
		}
		else if (!strcmp(fourcc, "MFOG"))
		{
			// Fog
		}
		else if (!strcmp(fourcc, "MCVP"))
		{
			// Convex Volumn Planes
			// ͹����Ϣ
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

	// �ĳ���ϵͳ�ļ���ļ��Ƿ���ڵ�API
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
