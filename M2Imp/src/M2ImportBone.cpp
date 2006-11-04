
#include "PluginsDefine.h"
#include "M2Import.h"

using namespace std;

const Class_ID IPOS_CONTROL_CLASS_ID = Class_ID(0x118f7e02,0xffee238a);
enum 
{
	IPOS_X_REF	=	0,
	IPOS_Y_REF	=	1,
	IPOS_Z_REF	=	2,
	IPOS_W_REF	=	3,
};

const float FramesPerSecond = 30.0f;
const float FramesIncrement = 1.0f/30.0f;
const int TicksPerFrame = GetTicksPerFrame();

inline TimeValue TimeToFrame(unsigned int t) 
{
	return TimeValue(t * FramesPerSecond * TicksPerFrame);
}
inline unsigned int FrameToTime(TimeValue t) 
{
	return unsigned int(t) / (FramesPerSecond * TicksPerFrame);
}

// 2. 加载骨骼数据
void M2Importer::ImportBoneObject()
{
	// Bone Group Header Node
	INode* groupHeadNode = CreateDummyNode();
	groupHeadNode->SetGroupHead(TRUE);
	groupHeadNode->SetGroupMember(FALSE);

	if (m_modelHeader->nameLength > 1)
	{
		TCHAR* modelName = (TCHAR*)(m_m2FileData + m_modelHeader->nameOfs);
		TCHAR boneGroupName[256];
		sprintf(boneGroupName, "%s_bone", modelName);
		groupHeadNode->SetName(boneGroupName);
	}
	else
		groupHeadNode->SetName("BoneGroup");


	// Bone
	// 一个Bone构造一个Node, 并且加入到组中

	ModelBoneDef* boneData = (ModelBoneDef*)(m_m2FileData + m_modelHeader->ofsBones);

	m_boneNodeList.reserve(m_modelHeader->nBones);
	for (unsigned int i = 0; i < m_modelHeader->nBones; ++i)
	{
		ModelBoneDef& boneDef = boneData[i];

		// create bone node
		HelperObject* obj = (HelperObject*)CreateInstance(HELPER_CLASS_ID, Class_ID(BONE_CLASS_ID, 0));

		ImpNode* node = m_impInterface->CreateNode();

		TCHAR boneName[256];
		sprintf(boneName, "bone_%02d", i);
		node->SetName(boneName);

		node->SetPivot(*(Point3*)&(boneDef.pivot));
		node->Reference(obj);

		m_impInterface->AddNodeToScene(node);

		// 设置变换矩阵
		Matrix3 tm;
		tm.IdentityMatrix();
		tm.SetTrans(*(Point3*)&(boneDef.pivot));
		node->SetTransform(0, tm);

		// 添加到组
		INode* realINode = node->GetINode();
		realINode->SetGroupHead(FALSE);
		realINode->SetGroupMember(TRUE);
		groupHeadNode->AttachChild(realINode);

		// 设置Bone父子关系
		realINode->ShowBone(2);
		m_boneNodeList.push_back(realINode);
		if (boneDef.parent != -1)
		{
			INode* parentNode = m_boneNodeList[boneDef.parent];
			parentNode->AttachChild(realINode);
		}

		realINode->EvalWorldState(0);
	}

	// 导入每根骨骼的关键桢数据
	for (unsigned int i = 0; i < m_modelHeader->nBones; ++i)
	{
		ModelBoneDef& boneDef = boneData[i];
		INode* realINode = m_boneNodeList[i];

		Control* tmControl = realINode->GetTMController();

		// Position
		if (boneDef.translation.nKeys)
		{
			// 设置动画控制器为线性控制器
			Control* posControl = CreatePositionController();
			tmControl->SetPositionController(posControl);

			unsigned int* timeData = (unsigned int*)(m_m2FileData + boneDef.translation.ofsTimes);
			Point3* keyData = (Point3*)(m_m2FileData + boneDef.translation.ofsKeys);

			// 设置动画时间范围
			bool animRangeChanged = false;
			Interval animRange = m_maxInterface->GetAnimRange();
			for (unsigned int j = 0; j < boneDef.translation.nKeys; ++j)
			{
				if (timeData[j] < animRange.Start())
				{
					animRange.SetStart(timeData[j]);
					animRangeChanged = true;
				}
				else if (timeData[j] > animRange.End())
				{
					animRange.SetEnd(timeData[j]);
					animRangeChanged = true;
				}
			}
			if (animRangeChanged)
				m_maxInterface->SetAnimRange(animRange);

			// 设置动画关键桢数据
			Control* xControl = posControl->GetXController();
			IKeyControl* xKeyControl = GetKeyControlInterface(xControl);
			xKeyControl->SetNumKeys(boneDef.translation.nKeys);

			Control* yControl = posControl->GetYController();
			IKeyControl* yKeyControl = GetKeyControlInterface(yControl);
			yKeyControl->SetNumKeys(boneDef.translation.nKeys);

			Control* zControl = posControl->GetZController();
			IKeyControl* zKeyControl = GetKeyControlInterface(zControl);
			zKeyControl->SetNumKeys(boneDef.translation.nKeys);

			for (unsigned int j = 0; j < boneDef.translation.nKeys; ++j)
			{
				// X
				AnyKey bufX;
				ILinFloatKey* keyX = reinterpret_cast<ILinFloatKey*>((IKey*)bufX);
				keyX->time = timeData[j];
				keyX->val = keyData[j].x;
				xKeyControl->AppendKey(keyX);

				// Y
				AnyKey bufY;
				ILinFloatKey* keyY = reinterpret_cast<ILinFloatKey*>((IKey*)bufY);
				keyY->time = timeData[j];
				keyY->val = keyData[j].y;
				yKeyControl->AppendKey(keyY);

				// Z
				AnyKey bufZ;
				ILinFloatKey* keyZ = reinterpret_cast<ILinFloatKey*>((IKey*)bufZ);
				keyZ->time = timeData[j];
				keyZ->val = keyData[j].z;
				zKeyControl->AppendKey(keyZ);
			}
		}
/*
		// Rotation
		if (boneDef.rotation.nKeys)
		{
			Control* rotControl = CreateRotationController();
			tmControl->SetRotationController(rotControl);

			unsigned int* timeData = (unsigned int*)(m_m2FileData + boneDef.rotation.ofsTimes);
			Quat* keyData = (Quat*)(m_m2FileData + boneDef.rotation.ofsKeys);

			// 设置动画时间范围
			bool animRangeChanged = false;
			Interval animRange = m_maxInterface->GetAnimRange();
			for (unsigned int j = 0; j < boneDef.rotation.nKeys; ++j)
			{
				if (timeData[j] < animRange.Start())
				{
					animRange.SetStart(timeData[j]);
					animRangeChanged = true;
				}
				else if (timeData[j] > animRange.End())
				{
					animRange.SetEnd(timeData[j]);
					animRangeChanged = true;
				}
			}
			if (animRangeChanged)
				m_maxInterface->SetAnimRange(animRange);

			// 设置动画关键桢数据
			IKeyControl* keyControl = GetKeyControlInterface(rotControl);
			keyControl->SetNumKeys(boneDef.rotation.nKeys);

			for (unsigned int j = 0; j < boneDef.rotation.nKeys; ++j)
			{
				AnyKey buf;
				ILinRotKey* key = reinterpret_cast<ILinRotKey*>((IKey*)buf);
				key->time = timeData[j];
				key->val = keyData[j];
				keyControl->AppendKey(key);
			}
		}
*/
		// Scaling
		if (boneDef.scaling.nKeys)
		{
			Control* scaControl = CreateScaleController();
			tmControl->SetScaleController(scaControl);

			unsigned int* timeData = (unsigned int*)(m_m2FileData + boneDef.scaling.ofsTimes);
			Point3* keyData = (Point3*)(m_m2FileData + boneDef.scaling.ofsKeys);

			// 设置动画时间范围
			bool animRangeChanged = false;
			Interval animRange = m_maxInterface->GetAnimRange();
			for (unsigned int j = 0; j < boneDef.scaling.nKeys; ++j)
			{
				if (timeData[j] < animRange.Start())
				{
					animRange.SetStart(timeData[j]);
					animRangeChanged = true;
				}
				else if (timeData[j] > animRange.End())
				{
					animRange.SetEnd(timeData[j]);
					animRangeChanged = true;
				}
			}
			if (animRangeChanged)
				m_maxInterface->SetAnimRange(animRange);

			// 设置动画关键桢数据
			IKeyControl* keyControl = GetKeyControlInterface(scaControl);
			keyControl->SetNumKeys(boneDef.scaling.nKeys);

			for (unsigned int j = 0; j < boneDef.scaling.nKeys; ++j)
			{
				AnyKey buf;
				ILinScaleKey* key = reinterpret_cast<ILinScaleKey*>((IKey*)buf);
				key->time = timeData[j];
				key->val = ScaleValue(keyData[j]);
				keyControl->AppendKey(key);
			}
		}
	}
}


// 创建线性的动画控制器
Control* M2Importer::CreatePositionController()
{
	Control* posControl = (Control*)m_maxInterface->CreateInstance(
		CTRL_POSITION_CLASS_ID, IPOS_CONTROL_CLASS_ID);

	Control* tmp;
	tmp = (Control*)m_maxInterface->CreateInstance(
		CTRL_FLOAT_CLASS_ID, Class_ID(LININTERP_FLOAT_CLASS_ID,0));
	posControl->SetReference(IPOS_X_REF, tmp);

	tmp = (Control*)m_maxInterface->CreateInstance(
		CTRL_FLOAT_CLASS_ID, Class_ID(LININTERP_FLOAT_CLASS_ID,0));
	posControl->SetReference(IPOS_Y_REF, tmp);

	tmp = (Control*)m_maxInterface->CreateInstance(
		CTRL_FLOAT_CLASS_ID, Class_ID(LININTERP_FLOAT_CLASS_ID,0));
	posControl->SetReference(IPOS_Z_REF, tmp);

	return posControl;
}

Control* M2Importer::CreateRotationController()
{
	Control* rotControl = (Control*)m_maxInterface->CreateInstance(
		CTRL_ROTATION_CLASS_ID, Class_ID(LININTERP_ROTATION_CLASS_ID,0));

	return rotControl;
}

Control* M2Importer::CreateScaleController()
{
	Control* scaControl = (Control*)m_maxInterface->CreateInstance(
		CTRL_SCALE_CLASS_ID, Class_ID(LININTERP_SCALE_CLASS_ID,0));

	return scaControl;
}
