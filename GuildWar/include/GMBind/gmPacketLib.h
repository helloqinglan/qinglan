
// *-----------------------------------------------------------------
//
// WorldPacket���󵽽ű��ķ�װ
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

// ����һ��Packet�������������
gmUserObject* createGMWorldPacket(gmMachine* a_machine, const WorldPacket& pkt);

// gmPacket����ID
extern gmType GM_PACKET;
