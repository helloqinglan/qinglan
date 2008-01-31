
// *-----------------------------------------------------------------
//
// 生物接口组件实现
//
// *-----------------------------------------------------------------

#pragma once

#include "Component/UnitInterfComp.h"

class UnitInterfImpl : public UnitInterfComp
{
public:
	UnitInterfImpl(Entity* entity);

	// *-------------------------------------------------------------
	u_int level() const;
	u_char race() const;
	u_char cls() const;
	u_char gender() const;

	// *-------------------------------------------------------------
	u_int map() const { return m_mapID; }
	u_int zone() const;
	float posX() const { return m_positionX; }
	float posY() const { return m_positionY; }
	float posZ() const { return m_positionZ; }
	float orientation() const { return m_orientation; }

	// *-------------------------------------------------------------
	void map(u_int id) { m_mapID = id; }
	void posX(float x) { m_positionX = x; }
	void posY(float y) { m_positionY = y; }
	void posZ(float z) { m_positionZ = z; }
	void orientation(float o) { m_orientation = o; }

private:
	// 坐标相关属性
	u_int m_mapID;
	float m_positionX;
	float m_positionY;
	float m_positionZ;
	float m_orientation;
};
