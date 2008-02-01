
// *-----------------------------------------------------------------
//
// ���ݴ�ȡ���ʵ����
//
// *-----------------------------------------------------------------

#pragma once

#include "Component/DataIOComp.h"

class DataIOImpl : public DataIOComp
{
public:
	DataIOImpl(Entity* entity);

	// �����ݿ�
	bool save();

	// ��С������, Ϊ��ɫ�б�׼������
	bool miniLoad(u_int id);

	// �����ݿ����
	// ***TODO*** ��ͬ���͵�entity��ȡ�ı�᲻һ��, ������Ҹ�NPC, �������?
	bool load(u_int id);

	// �������ڽ�ɫ�б�����ݰ�
	void buildEnumPacket(WorldPacket& packet);
};
