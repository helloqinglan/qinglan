
// *-----------------------------------------------------------------
//
// 世界服务网络包结构
//     对ByteBuffer的简单封装, 方便取该网络包的消息码
//
// *-----------------------------------------------------------------

#pragma once

#include "Util/ByteBuffer.h"

class WorldPacket : public ByteBuffer
{
public:
	WorldPacket() : ByteBuffer(), m_opcode(0) {}

	WorldPacket(size_t res) : ByteBuffer(res), m_opcode(0) {}

	explicit WorldPacket(u_short cmd, size_t res = 200) : ByteBuffer(res), m_opcode(cmd) {}

	WorldPacket(const WorldPacket& rh) : ByteBuffer(rh), m_opcode(rh.m_opcode) {}

	void initialize(u_short opcode)
	{
		clear();
		m_opcode = opcode;
	}

	// 获取该网络包的命令码
	u_short getOpcode() const { return m_opcode; }

	void setOpcode(u_short opcode) { m_opcode = opcode; }

protected:
	u_short m_opcode;
};
