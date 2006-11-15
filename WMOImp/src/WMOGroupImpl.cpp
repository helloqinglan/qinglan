
#include "WMOGroupImpl.h"

WMOGroupImpl::WMOGroupImpl(unsigned int num, WMOGroupInfo& info, const char* nameData)
{
	m_number = num;

	if (info.nameOfs)
		m_wmoGroupName = std::string(nameData + info.nameOfs);

	m_boundingBox[0] = info.boxA;
	m_boundingBox[1] = info.boxB;
}

WMOGroupImpl::~WMOGroupImpl()
{
}
