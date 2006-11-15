
#include <windows.h>

#include "PluginsDefine.h"
#include "WMOImport.h"
#include "WMOImpClassDesc.h"
#include "../res/resource.h"


// -------------------------------------------------------------
// Global Vars
HINSTANCE hInstance;
int controlsInit = FALSE;


// -------------------------------------------------------------
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) 
{
	hInstance = hinstDLL; // Hang on to this DLL's instance handle. 
	if (!controlsInit)
	{
		controlsInit = TRUE; // Initialize Max's custom controls 

		// jaguar controls 
		InitCustomControls(hInstance); 

#ifdef OLD3DCONTROLS 
		// initialize 3D controls 
		Ctl3dRegister(hinstDLL); 
		Ctl3dAutoSubclass(hinstDLL); 
#endif 

		InitCommonControls();
	}

	return TRUE; 
}


// -------------------------------------------------------------
PLUGINS_API const TCHAR* LibDescription() 
{
	return GetString(IDS_PLUGINS_CLASSNAME);
}

PLUGINS_API int LibNumberClasses() 
{
	return 1;
}


PLUGINS_API ClassDesc* LibClassDesc(int i) 
{
	switch(i) 
	{
	case 0: 
		return &WMOImpDesc;

	default: 
		return 0;
	}
}

PLUGINS_API ULONG LibVersion() 
{
	return VERSION_3DSMAX;
}
