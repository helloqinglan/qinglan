
#include "WMOImpClassDesc.h"
#include "PluginsDefine.h"
#include "WMOImport.h"
#include "../res/resource.h"

// global var
WMOImpClassDesc WMOImpDesc;

TCHAR *GetString(int id)
{
	static TCHAR buf[256];
	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
}

int WMOImpClassDesc::IsPublic()
{
	return TRUE;
}

void* WMOImpClassDesc::Create(BOOL loading)
{
	return new WMOImporter();
}

const TCHAR* WMOImpClassDesc::ClassName()
{
	return GetString(IDS_PLUGINS_CLASSNAME);
}

SClass_ID WMOImpClassDesc::SuperClassID()
{
	return SCENE_IMPORT_CLASS_ID;
}

Class_ID WMOImpClassDesc::ClassID()
{
	return Class_ID(WMOIMP_CLASS_ID1, WMOIMP_CLASS_ID2);
}

const TCHAR* WMOImpClassDesc::Category()
{
	return GetString(IDS_PLUGINS_CATEGORY);
}
