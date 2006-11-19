
#include "PluginsDefine.h"
#include "WMOImport.h"
#include "../res/resource.h"

using namespace std;

// ------------------------------------------------------------------
WMOImporter::WMOImporter()
: m_maxInterface(0), m_impInterface(0), m_wmoFileData(0), m_indexCount(0)
{
	m_wmoRootHeader = 0;
	m_textureData = 0;
	m_wmoMatData = 0;
	m_groupNameData = 0;
}

WMOImporter::~WMOImporter()
{
	delete []m_wmoFileData;
}

// ------------------------------------------------------------------
int WMOImporter::ExtCount()
{
	return 1;
}

const TCHAR* WMOImporter::Ext(int i)
{
	switch (i)
	{
	case 0:
		return _T("wmo");
	}

	return _T("");
}

// ------------------------------------------------------------------
const TCHAR* WMOImporter::LongDesc()
{
	return GetString(IDS_PLUGINS_LONGDESC);
}

const TCHAR* WMOImporter::ShortDesc()
{
	return GetString(IDS_PLUGINS_SHORTDESC);
}

// ------------------------------------------------------------------
const TCHAR* WMOImporter::AuthorName()
{
	return GetString(IDS_PLUGINS_AUTHOR);
}

const TCHAR* WMOImporter::CopyrightMessage()
{
	return GetString(IDS_PLUGINS_COPYRIGHT);
}

const TCHAR* WMOImporter::OtherMessage1()
{
	return GetString(IDS_PLUGINS_OTHER1);
}

const TCHAR* WMOImporter::OtherMessage2()
{
	return GetString(IDS_PLUGINS_OTHER2);
}

// ------------------------------------------------------------------
unsigned int WMOImporter::Version()
{
	return WMOIMP_VERSION;
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
void WMOImporter::ShowAbout(HWND hWnd)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX),
		hWnd, AboutBoxDlgProc, 0);
}


// ------------------------------------------------------------------
int WMOImporter::DoImport(const TCHAR *name,ImpInterface *ii,Interface *i, BOOL suppressPrompts)
{
	m_maxInterface = i;
	m_impInterface = ii;

	m_modelName = name;
	size_t slashPos = m_modelName.find_last_of('\\');
	m_modelPath = m_modelName.substr(0, slashPos + 1);

	// 日记日志
	m_logStream.open("wmoimplog.txt", ios::out | ios::trunc);
	if (m_logStream.fail())
	{
		MessageBox(NULL, "Open log stream error.", "TBD: Error.", MB_OK);
		return 1;
	}

	m_logStream << "Start logging..." << endl;
	m_logStream << "Model File: " << name << endl;

	// 打开文件
	ifstream wmoStream(name, ios::binary | ios::in);
	if (wmoStream.fail())
	{
		MessageBox(NULL, "World Model Object file open error.", "TBD: Error.", MB_OK);
		m_logStream << "World Model Object file open error." << endl;
		return 0;
	}
	m_logStream << "World Model Object file open ok." << endl;

	wmoStream.seekg(0, ios::end);
	m_wmoFileLength = (unsigned int)wmoStream.tellg();
	wmoStream.seekg(0, ios::beg);

	m_wmoFileData = new unsigned char[m_wmoFileLength];
	wmoStream.read((char*)m_wmoFileData, m_wmoFileLength);
	wmoStream.close();

	// 创建新场景 暂不实现添加到场景中的功能
	m_impInterface->NewScene();

	// 加载模型顶点数据
	m_logStream << "Import geometry object..." << endl;
	m_logStream.flush();
	importGeomObject();

	// 加载Portal面
	m_logStream << "Import portals..." << endl;
	m_logStream.flush();
	importPortals();

	m_maxInterface->RedrawViews(m_maxInterface->GetTime());

	return 1;
}
