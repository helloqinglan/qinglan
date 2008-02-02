
// *-----------------------------------------------------------------
//
// WorldPacket���󵽽ű��ķ�װ
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "GMBind/gmPacketLib.h"
#include "GMBind/gmHelpers.h"
#include "WorldThread/WorldPacket.h"


// ��WorldPacket�ķ�װ, ���ڽű��д����������ݰ�
// ***TODO*** �ǲ���ֻ��ʹ��static����?
class gmPacket
{
public:
	static int getOpcode(const gmPacket& pkt)
	{
		return pkt.m_packet.getOpcode();
	};

public:
	WorldPacket m_packet;
};


// The Packet bindings
// Just a set of useful functions, operators, etc. for Packet
class gmPacketObj
{
public:
	static int GM_CDECL getOpcode(gmThread * a_thread)
	{
		// ***TODO*** ��Щ���ö���ʲô��˼?
		GM_CHECK_NUM_PARAMS(0);
		gmPacket* thisPkt = (gmPacket*)a_thread->ThisUser_NoChecks();

		a_thread->PushInt(gmPacket::getOpcode(*thisPkt));
		return GM_OK;
	}


	static int GM_CDECL Packet(gmThread * a_thread)
	{
		int numParams = a_thread->GetNumParams();
		gmPacket* newPkt = Alloc(a_thread->GetMachine(),true);

		if (numParams > 0)
			newPkt->m_packet.setOpcode((u_short)gmGetFloatOrIntParamAsInt(a_thread, 0));

		a_thread->PushNewUser(newPkt, GM_PACKET);
		return GM_OK;
	}

	// *-------------------------------------------------------------
	// gmMachine���������������ڴ�����Ҫ�ķ���
#if GM_USE_INCGC
	static void GM_CDECL GCDestruct(gmMachine * a_machine, gmUserObject* a_object)
#else
	static void GM_CDECL Collect(gmMachine * a_machine, gmUserObject * a_object, gmuint32 a_mark)
#endif
	{
		GM_ASSERT(a_object->m_userType == GM_PACKET);
		gmPacket* object = (gmPacket*)a_object->m_user;
		Free(a_machine, object);
	}

	static void GM_CDECL AsString(gmUserObject * a_object, char* a_buffer, int a_bufferLen)
	{
		gmPacket* pkt = (gmPacket*)a_object->m_user;
		_gmsnprintf(a_buffer, a_bufferLen, "(%d)", pkt->m_packet.getOpcode());
	}

	// Allocate memory for one object
	static gmPacket* Alloc(gmMachine* a_machine, bool a_clearToZero)
	{
		a_machine->AdjustKnownMemoryUsed(sizeof(gmPacket));
		gmPacket* newObj = (gmPacket*)s_mem.Alloc();
		return newObj;
	}

	// Free memory for one object
	static void Free(gmMachine* a_machine, gmPacket* a_obj)
	{
		a_machine->AdjustKnownMemoryUsed(-(int)sizeof(gmPacket));
		s_mem.Free(a_obj);
	}

	// Only call when gmMachine is shutdown
	static void FreeAllMemory()
	{
		s_mem.ResetAndFreeMemory();
	}

private:
	// Static members
	static gmMemFixed s_mem;
};


// Static and Global instances
gmMemFixed gmPacketObj::s_mem(128, 64);
gmType GM_PACKET = GM_NULL;


// *-----------------------------------------------------------------
// ��һ��Packet����ѹ��ջ��
void gmPacket_Push(gmThread* a_thread, const WorldPacket& pkt)
{
	gmPacket* newPkt = gmPacketObj::Alloc(a_thread->GetMachine(), false);
	// ***TODO*** ��ô��ֵ?
	//*newPkt = *(gmPacket*)a_vec;
	a_thread->PushNewUser(newPkt, GM_PACKET);
}

// ����һ��Packet�������������
gmUserObject* gmPacket_Create(gmMachine* a_machine, const WorldPacket& pkt)
{
	gmPacket* newPkt = gmPacketObj::Alloc(a_machine, false);
	// ***TODO*** ��ô��ֵ?
	//*newPkt = *(gmPacket*)a_vec;
	return a_machine->AllocUserObject(newPkt, GM_PACKET);
}


// *-----------------------------------------------------------------
// Libs

static gmFunctionEntry s_packetLib[] =
{
	{"Packet", gmPacketObj::Packet},
};

static gmFunctionEntry s_packetTypeLib[] =
{
	{"getOpcode", gmPacketObj::getOpcode},
};


// �� Packet Library
void gmBindPacketLib(gmMachine * a_machine)
{
	// Lib
	a_machine->RegisterLibrary(s_packetLib, sizeof(s_packetLib) / sizeof(s_packetLib[0]));

	// Register new user type
	GM_PACKET = a_machine->CreateUserType("Packet");

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
