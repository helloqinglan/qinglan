
#include "M2ImpClassDesc.h"
#include "PluginsDefine.h"
#include "M2Import.h"
#include "../res/resource.h"

// global var
M2ImpClassDesc M2ImpDesc;

TCHAR *GetString(int id)
{
	static TCHAR buf[256];
	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
}

int M2ImpClassDesc::IsPublic()
{
	return TRUE;
}

void* M2ImpClassDesc::Create(BOOL loading)
{
	return new M2Importer();
}

const TCHAR* M2ImpClassDesc::ClassName()
{
	return GetString(IDS_PLUGINS_CLASSNAME);
}

SClass_ID M2ImpClassDesc::SuperClassID()
{
	return SCENE_IMPORT_CLASS_ID;
}

Class_ID M2ImpClassDesc::ClassID()
{
	return Class_ID(M2IMP_CLASS_ID1, M2IMP_CLASS_ID2);
}

const TCHAR* M2ImpClassDesc::Category()
{
	return GetString(IDS_PLUGINS_CATEGORY);
}
