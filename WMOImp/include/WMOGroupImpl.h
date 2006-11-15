
#pragma once

#include "WMOModelHeaders.h"
#include <string>

class WMOGroupImpl
{
public:
	WMOGroupImpl(unsigned int num, WMOGroupInfo& info, const char* nameData);
	~WMOGroupImpl();

private:
	unsigned int m_number;
	std::string m_wmoGroupName;
	Vec3D m_boundingBox[2];
};
