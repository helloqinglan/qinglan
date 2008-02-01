
// *-----------------------------------------------------------------
//
// ���ݴ�ȡ�������
//
// *-----------------------------------------------------------------

#pragma once

#include "ComponentBase.h"

class DataIOComp : public ComponentBase
{
public:
	DataIOComp(Entity* entity)
		: ComponentBase(ComponentBase::DataIO, entity)
	{
	}


	// *-------------------------------------------------------------
	// �����ݿ�
	virtual bool save() = 0;

	// ��С������, Ϊ��ɫ�б�׼������
	// ***TODO*** ������û�б�Ҫ��miniLoad()��load()��������
	virtual bool miniLoad(u_int id) = 0;

	// �����ݿ����
	// ***TODO*** ��ͬ���͵�entity��ȡ�ı�᲻һ��, ������Ҹ�NPC, �������?
	virtual bool load(u_int id) = 0;


	// *-------------------------------------------------------------
	// �������ڽ�ɫ�б�����ݰ�
	virtual void buildEnumPacket(WorldPacket& packet) = 0;
};
