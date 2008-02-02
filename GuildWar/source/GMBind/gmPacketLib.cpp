
// *-----------------------------------------------------------------
//
// WorldPacket对象到脚本的封装
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "GMBind/gmPacketLib.h"
#include "GMBind/gmHelpers.h"
#include "GMBind/ScriptManager.h"
#include "WorldThread/WorldPacket.h"


gmType GM_PACKET = GM_NULL;

// 对WorldPacket的封装, 以在脚本中处理网络数据包
class gmPacketObj
{
public:
	static int GM_CDECL getOpcode(gmThread* a_thread)
	{
		WorldPacket* pkt = (WorldPacket*)a_thread->ThisUser_NoChecks();
		a_thread->PushInt(pkt->getOpcode());
		return GM_OK;
	}

	static int GM_CDECL setOpcode(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_INT_PARAM(opcode, 0);

		WorldPacket* pkt = (WorldPacket*)a_thread->ThisUser_NoChecks();
		pkt->setOpcode((u_short)opcode);
		return GM_OK;
	}

	// 获取数据
	static int GM_CDECL getInt(gmThread* a_thread)
	{
		WorldPacket* pkt = (WorldPacket*)a_thread->ThisUser_NoChecks();
		a_thread->PushInt(pkt->read<u_int>());
		return GM_OK;
	}

	static int GM_CDECL getShort(gmThread* a_thread)
	{
		WorldPacket* pkt = (WorldPacket*)a_thread->ThisUser_NoChecks();
		a_thread->PushInt(pkt->read<u_short>());
		return GM_OK;
	}

	static int GM_CDECL getFloat(gmThread* a_thread)
	{
		WorldPacket* pkt = (WorldPacket*)a_thread->ThisUser_NoChecks();
		a_thread->PushFloat(pkt->read<float>());
		return GM_OK;
	}

	static int GM_CDECL getChar(gmThread* a_thread)
	{
		WorldPacket* pkt = (WorldPacket*)a_thread->ThisUser_NoChecks();
		a_thread->PushFloat(pkt->read<u_char>());
		return GM_OK;
	}

	static int GM_CDECL getString(gmThread* a_thread)
	{
		WorldPacket* pkt = (WorldPacket*)a_thread->ThisUser_NoChecks();

		std::string data;
		*pkt >> data;
		gmStringObject* strObj = SCRIPTMANAGER->machine()->AllocStringObject(data.c_str());

		a_thread->PushString(strObj);
		return GM_OK;
	}

	// 设置数据
	static int GM_CDECL putInt(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_INT_PARAM(data, 0);

		WorldPacket* pkt = (WorldPacket*)a_thread->ThisUser_NoChecks();
		pkt->append<int>(data);
		return GM_OK;
	}

	static int GM_CDECL putShort(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_INT_PARAM(data, 0);

		WorldPacket* pkt = (WorldPacket*)a_thread->ThisUser_NoChecks();
		pkt->append<short>((short)data);
		return GM_OK;
	}

	static int GM_CDECL putFloat(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_FLOAT_PARAM(data, 0);

		WorldPacket* pkt = (WorldPacket*)a_thread->ThisUser_NoChecks();
		pkt->append<float>(data);
		return GM_OK;
	}

	static int GM_CDECL putChar(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_INT_PARAM(data, 0);

		WorldPacket* pkt = (WorldPacket*)a_thread->ThisUser_NoChecks();
		pkt->append<char>((char)data);
		return GM_OK;
	}

	static int GM_CDECL putString(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_STRING_PARAM(data, 0);

		WorldPacket* pkt = (WorldPacket*)a_thread->ThisUser_NoChecks();
		pkt->append(data);
		return GM_OK;
	}
/*
	static int GM_CDECL setOpcode(gmThread* a_thread)
	{
		return GM_OK;
	}
*/
	// *-------------------------------------------------------------
	// 脚本中WorldPacket类型的构造函数
	static int GM_CDECL Packet(gmThread* a_thread)
	{
		int numParams = a_thread->GetNumParams();
		WorldPacket* newPkt = Alloc(a_thread->GetMachine(),true);

		if (numParams > 0)
			newPkt->setOpcode((u_short)gmGetFloatOrIntParamAsInt(a_thread, 0));

		a_thread->PushNewUser(newPkt, GM_PACKET);
		return GM_OK;
	}

	// *-------------------------------------------------------------
	// gmMachine用来创建及回收内存所需要的方法
#if GM_USE_INCGC
	static void GM_CDECL GCDestruct(gmMachine* a_machine, gmUserObject* a_object)
#else
	static void GM_CDECL Collect(gmMachine * a_machine, gmUserObject * a_object, gmuint32 a_mark)
#endif
	{
		GM_ASSERT(a_object->m_userType == GM_PACKET);
		WorldPacket* object = (WorldPacket*)a_object->m_user;
		Free(a_machine, object);
	}

	static void GM_CDECL AsString(gmUserObject* a_object, char* a_buffer, int a_bufferLen)
	{
		WorldPacket* pkt = (WorldPacket*)a_object->m_user;
		_gmsnprintf(a_buffer, a_bufferLen, "(%d)", pkt->getOpcode());
	}

	// Allocate memory for one object
	static WorldPacket* Alloc(gmMachine* a_machine, bool a_clearToZero)
	{
		a_machine->AdjustKnownMemoryUsed(sizeof(WorldPacket));
		WorldPacket* newObj = (WorldPacket*)s_mem.Alloc();
		return newObj;
	}

	// Free memory for one object
	static void Free(gmMachine* a_machine, WorldPacket* a_obj)
	{
		a_machine->AdjustKnownMemoryUsed(-(int)sizeof(WorldPacket));
		s_mem.Free(a_obj);
	}

};

/*
// *-----------------------------------------------------------------
// 将一个WorldPacket对象压到栈上
void gmPacket_Push(gmThread* a_thread, const WorldPacket& pkt)
{
	WorldPacket* newPkt = gmPacketObj::Alloc(a_thread->GetMachine(), false);
	// ***TODO*** 怎么赋值?
	//*newPkt = *(WorldPacket*)a_vec;
	a_thread->PushNewUser(newPkt, GM_PACKET);
}

// 创建一个WorldPacket对象并且填充数据
gmUserObject* gmPacket_Create(gmMachine* a_machine, const WorldPacket& pkt)
{
	WorldPacket* newPkt = gmPacketObj::Alloc(a_machine, false);
	// ***TODO*** 怎么赋值?
	//*newPkt = *(WorldPacket*)a_vec;
	return a_machine->AllocUserObject(newPkt, GM_PACKET);
}
*/

// *-----------------------------------------------------------------
// Libs

static gmFunctionEntry s_packetLib[] =
{
	{"WorldPacket", gmPacketObj::Packet},
};

static gmFunctionEntry s_packetTypeLib[] =
{
	{"getOpcode", gmPacketObj::getOpcode},
	{"setOpcode", gmPacketObj::setOpcode},
};


// 绑定 WorldPacket Library
void gmBindPacketLib(gmMachine * a_machine)
{
	// Lib
	a_machine->RegisterLibrary(s_packetLib, sizeof(s_packetLib) / sizeof(s_packetLib[0]));

	// Register new user type
	GM_PACKET = a_machine->CreateUserType("WorldPacket");

	// Operators

	// Type Lib
	a_machine->RegisterTypeLibrary(GM_PACKET, s_packetTypeLib, sizeof(s_packetTypeLib) / sizeof(s_packetTypeLib[0]));

	// Register garbage collection for type
#if GM_USE_INCGC
	a_machine->RegisterUserCallbacks(GM_PACKET, NULL, gmPacketObj::GCDestruct, gmPacketObj::AsString); 
#else
	a_machine->RegisterUserCallbacks(GM_PACKET, NULL, gmPacketObj::Collect, gmPacketObj::AsString);
#endif
}
