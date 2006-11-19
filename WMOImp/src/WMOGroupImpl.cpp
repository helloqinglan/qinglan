
#include "WMOImport.h"
#include "WMOGroupImpl.h"

using namespace std;

extern void flipcc(char *fcc);

WMOGroupImpl::WMOGroupImpl(unsigned int num, WMOGroupInfo& info, WMOImporter* importer)
: m_wmoImporter(importer), m_number(num), m_groupFileData(0), m_groupFileLength(0)
{
	m_flags = info.flags;

	m_v[0] = info.boxA;
	m_v[1] = info.boxB;

	openGroupFile();

	// ����max�ļ�����
	buildMaxData();
}

WMOGroupImpl::~WMOGroupImpl()
{
}

void WMOGroupImpl::openGroupFile()
{
	// Group File Name
	char temp[256];
	strcpy(temp, m_wmoImporter->m_modelName.c_str());
	temp[m_wmoImporter->m_modelName.length() - 4] = 0;

	char fname[256];
	sprintf(fname, "%s_%03d.wmo", temp, m_number);
	m_wmoImporter->m_logStream << "WMO Group Name: " << fname << endl;


	// ���ļ�
	ifstream groupStream(fname, ios::binary | ios::in);
	if (groupStream.fail())
	{
		MessageBox(NULL, "Group file open error.", "TBD: Error.", MB_OK);
		m_wmoImporter->m_logStream << "Group file open error. file name: " << fname << endl;
		return;
	}
	m_wmoImporter->m_logStream << "World Model Object file open ok." << endl;

	groupStream.seekg(0, ios::end);
	m_groupFileLength = (unsigned int)groupStream.tellg();
	groupStream.seekg(0, ios::beg);

	m_groupFileData = new unsigned char[m_groupFileLength];
	groupStream.read((char*)m_groupFileData, m_groupFileLength);
	groupStream.close();


	// ����ǰ0x14�ֽ�
	m_groupHeader = (WMOGroupHeader*)(m_groupFileData + 0x14);

	m_groupName = string(m_wmoImporter->m_groupNameData + m_groupHeader->nameStart);
	m_groupDesc = string(m_wmoImporter->m_groupNameData + m_groupHeader->nameStart2);
	if (!m_groupName.length())
	{
		char gname[256];
		sprintf(gname, "group_%03d", m_number);
		m_groupName = gname;
	}
	m_wmoImporter->m_logStream << "Group Name: " << m_groupName << endl;
	m_wmoImporter->m_logStream << "Group Desc: " << m_groupDesc << endl;

	m_boundingBox[0] = m_groupHeader->boxA;
	m_boundingBox[1] = m_groupHeader->boxB;


	unsigned int readLen = 0x58;	// ������һ��Chunk��
	char fourcc[5];
	unsigned int chunkSize = 0;

	while (readLen < m_groupFileLength)
	{
		strncpy(fourcc, (const char*)(m_groupFileData + readLen), 4);
		readLen += 4;
		flipcc(fourcc);
		fourcc[4] = 0;

		chunkSize = *((unsigned int*)(m_groupFileData + readLen));
		readLen += 4;

		if (!strcmp(fourcc, "MOPY"))
		{
			// �����εĲ�����Ϣ ÿ���������������ֽ�������
			// ÿ�������ζ������в�ͬ�Ĳ��� (��ͼ)
			m_triangleCount = (int)chunkSize / 2;
			m_triangleMaterial = (unsigned short*)(m_groupFileData + readLen);
		}
		else if (!strcmp(fourcc, "MOVI"))
		{
			// ������������
			m_indexData = (unsigned short*)(m_groupFileData + readLen);
		}
		else if (!strcmp(fourcc, "MOVT"))
		{
			// ��������
			m_vertexCount = (int)chunkSize / sizeof(Vec3D);
			m_vertexData = (Vec3D*)(m_groupFileData + readLen);
		}
		else if (!strcmp(fourcc, "MONR"))
		{
			// ��������
			m_normalData = (Vec3D*)(m_groupFileData + readLen);
		}
		else if (!strcmp(fourcc, "MOTV"))
		{
			// ��������
			m_textureCoords = (Vec2D*)(m_groupFileData + readLen);
		}
		else if (!strcmp(fourcc, "MOBA"))
		{
			// Render batches.
			m_batchCount = (int)chunkSize / sizeof(WMORenderBatch);
			m_batchData = (WMORenderBatch*)(m_groupFileData + readLen);
		}
		else if (!strcmp(fourcc, "MOLR"))
		{
			// �ƹ�
		}
		else if (!strcmp(fourcc, "MODR"))
		{
			// Doodad Sets
		}
		else if (!strcmp(fourcc, "MOBN"))
		{
			// BSP Node��Ϣ
		}
		else if (!strcmp(fourcc, "MOBR"))
		{
			// BSP�������� ����BSP Node�еļ���ƽ��
		}
		else if (!strcmp(fourcc, "MOCV"))
		{
			// ������ɫ
		}
		else if (!strcmp(fourcc, "MLIQ"))
		{
			// ˮλ��Ϣ
		}

		readLen += chunkSize;
	}
}

void WMOGroupImpl::buildMaxData()
{
	// Group Header Node
	INode* groupHeadNode = createGroupHeaderNode();
	groupHeadNode->SetGroupHead(TRUE);
	groupHeadNode->SetGroupMember(FALSE);

	// Geoset
	// һ��Render Batch����һ��Node, ���Ҽ��뵽����

	for (int i = 0; i < m_batchCount; ++i)
	{
		WMORenderBatch& renderBatch = m_batchData[i];

		m_wmoImporter->m_logStream << "Model Geoset " << i << " Vertex Info: " 
			<< renderBatch.vertexStart << " -- " << renderBatch.vertexEnd << endl;
		m_wmoImporter->m_logStream << "Model Geoset " << i << " Index Info: " 
			<< renderBatch.indexStart << " -- " << renderBatch.indexCount << endl;

		// Triangle Mesh Object
		// ������������ģ�Ͷ���
		TriObject* triObject = CreateNewTriObject();

		// ����Node, ������ΪGroup Header Node���ӽڵ�
		ImpNode* tmpImpNode = m_wmoImporter->m_impInterface->CreateNode();
		tmpImpNode->Reference(triObject);

		m_wmoImporter->m_impInterface->AddNodeToScene(tmpImpNode);
		INode* realINode = tmpImpNode->GetINode();
		realINode->SetGroupHead(FALSE);
		realINode->SetGroupMember(TRUE);
		groupHeadNode->AttachChild(realINode);

		m_geosetNodeList.push_back(realINode);

		TCHAR nodeName[256];
		sprintf(nodeName, "%s_part_%03d", m_groupName.c_str(), i);
		realINode->SetName(nodeName);

		// mesh
		unsigned short vertexCount = renderBatch.vertexEnd - renderBatch.vertexStart + 1;
		unsigned int trigangleCount = renderBatch.indexCount / 3;

		Mesh& mesh = triObject->GetMesh();
		mesh.setNumVerts(vertexCount);
		mesh.setNumTVerts(vertexCount, TRUE);

		mesh.setNumFaces(trigangleCount);
		mesh.setNumTVFaces(trigangleCount);

		// ���������UV
		for (int i = 0; i < vertexCount; ++i)
		{
			mesh.verts[i] = *(Point3*)(&(m_vertexData[renderBatch.vertexStart + i]));

			// UV���귴ת
			mesh.tVerts[i].x = m_textureCoords[renderBatch.vertexStart + i].x;
			mesh.tVerts[i].y = 1.0f - m_textureCoords[renderBatch.vertexStart + i].y;
		}

		// ������
		for (unsigned int i = 0; i < trigangleCount; ++i)
		{
			Face& face = mesh.faces[i];
			int index1 = m_indexData[renderBatch.indexStart + i*3] - renderBatch.vertexStart;
			int index2 = m_indexData[renderBatch.indexStart + i*3+1] - renderBatch.vertexStart;
			int index3 = m_indexData[renderBatch.indexStart + i*3+2] - renderBatch.vertexStart;

			face.setVerts(index1, index2, index3);
			face.Show();
			face.setEdgeVisFlags(EDGE_VIS, EDGE_VIS, EDGE_VIS);

			TVFace& tface = mesh.tvFace[i];
			tface.setTVerts(index1, index2, index3);
		}

		// ����
		mesh.SpecifyNormals();
		MeshNormalSpec *specNorms = mesh.GetSpecifiedNormals();
		if (specNorms)
		{
			specNorms->ClearAndFree();
			specNorms->SetNumFaces(trigangleCount);
			specNorms->SetNumNormals(vertexCount);

			Point3* norms = specNorms->GetNormalArray();
			for (int i = 0; i < vertexCount; ++i)
			{
				norms[i] = *(Point3*)(&m_normalData[renderBatch.vertexStart + i]);
			}

			MeshNormalFace* pFaces = specNorms->GetFaceArray();
			for (unsigned int i = 0; i < trigangleCount; ++i)
			{
				int index1 = m_indexData[renderBatch.indexStart + i*3] - renderBatch.vertexStart;
				int index2 = m_indexData[renderBatch.indexStart + i*3+1] - renderBatch.vertexStart;
				int index3 = m_indexData[renderBatch.indexStart + i*3+2] - renderBatch.vertexStart;

				pFaces[i].SpecifyNormalID(0, index1);
				pFaces[i].SpecifyNormalID(1, index2);
				pFaces[i].SpecifyNormalID(2, index3);
			}

			specNorms->SetAllExplicit(true);
			specNorms->CheckNormals();
		}

		// ɾ���ظ��ĺ���Ч����
		mesh.RemoveDegenerateFaces();
		mesh.RemoveIllegalFaces();

		realINode->SetMtl(m_wmoImporter->m_materialList[renderBatch.texture]);
		//realINode->BackCull(FALSE);			// ȡ������ü� �������е�Node��Ҫȡ������ü�
		realINode->EvalWorldState(0);
	}
}

INode* WMOGroupImpl::createGroupHeaderNode()
{
	DummyObject* obj = (DummyObject*)CreateInstance(HELPER_CLASS_ID, Class_ID(DUMMY_CLASS_ID, 0));

	ImpNode* node = m_wmoImporter->m_impInterface->CreateNode();
	node->Reference(obj);

	node->SetName(m_groupName.c_str());

	m_wmoImporter->m_impInterface->AddNodeToScene(node);

	return node->GetINode();
}
