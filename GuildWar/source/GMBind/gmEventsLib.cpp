
// *-----------------------------------------------------------------
//
// �¼�ϵͳ���ű��ķ�װ
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "GMBind/gmCall.h"
#include "GMBind/gmEventsLib.h"
#include "GMBind/gmByteBufferLib.h"
#include "GMBind/gmHelpers.h"
#include "GMBind/ScriptManager.h"
#include "Util/EventServer.h"


// �ű��ص������ķ�װ��
class ScriptCBWrapper
{
public:
	ScriptCBWrapper(u_int id, gmFunctionObject* func)
		: m_eventID(id), m_cbFunction(func)
	{
		S_EventServer->bind(id, this, &ScriptCBWrapper::callback);
	}

	~ScriptCBWrapper()
	{
		S_EventServer->unbind(m_eventID, this, &ScriptCBWrapper::callback);
	}

	bool callback(const ByteBuffer& param)
	{
		gmCall call;

		if (!call.BeginFunction(SCRIPT_MANAGER->machine(), m_cbFunction))
		{
			ACE_ERROR ((GAME_ERROR ACE_TEXT("ScriptCBWrapper::callback ����������, �¼� %d �ص�ʧ��.\n"), m_eventID));
			return false;
		}

		gmUserObject* obj = createGMByteBuffer(SCRIPT_MANAGER->machine(), param);
		call.AddParamUser(obj);
		call.End();

		// create������GMByteBuffer������Ҫ������ɾ��
		// ����������ٱ�ʹ��, ��ᱻGC����, ��GC��������ɾ���ö���

		int retval = 1;
		if (!call.GetReturnedInt(retval))
			ACE_ERROR ((GAME_ERROR ACE_TEXT("ScriptCBWrapper::callback �ص�����û�з���ֵ, ������Ϊ 1 .\n")));

		return (retval != 0);
	}

	const u_int& id() const { return m_eventID; }
	const gmFunctionObject* func() const { return m_cbFunction; }

private:
	u_int m_eventID;					// �¼�ID
	gmFunctionObject* m_cbFunction;		// �ص��Ľű���������
};


// �ű����¼��ص�����������
class ScriptCBWrapperMgr
{
public:
	ScriptCBWrapperMgr()
	{
	}

	~ScriptCBWrapperMgr()
	{
	}

	void add(u_int id, gmFunctionObject* func)
	{
		if (has(id, func))
			return;

		ScriptCBWrapper* wrapper = new ScriptCBWrapper(id, func);
		m_wrapperList.push_back(wrapper);
	}

	void del(u_int id, gmFunctionObject* func)
	{
		if (!has(id, func))
			return;

		CBWrapperList::iterator itr;
		for (itr = m_wrapperList.begin(); itr != m_wrapperList.end(); ++itr)
		{
			ScriptCBWrapper* wrapper = *itr;
			if (wrapper->id() == id && wrapper->func() == func)
			{
				m_wrapperList.erase(itr);
				delete wrapper;
				return;
			}
		}
	}

	bool has(u_int id, gmFunctionObject* func)
	{
		CBWrapperList::const_iterator itr;
		for (itr = m_wrapperList.begin(); itr != m_wrapperList.end(); ++itr)
		{
			ScriptCBWrapper* wrapper = *itr;
			if (wrapper->id() == id && wrapper->func() == func)
				return true;
		}

		return false;
	}

private:
	// һ���¼������ж���ص����� (������ط�ע��)
	typedef std::vector<ScriptCBWrapper*> CBWrapperList;
	CBWrapperList m_wrapperList;
};
ScriptCBWrapperMgr g_scriptCbWrapperMgr;


// ע���¼��Ľű��ص�
static int GM_CDECL bindEventCallback(gmThread* a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(id, 0);
	GM_CHECK_FUNCTION_PARAM(callback, 1);

	g_scriptCbWrapperMgr.add(id, callback);

	return GM_OK;
}

static int GM_CDECL unbindEventCallback(gmThread* a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(id, 0);
	GM_CHECK_FUNCTION_PARAM(callback, 1);

	g_scriptCbWrapperMgr.del(id, callback);

	return GM_OK;
}


// *-----------------------------------------------------------------
// Libs

static gmFunctionEntry s_eventsLib[] =
{
	{"bindEventCallback", bindEventCallback},
	{"unbindEventCallback", unbindEventCallback},
};


// �� EventServer Library
void gmBindEventsLib(gmMachine * a_machine)
{
	a_machine->RegisterLibrary(s_eventsLib, sizeof(s_eventsLib) / sizeof(s_eventsLib[0]), "Events");
}
