
// *-----------------------------------------------------------------
//
// ByteBuffer���󵽽ű��ķ�װ
//
// *-----------------------------------------------------------------

#pragma once

#include "gmConfig.h"
#include "gmVariable.h"

class gmMachine;
class gmThread;
class gmUserObject;
class ByteBuffer;


// ByteBuffer Library
void gmBindByteBufferLib(gmMachine * a_machine);

// ����һ��Packet�������������
gmUserObject* createGMByteBuffer(gmMachine* a_machine, const ByteBuffer& buf);

// gmPacket����ID
extern gmType GM_BYTEBUFFER;
