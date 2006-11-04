
#include "PluginsDefine.h"
#include "M2Import.h"
#include "../res/resource.h"

using namespace std;

// ------------------------------------------------------------------
M2Importer::M2Importer()
: m_maxInterface(0), m_impInterface(0), m_m2FileData(0), m_indexCount(0)
{
}

M2Importer::~M2Importer()
{
	delete []m_m2FileData;
}

// ------------------------------------------------------------------
int M2Importer::ExtCount()
{
	return 1;
}

const TCHAR* M2Importer::Ext(int i)
{
	switch (i)
	{
	case 0:
		return _T("m2");
	}

	return _T("");
}

// ------------------------------------------------------------------
const TCHAR* M2Importer::LongDesc()
{
	return GetString(IDS_PLUGINS_LONGDESC);
}

const TCHAR* M2Importer::ShortDesc()
{
	return GetString(IDS_PLUGINS_SHORTDESC);
}

// ------------------------------------------------------------------
const TCHAR* M2Importer::AuthorName()
{
	return GetString(IDS_PLUGINS_AUTHOR);
}

const TCHAR* M2Importer::CopyrightMessage()
{
	return GetString(IDS_PLUGINS_COPYRIGHT);
}

const TCHAR* M2Importer::OtherMessage1()
{
	return GetString(IDS_PLUGINS_OTHER1);
}

const TCHAR* M2Importer::OtherMessage2()
{
	return GetString(IDS_PLUGINS_OTHER2);
}

// ------------------------------------------------------------------
unsigned int M2Importer::Version()
{
	return M2IMP_VERSION;
}

// ------------------------------------------------------------------
static BOOL CALLBACK AboutBoxDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
	case WM_INITDIALOG:
		CenterWindow(hWnd, GetParent(hWnd)); 
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) 
		{
		case IDOK:
			EndDialog(hWnd, 1);
			break;
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
} 
void M2Importer::ShowAbout(HWND hWnd)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX),
		hWnd, AboutBoxDlgProc, 0);
}


// ------------------------------------------------------------------
int M2Importer::DoImport(const TCHAR *name,ImpInterface *ii,Interface *i, BOOL suppressPrompts)
{
	m_maxInterface = i;
	m_impInterface = ii;

	m_modelName = name;

	// 日记日志
	m_logStream.open("m2implog.txt", ios::out | ios::trunc);
	if (m_logStream.fail())
	{
		MessageBox(NULL, "Open log stream error.", "TBD: Error.", MB_OK);
		return 1;
	}

	m_logStream << "Start logging..." << endl;
	m_logStream << "Model File: " << name << endl;

	// 打开文件
	ifstream m2Stream(name, ios::binary | ios::in);
	if (m2Stream.fail())
	{
		MessageBox(NULL, "Model file open error.", "TBD: Error.", MB_OK);
		m_logStream << "Model file open error." << endl;
		return 0;
	}
	m_logStream << "Model file open ok." << endl;

	m2Stream.seekg(0, ios::end);
	m_m2FileLength = (unsigned int)m2Stream.tellg();
	m2Stream.seekg(0, ios::beg);

	m_m2FileData = new unsigned char[m_m2FileLength];
	m2Stream.read((char*)m_m2FileData, m_m2FileLength);
	m2Stream.close();

	m_modelHeader = (ModelHeader*)m_m2FileData;
	if (strncmp(m_modelHeader->id, "MD20", 4) != 0)
	{
		MessageBox(NULL, "Model file is invalid.", "TBD: Error.", MB_OK);
		m_logStream << "Model file is invalid." << endl;
		return 0;
	}

	m_globalVertices = (ModelVertex*)(m_m2FileData + m_modelHeader->ofsVertices);

	// 取m2模型的第一个视图信息
	m_modelView = (ModelView*)(m_m2FileData + m_modelHeader->ofsViews);
	m_modelGeoset = (ModelGeoset*)(m_m2FileData + m_modelView->ofsSub);

	// 创建新场景 暂不实现添加到场景中的功能
	m_impInterface->NewScene();

	// 1. 加载模型顶点数据
	m_logStream << "Import geometry object..." << endl;
	m_logStream.flush();
	ImportGeomObject();

	// 2. 加载骨骼数据
	m_logStream << "Import bone object..." << endl;
	m_logStream.flush();
	ImportBoneObject();

	return 1;
}
