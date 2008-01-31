
// *-----------------------------------------------------------------
//
// 生物接口组件基类
//
// *-----------------------------------------------------------------

#pragma once

#include "ComponentBase.h"

class UnitInterfComp : public ComponentBase
{
public:
	UnitInterfComp(Entity* entity)
		: ComponentBase(ComponentBase::UnitInterf, entity)
	{
	}

	// *----------------------------------------------------------------
	// 等级
	virtual u_int level() const = 0;

	// 种族
	virtual u_char race() const = 0;

	// 职业 (class的简称)
	virtual u_char cls() const = 0;

	// 性别
	virtual u_char gender() const = 0;


	// *----------------------------------------------------------------
	// 坐标相关
	virtual u_int map() const = 0;

	virtual u_int zone() const = 0;

	virtual float posX() const = 0;

	virtual float posY() const = 0;

	virtual float posZ() const = 0;

	virtual float orientation() const = 0;


	// *----------------------------------------------------------------
	virtual void map(u_int id) = 0;

	virtual void posX(float x) = 0;

	virtual void posY(float y) = 0;

	virtual void posZ(float z) = 0;

	virtual void orientation(float o) = 0;
};
