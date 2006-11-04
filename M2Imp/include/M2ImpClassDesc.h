
#pragma once

#include <max.h>

class M2ImpClassDesc : public ClassDesc
{
public:
	int IsPublic();
	void* Create(BOOL loading = FALSE);
	const TCHAR* ClassName();
	SClass_ID SuperClassID();
	Class_ID ClassID();
	const TCHAR* Category();
};

extern M2ImpClassDesc M2ImpDesc;
