
// *-----------------------------------------------------------------
//
// 数据存取组件基类
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
	// 存数据库
	virtual bool save() = 0;

	// 最小化加载, 为角色列表准备数据
	// ***TODO*** 看看有没有必要分miniLoad()和load()两个方法
	virtual bool miniLoad(u_int id) = 0;

	// 从数据库加载
	// ***TODO*** 不同类型的entity读取的表会不一样, 比如玩家跟NPC, 如何区分?
	virtual bool load(u_int id) = 0;


	// *-------------------------------------------------------------
	// 构造用于角色列表的数据包
	virtual void buildEnumPacket(WorldPacket& packet) = 0;
};
