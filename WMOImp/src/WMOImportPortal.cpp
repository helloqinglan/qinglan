
#include "PluginsDefine.h"
#include "WMOImport.h"

using namespace std;


// 2. 加载Portal面
void WMOImporter::importPortals()
{
	PortalVertexList::const_iterator itr;
	for (itr = m_portalVertexList.begin(); itr != m_portalVertexList.end(); ++itr)
	{
		createPlane(*itr);
	}
}

void WMOImporter::createPlane(const WMOPortalVertex& portalVertex)
{
	// Triangle Mesh Object
	// 基本的三角形模型对象
	TriObject* triObject = CreateNewTriObject();

	// 创建Node
	ImpNode* tmpImpNode = m_impInterface->CreateNode();
	tmpImpNode->Reference(triObject);

	m_impInterface->AddNodeToScene(tmpImpNode);
	INode* realINode = tmpImpNode->GetINode();
	realINode->SetGroupHead(FALSE);
	realINode->SetGroupMember(FALSE);

	static int portal_index = 0;
	TCHAR nodeName[256];
	sprintf(nodeName, "Portal_%d", portal_index++);
	realINode->SetName(nodeName);

	// mesh 一个Portal面由四个顶点, 两个三角形组成
	Mesh& mesh = triObject->GetMesh();
	mesh.setNumVerts(4);
	mesh.setNumFaces(2);

	char verInfoStr[1024];
	sprintf(verInfoStr, "%f,%f,%f  %f,%f,%f  %f,%f,%f  %f,%f,%f",
		portalVertex.a.x, portalVertex.a.y, portalVertex.a.z,
		portalVertex.b.x, portalVertex.b.y, portalVertex.b.z,
		portalVertex.c.x, portalVertex.c.y, portalVertex.c.z,
		portalVertex.d.x, portalVertex.d.y, portalVertex.d.z);
	m_logStream << "Portal " << portal_index << " vertex info: " << verInfoStr << endl;

	// 顶点坐标和UV
	mesh.verts[0] = *(Point3*)(&portalVertex.a);
	mesh.verts[1] = *(Point3*)(&portalVertex.b);
	mesh.verts[2] = *(Point3*)(&portalVertex.c);
	mesh.verts[3] = *(Point3*)(&portalVertex.d);

	// 三角形
	Face& face1 = mesh.faces[0];
	face1.setVerts(0, 1, 3);
	face1.Show();
	face1.setEdgeVisFlags(EDGE_VIS, EDGE_VIS, EDGE_VIS);

	Face& face2 = mesh.faces[1];
	face2.setVerts(1, 2, 3);
	face2.Show();
	face2.setEdgeVisFlags(EDGE_VIS, EDGE_VIS, EDGE_VIS);

	// 法线
	mesh.buildNormals();

	// 删除重复的和无效的面
	mesh.RemoveDegenerateFaces();
	mesh.RemoveIllegalFaces();

	realINode->BackCull(FALSE);			// 取消背面裁减
	realINode->EvalWorldState(0);
}
