
// *-----------------------------------------------------------------
//
// WorldPacket对象到脚本的封装
//
// *-----------------------------------------------------------------

#pragma once

#include "gmConfig.h"
#include "gmVariable.h"

class gmMachine;
class gmThread;
class gmUserObject;
class WorldPacket;


// 绑定 Packet Library
void gmBindPacketLib(gmMachine * a_machine);

// 将一个Packet对象压到栈上
void gmPacket_Push(gmThread* a_thread, const WorldPacket& pkt);

// 创建一个Packet对象并且填充数据
gmUserObject* gmPacket_Create(gmMachine* a_machine, const WorldPacket& pkt);


// gmPacket类型ID
extern gmType GM_PACKET;
