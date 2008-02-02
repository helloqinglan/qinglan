
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


// �� Packet Library
void gmBindPacketLib(gmMachine * a_machine);

// ��һ��Packet����ѹ��ջ��
void gmPacket_Push(gmThread* a_thread, const WorldPacket& pkt);

// ����һ��Packet�������������
gmUserObject* gmPacket_Create(gmMachine* a_machine, const WorldPacket& pkt);


// gmPacket����ID
extern gmType GM_PACKET;
