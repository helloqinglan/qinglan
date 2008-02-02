
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
class gmWorldPacket
{
public:
	gmWorldPacket()
		: m_userObject(0)
	{
		m_packet = new WorldPacket();
	}

	gmWorldPacket(const WorldPacket& pkt)
		: m_userObject(0)
	{
		m_packet = new WorldPacket(pkt);
	}

	~gmWorldPacket()
	{
		delete m_packet;
	}

	static int GM_CDECL getOpcode(gmThread* a_thread)
	{
		gmWorldPacket* pkt = (gmWorldPacket*)a_thread->ThisUser_NoChecks();
		a_thread->PushInt(pkt->m_packet->getOpcode());
		return GM_OK;
	}

	static int GM_CDECL setOpcode(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_INT_PARAM(opcode, 0);

		gmWorldPacket* pkt = (gmWorldPacket*)a_thread->ThisUser_NoChecks();
		pkt->m_packet->setOpcode((u_short)opcode);
		return GM_OK;
	}

	// 获取数据
	static int GM_CDECL getInt(gmThread* a_thread)
	{
		gmWorldPacket* pkt = (gmWorldPacket*)a_thread->ThisUser_NoChecks();
		a_thread->PushInt(pkt->m_packet->read<u_int>());
		return GM_OK;
	}

	static int GM_CDECL getShort(gmThread* a_thread)
	{
		gmWorldPacket* pkt = (gmWorldPacket*)a_thread->ThisUser_NoChecks();
		a_thread->PushInt(pkt->m_packet->read<u_short>());
		return GM_OK;
	}

	static int GM_CDECL getFloat(gmThread* a_thread)
	{
		gmWorldPacket* pkt = (gmWorldPacket*)a_thread->ThisUser_NoChecks();
		a_thread->PushFloat(pkt->m_packet->read<float>());
		return GM_OK;
	}

	static int GM_CDECL getChar(gmThread* a_thread)
	{
		gmWorldPacket* pkt = (gmWorldPacket*)a_thread->ThisUser_NoChecks();
		a_thread->PushFloat(pkt->m_packet->read<u_char>());
		return GM_OK;
	}

	static int GM_CDECL getString(gmThread* a_thread)
	{
		gmWorldPacket* pkt = (gmWorldPacket*)a_thread->ThisUser_NoChecks();

		std::string data;
		*(pkt->m_packet) >> data;
		gmStringObject* strObj = SCRIPTMANAGER->machine()->AllocStringObject(data.c_str());

		a_thread->PushString(strObj);
		return GM_OK;
	}

	// 设置数据
	static int GM_CDECL putInt(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_INT_PARAM(data, 0);

		gmWorldPacket* pkt = (gmWorldPacket*)a_thread->ThisUser_NoChecks();
		pkt->m_packet->append<int>(data);
		return GM_OK;
	}

	static int GM_CDECL putShort(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_INT_PARAM(data, 0);

		gmWorldPacket* pkt = (gmWorldPacket*)a_thread->ThisUser_NoChecks();
		pkt->m_packet->append<short>((short)data);
		return GM_OK;
	}

	static int GM_CDECL putFloat(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_FLOAT_PARAM(data, 0);

		gmWorldPacket* pkt = (gmWorldPacket*)a_thread->ThisUser_NoChecks();
		pkt->m_packet->append<float>(data);
		return GM_OK;
	}

	static int GM_CDECL putChar(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_INT_PARAM(data, 0);

		gmWorldPacket* pkt = (gmWorldPacket*)a_thread->ThisUser_NoChecks();
		pkt->m_packet->append<char>((char)data);
		return GM_OK;
	}

	static int GM_CDECL putString(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_STRING_PARAM(data, 0);

		gmWorldPacket* pkt = (gmWorldPacket*)a_thread->ThisUser_NoChecks();
		pkt->m_packet->append(data);
		return GM_OK;
	}

	// *-------------------------------------------------------------
	// 脚本中WorldPacket类型的构造函数
	static int GM_CDECL Packet(gmThread* a_thread)
	{
		gmWorldPacket* pkt = new gmWorldPacket();
		pkt->m_userObject = SCRIPTMANAGER->machine()->AllocUserObject(pkt, GM_PACKET);

		int numParams = a_thread->GetNumParams();
		if (numParams > 0)
			pkt->m_packet->setOpcode((u_short)gmGetFloatOrIntParamAsInt(a_thread, 0));
		if (numParams > 1)
			pkt->m_packet->reserve(gmGetFloatOrIntParamAsInt(a_thread, 1));

		a_thread->PushNewUser(pkt, GM_PACKET);
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
		gmWorldPacket* object = (gmWorldPacket*)a_object->m_user;
		// ***TODO*** GC回收是怎么处理的?
		//delete object;
	}

	static void GM_CDECL AsString(gmUserObject* a_object, char* a_buffer, int a_bufferLen)
	{
		WorldPacket* pkt = (WorldPacket*)a_object->m_user;
		_gmsnprintf(a_buffer, a_bufferLen, "WorldPacket (%d)", pkt->getOpcode());
	}

public:
	WorldPacket* m_packet;			// 引用到实际的数据
	gmUserObject* m_userObject;		// 引用到脚本中创建的对象
};


// *-----------------------------------------------------------------
// 创建一个WorldPacket对象并且填充数据
gmUserObject* createGMWorldPacket(gmMachine* a_machine, const WorldPacket& pkt)
{
	gmWorldPacket* gmpkt = new gmWorldPacket(pkt);
	gmpkt->m_userObject = a_machine->AllocUserObject(gmpkt, GM_PACKET);

	return gmpkt->m_userObject;
}


// *-----------------------------------------------------------------
// Libs

static gmFunctionEntry s_packetLib[] =
{
	{"WorldPacket", gmWorldPacket::Packet},
};

static gmFunctionEntry s_packetTypeLib[] =
{
	{"getOpcode", gmWorldPacket::getOpcode},
	{"setOpcode", gmWorldPacket::setOpcode},
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
	a_machine->RegisterUserCallbacks(GM_PACKET, NULL, gmWorldPacket::GCDestruct, gmWorldPacket::AsString); 
#else
	a_machine->RegisterUserCallbacks(GM_PACKET, NULL, gmWorldPacket::Collect, gmWorldPacket::AsString);
#endif
}
