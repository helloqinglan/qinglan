
// *-----------------------------------------------------------------
//
// 事件系统到脚本的封装
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "GMBind/gmCall.h"
#include "GMBind/gmEventsLib.h"
#include "GMBind/gmByteBufferLib.h"
#include "GMBind/gmHelpers.h"
#include "GMBind/ScriptManager.h"
#include "Util/EventServer.h"


// 脚本回调函数的封装类
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
			ACE_ERROR ((GAME_ERROR ACE_TEXT("ScriptCBWrapper::callback 函数不存在, 事件 %d 回调失败.\n"), m_eventID));
			return false;
		}

		gmUserObject* obj = createGMByteBuffer(SCRIPT_MANAGER->machine(), param);
		call.AddParamUser(obj);
		call.End();

		// create出来的GMByteBuffer对象不需要在这里删除
		// 如果参数不再被使用, 则会被GC回收, 在GC处理函数中删除该对象

		int retval = 1;
		if (!call.GetReturnedInt(retval))
			ACE_ERROR ((GAME_ERROR ACE_TEXT("ScriptCBWrapper::callback 回调函数没有返回值, 已设置为 1 .\n")));

		return (retval != 0);
	}

	const u_int& id() const { return m_eventID; }
	const gmFunctionObject* func() const { return m_cbFunction; }

private:
	u_int m_eventID;					// 事件ID
	gmFunctionObject* m_cbFunction;		// 回调的脚本函数对象
};


// 脚本的事件回调方法管理器
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
	// 一个事件可以有多个回调函数 (被多个地方注册)
	typedef std::vector<ScriptCBWrapper*> CBWrapperList;
	CBWrapperList m_wrapperList;
};
ScriptCBWrapperMgr g_scriptCbWrapperMgr;


// 注册事件的脚本回调
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


// 绑定 EventServer Library
void gmBindEventsLib(gmMachine * a_machine)
{
	a_machine->RegisterLibrary(s_eventsLib, sizeof(s_eventsLib) / sizeof(s_eventsLib[0]), "Events");
}
