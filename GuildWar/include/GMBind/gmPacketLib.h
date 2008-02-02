
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


// WorldPacket Library
void gmBindPacketLib(gmMachine * a_machine);

// 创建一个Packet对象并且填充数据
gmUserObject* createGMWorldPacket(gmMachine* a_machine, const WorldPacket& pkt);

// gmPacket类型ID
extern gmType GM_PACKET;
