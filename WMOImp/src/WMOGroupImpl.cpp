
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

	// 构造max的几何体
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


	// 打开文件
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


	// 跳过前0x14字节
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


	unsigned int readLen = 0x58;	// 跳到第一个Chunk处
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
			// 三角形的材质信息 每个三角形用两个字节来描述
			// 每个三角形都可以有不同的材质 (贴图)
			m_triangleCount = (int)chunkSize / 2;
			m_triangleMaterial = (unsigned short*)(m_groupFileData + readLen);
		}
		else if (!strcmp(fourcc, "MOVI"))
		{
			// 顶点索引数据
			m_indexData = (unsigned short*)(m_groupFileData + readLen);
		}
		else if (!strcmp(fourcc, "MOVT"))
		{
			// 顶点数据
			m_vertexCount = (int)chunkSize / sizeof(Vec3D);
			m_vertexData = (Vec3D*)(m_groupFileData + readLen);
		}
		else if (!strcmp(fourcc, "MONR"))
		{
			// 法线数据
			m_normalData = (Vec3D*)(m_groupFileData + readLen);
		}
		else if (!strcmp(fourcc, "MOTV"))
		{
			// 纹理坐标
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
			// 灯光
		}
		else if (!strcmp(fourcc, "MODR"))
		{
			// Doodad Sets
		}
		else if (!strcmp(fourcc, "MOBN"))
		{
			// BSP Node信息
		}
		else if (!strcmp(fourcc, "MOBR"))
		{
			// BSP顶点索引 描述BSP Node中的几何平面
		}
		else if (!strcmp(fourcc, "MOCV"))
		{
			// 顶点颜色
		}
		else if (!strcmp(fourcc, "MLIQ"))
		{
			// 水位信息
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
	// 一个Render Batch构造一个Node, 并且加入到组中

	for (int i = 0; i < m_batchCount; ++i)
	{
		WMORenderBatch& renderBatch = m_batchData[i];

		m_wmoImporter->m_logStream << "Model Geoset " << i << " Vertex Info: " 
			<< renderBatch.vertexStart << " -- " << renderBatch.vertexEnd << endl;
		m_wmoImporter->m_logStream << "Model Geoset " << i << " Index Info: " 
			<< renderBatch.indexStart << " -- " << renderBatch.indexCount << endl;

		// Triangle Mesh Object
		// 基本的三角形模型对象
		TriObject* triObject = CreateNewTriObject();

		// 创建Node, 并且设为Group Header Node的子节点
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

		// 顶点坐标和UV
		for (int i = 0; i < vertexCount; ++i)
		{
			mesh.verts[i] = *(Point3*)(&(m_vertexData[renderBatch.vertexStart + i]));

			// UV坐标反转
			mesh.tVerts[i].x = m_textureCoords[renderBatch.vertexStart + i].x;
			mesh.tVerts[i].y = 1.0f - m_textureCoords[renderBatch.vertexStart + i].y;
		}

		// 三角形
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

		// 法线
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

		// 删除重复的和无效的面
		mesh.RemoveDegenerateFaces();
		mesh.RemoveIllegalFaces();

		realINode->SetMtl(m_wmoImporter->m_materialList[renderBatch.texture]);
		//realINode->BackCull(FALSE);			// 取消背面裁减 不是所有的Node都要取消背面裁减
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
