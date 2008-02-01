
// *-----------------------------------------------------------------
//
// 数据存取组件实现类
//
// *-----------------------------------------------------------------

#pragma once

#include "Component/DataIOComp.h"

class DataIOImpl : public DataIOComp
{
public:
	DataIOImpl(Entity* entity);

	// 存数据库
	bool save();

	// 最小化加载, 为角色列表准备数据
	bool miniLoad(u_int id);

	// 从数据库加载
	// ***TODO*** 不同类型的entity读取的表会不一样, 比如玩家跟NPC, 如何区分?
	bool load(u_int id);

	// 构造用于角色列表的数据包
	void buildEnumPacket(WorldPacket& packet);
};
