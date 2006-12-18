
#include "WMOImport.h"
#include "WMOGroupImpl.h"

using namespace std;

// 3. ¼ÓÔØLights
void WMOImporter::importLights()
{
	OmniLightsList::const_iterator itr;
	for (itr = m_omniLightsList.begin(); itr != m_omniLightsList.end(); ++itr)
	{
		createWMOLight(*itr);
	}
}

void WMOImporter::createWMOLight(const WMOLights& light)
{
	// ÑÕÉ«
	float fr = ((light.color & 0x00ff0000) >> 16) / 255.0f;
	float fg = ((light.color & 0x0000ff00) >>  8) / 255.0f;
	float fb = ((light.color & 0x000000ff)      ) / 255.0f;

	GenLight *obj = m_maxInterface->CreateLightObject(OMNI_LIGHT);

	obj->Enable(1);
	obj->SetUseLight(1);
	obj->SetIntensity(0, light.intensity);

	obj->SetAffectDiffuse(TRUE);
	obj->SetRGBColor(0, Point3(fr,fg, fb));
	obj->SetAffectSpecular(TRUE);
	obj->SetUseAtten(1);
	obj->SetAtten(0, ATTEN_START, light.r1);
	obj->SetAtten(0, ATTEN_END, light.r2);

	INode* lightNode = m_maxInterface->CreateObjectNode(obj);

	static int index = 0;
	char light_name[256];
	sprintf(light_name, "light_%d", index++);
	lightNode->SetName(light_name);

	lightNode->SetNodeTM(0, TransMatrix(Point3(light.pos.x, light.pos.y, light.pos.z)));
}
