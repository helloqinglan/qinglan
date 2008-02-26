
// *-----------------------------------------------------------------
//
// ByteBuffer���󵽽ű��ķ�װ
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "GMBind/gmPacketLib.h"
#include "GMBind/gmHelpers.h"
#include "GMBind/ScriptManager.h"
#include "Util/ByteBuffer.h"


gmType GM_BYTEBUFFER = GM_NULL;

// ��ByteBuffer�ķ�װ, ���ڽű��д����������ݰ�
class gmByteBuffer
{
public:
	gmByteBuffer()
	{
		m_byteBuffer = new ByteBuffer();
	}

	gmByteBuffer(const ByteBuffer& buf)
	{
		m_byteBuffer = new ByteBuffer(buf);
	}

	~gmByteBuffer()
	{
		delete m_byteBuffer;
	}

	// ��ȡ����
	static int GM_CDECL getInt(gmThread* a_thread)
	{
		gmByteBuffer* buf = (gmByteBuffer*)a_thread->ThisUser_NoChecks();
		a_thread->PushInt(buf->m_byteBuffer->read<u_int>());
		return GM_OK;
	}

	static int GM_CDECL getShort(gmThread* a_thread)
	{
		gmByteBuffer* buf = (gmByteBuffer*)a_thread->ThisUser_NoChecks();
		a_thread->PushInt(buf->m_byteBuffer->read<u_short>());
		return GM_OK;
	}

	static int GM_CDECL getFloat(gmThread* a_thread)
	{
		gmByteBuffer* buf = (gmByteBuffer*)a_thread->ThisUser_NoChecks();
		a_thread->PushFloat(buf->m_byteBuffer->read<float>());
		return GM_OK;
	}

	static int GM_CDECL getChar(gmThread* a_thread)
	{
		gmByteBuffer* buf = (gmByteBuffer*)a_thread->ThisUser_NoChecks();
		a_thread->PushFloat(buf->m_byteBuffer->read<u_char>());
		return GM_OK;
	}

	static int GM_CDECL getString(gmThread* a_thread)
	{
		gmByteBuffer* buf = (gmByteBuffer*)a_thread->ThisUser_NoChecks();

		std::string data;
		*(buf->m_byteBuffer) >> data;

		gmStringObject* strObj = SCRIPT_MANAGER->machine()->AllocStringObject(data.c_str());
		a_thread->PushString(strObj);

		return GM_OK;
	}

	// ��������
	static int GM_CDECL putInt(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_INT_PARAM(data, 0);

		gmByteBuffer* buf = (gmByteBuffer*)a_thread->ThisUser_NoChecks();
		buf->m_byteBuffer->append<int>(data);
		return GM_OK;
	}

	static int GM_CDECL putShort(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_INT_PARAM(data, 0);

		gmByteBuffer* buf = (gmByteBuffer*)a_thread->ThisUser_NoChecks();
		buf->m_byteBuffer->append<short>((short)data);
		return GM_OK;
	}

	static int GM_CDECL putFloat(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_FLOAT_PARAM(data, 0);

		gmByteBuffer* buf = (gmByteBuffer*)a_thread->ThisUser_NoChecks();
		buf->m_byteBuffer->append<float>(data);
		return GM_OK;
	}

	static int GM_CDECL putChar(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_INT_PARAM(data, 0);

		gmByteBuffer* buf = (gmByteBuffer*)a_thread->ThisUser_NoChecks();
		buf->m_byteBuffer->append<char>((char)data);
		return GM_OK;
	}

	static int GM_CDECL putString(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_STRING_PARAM(data, 0);

		gmByteBuffer* buf = (gmByteBuffer*)a_thread->ThisUser_NoChecks();
		buf->m_byteBuffer->append((const char*)data, strlen(data) + 1);
		return GM_OK;
	}

	// *-------------------------------------------------------------
	// �ű���ByteBuffer���͵Ĺ��캯��
	static int GM_CDECL Buffer(gmThread* a_thread)
	{
		gmByteBuffer* buf = new gmByteBuffer();

		int numParams = a_thread->GetNumParams();
		if (numParams > 0)
			buf->m_byteBuffer->reserve(gmGetFloatOrIntParamAsInt(a_thread, 0));

		a_thread->PushNewUser(buf, GM_BYTEBUFFER);
		return GM_OK;
	}

	// *-------------------------------------------------------------
	// gmMachine GC ����Ҫ�ķ���
	static void GM_CDECL GCDestruct(gmMachine* a_machine, gmUserObject* a_object)
	{
		GM_ASSERT(a_object->m_userType == GM_BYTEBUFFER);
		gmByteBuffer* object = (gmByteBuffer*)a_object->m_user;
		delete object;
	}

	static void GM_CDECL AsString(gmUserObject* a_object, char* a_buffer, int a_bufferLen)
	{
		gmByteBuffer* buf = (gmByteBuffer*)a_object->m_user;
		_gmsnprintf(a_buffer, a_bufferLen, "ByteBuffer (%d)", buf->m_byteBuffer->length());
	}

	// *-------------------------------------------------------------
	// �ӽű���������ʹ�õķ���
	ByteBuffer* getBufferData() const
	{
		return m_byteBuffer;
	}

private:
	ByteBuffer* m_byteBuffer;			// ���õ�ʵ�ʵ�����
};


// *-----------------------------------------------------------------
// ����һ��ByteBuffer�������������
gmUserObject* createGMByteBuffer(gmMachine* a_machine, const ByteBuffer& buf)
{
	gmByteBuffer* gmbuf = new gmByteBuffer(buf);
	return a_machine->AllocUserObject(gmbuf, GM_BYTEBUFFER);
}


// *-----------------------------------------------------------------
// Libs

static gmFunctionEntry s_byteBufferLib[] =
{
	{"ByteBuffer", gmByteBuffer::Buffer},
};

static gmFunctionEntry s_packetTypeLib[] =
{
	// ��ȡ����
	{"getInt", gmByteBuffer::getInt},
	{"getShort", gmByteBuffer::getShort},
	{"getFloat", gmByteBuffer::getFloat},
	{"getChar", gmByteBuffer::getChar},
	{"getString", gmByteBuffer::getString},

	// ��������
	{"putInt", gmByteBuffer::putInt},
	{"putShort", gmByteBuffer::putShort},
	{"putFloat", gmByteBuffer::putFloat},
	{"putChar", gmByteBuffer::putChar},
	{"putString", gmByteBuffer::putString},
};


// �� ByteBuffer Library
void gmBindByteBufferLib(gmMachine * a_machine)
{
	// Lib (�൱��ע��ȫ�ֺ���)
	a_machine->RegisterLibrary(s_byteBufferLib, sizeof(s_byteBufferLib) / sizeof(s_byteBufferLib[0]));

	// Register new user type
	GM_BYTEBUFFER = a_machine->CreateUserType("ByteBuffer");

	// Operators

	// Type Lib (�൱��ע�����Ա����)
	a_machine->RegisterTypeLibrary(GM_BYTEBUFFER, s_packetTypeLib, sizeof(s_packetTypeLib) / sizeof(s_packetTypeLib[0]));

	// Register garbage collection for type
	a_machine->RegisterUserCallbacks(GM_BYTEBUFFER, NULL, gmByteBuffer::GCDestruct, gmByteBuffer::AsString);
}
