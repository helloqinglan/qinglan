
// *-----------------------------------------------------------------
//
// ByteBuffer对象到脚本的封装
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

// 创建一个Packet对象并且填充数据
gmUserObject* createGMByteBuffer(gmMachine* a_machine, const ByteBuffer& buf);

// gmPacket类型ID
extern gmType GM_BYTEBUFFER;
