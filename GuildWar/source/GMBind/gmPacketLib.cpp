
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
	{
		m_packet = new WorldPacket();
	}

	gmWorldPacket(const WorldPacket& pkt)
	{
		m_packet = new WorldPacket(pkt);
	}

	~gmWorldPacket()
	{
		ACE_DEBUG ((GAME_DEBUG ACE_TEXT("gmWorldPacket::~gmWorldPacket gmWorldPacket对象(%d)被析构.\n"),
			m_packet->getOpcode()));
		delete m_packet;
	}

	// 消息码
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

		gmStringObject* strObj = SCRIPT_MANAGER->machine()->AllocStringObject(data.c_str());
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
		pkt->m_packet->append((const char*)data, strlen(data) + 1);
		return GM_OK;
	}

	// ***TODO*** test
	static int GM_CDECL sendData(gmThread* a_thread)
	{
		gmWorldPacket* pkt = (gmWorldPacket*)a_thread->ThisUser_NoChecks();
		std::string data;
		*(pkt->m_packet) >> data;
		ACE_DEBUG ((GAME_DEBUG ACE_TEXT("gmWorldPacket::sendData 测试数据: %s.\n"), 
			data.c_str()));
		return GM_OK;
	}

	// *-------------------------------------------------------------
	// 脚本中WorldPacket类型的构造函数
	static int GM_CDECL Packet(gmThread* a_thread)
	{
		gmWorldPacket* pkt = new gmWorldPacket();

		int numParams = a_thread->GetNumParams();
		if (numParams > 0)
			pkt->m_packet->setOpcode((u_short)gmGetFloatOrIntParamAsInt(a_thread, 0));
		if (numParams > 1)
			pkt->m_packet->reserve(gmGetFloatOrIntParamAsInt(a_thread, 1));

		a_thread->PushNewUser(pkt, GM_PACKET);
		return GM_OK;
	}

	// *-------------------------------------------------------------
	// gmMachine GC 所需要的方法
#if GM_USE_INCGC
	static void GM_CDECL GCDestruct(gmMachine* a_machine, gmUserObject* a_object)
#else
	static void GM_CDECL Collect(gmMachine * a_machine, gmUserObject * a_object, gmuint32 a_mark)
#endif
	{
		GM_ASSERT(a_object->m_userType == GM_PACKET);
		gmWorldPacket* object = (gmWorldPacket*)a_object->m_user;

		ACE_DEBUG ((GAME_DEBUG ACE_TEXT("gmWorldPacket::GCDestruct gmWorldPacket对象(%d)被GC回收.\n"), 
			object->m_packet->getOpcode()));

		// ***TODO*** GC是怎么处理的? 需不需要显示的删除?
		delete object;
	}

	static void GM_CDECL AsString(gmUserObject* a_object, char* a_buffer, int a_bufferLen)
	{
		gmWorldPacket* pkt = (gmWorldPacket*)a_object->m_user;
		_gmsnprintf(a_buffer, a_bufferLen, "WorldPacket (%d)", pkt->m_packet->getOpcode());
	}

private:
	WorldPacket* m_packet;			// 引用到实际的数据
};


// *-----------------------------------------------------------------
// 创建一个WorldPacket对象并且填充数据
gmUserObject* createGMWorldPacket(gmMachine* a_machine, const WorldPacket& pkt)
{
	gmWorldPacket* gmpkt = new gmWorldPacket(pkt);
	return a_machine->AllocUserObject(gmpkt, GM_PACKET);
}


// *-----------------------------------------------------------------
// Libs

static gmFunctionEntry s_packetLib[] =
{
	{"WorldPacket", gmWorldPacket::Packet},
};

static gmFunctionEntry s_packetTypeLib[] =
{
	// 消息码
	{"getOpcode", gmWorldPacket::getOpcode},
	{"setOpcode", gmWorldPacket::setOpcode},

	// 获取数据
	{"getInt", gmWorldPacket::getInt},
	{"getShort", gmWorldPacket::getShort},
	{"getFloat", gmWorldPacket::getFloat},
	{"getChar", gmWorldPacket::getChar},
	{"getString", gmWorldPacket::getString},

	// 设置数据
	{"putInt", gmWorldPacket::putInt},
	{"putShort", gmWorldPacket::putShort},
	{"putFloat", gmWorldPacket::putFloat},
	{"putChar", gmWorldPacket::putChar},
	{"putString", gmWorldPacket::putString},

	// ***TODO*** test
	{"sendData", gmWorldPacket::sendData},
};


// 绑定 WorldPacket Library
void gmBindPacketLib(gmMachine * a_machine)
{
	// Lib (相当于注册全局函数)
	a_machine->RegisterLibrary(s_packetLib, sizeof(s_packetLib) / sizeof(s_packetLib[0]));

	// Register new user type
	GM_PACKET = a_machine->CreateUserType("WorldPacket");

	// Operators

	// Type Lib (相当于注册类成员函数)
	a_machine->RegisterTypeLibrary(GM_PACKET, s_packetTypeLib, sizeof(s_packetTypeLib) / sizeof(s_packetTypeLib[0]));

	// Register garbage collection for type
#if GM_USE_INCGC
	a_machine->RegisterUserCallbacks(GM_PACKET, NULL, gmWorldPacket::GCDestruct, gmWorldPacket::AsString); 
#else
	a_machine->RegisterUserCallbacks(GM_PACKET, NULL, gmWorldPacket::Collect, gmWorldPacket::AsString);
#endif
}
