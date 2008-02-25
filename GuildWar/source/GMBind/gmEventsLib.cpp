
// *-----------------------------------------------------------------
//
// ByteBuffer对象到脚本的封装
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "GMBind/gmCall.h"
#include "GMBind/gmEventsLib.h"
#include "GMBind/gmByteBufferLib.h"
#include "GMBind/gmHelpers.h"
#include "GMBind/ScriptManager.h"
#include "Util/EventServer.h"

// 做一个封装std::string类型的回调函数的类
// 注册时new一个类对象进行注册
// 注销时根据id和string查询到对象来注销, 删除对象

class ScriptCBWrapper
{
public:
	ScriptCBWrapper(u_int id, const std::string& func)
		: m_eventID(id), m_cbFuncName(func)
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
		call.BeginGlobalFunction(SCRIPT_MANAGER->machine(), m_cbFuncName.c_str());
		gmUserObject* obj = createGMByteBuffer(SCRIPT_MANAGER->machine(), param);
		call.AddParamUser(obj);
		call.End();

		return true;
	}

	const u_int& id() const { return m_eventID; }
	const std::string& name() const { return m_cbFuncName; }

private:
	u_int m_eventID;				// 事件ID
	std::string m_cbFuncName;		// 回调的脚本函数名
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

	void add(u_int id, const std::string& func)
	{
		if (has(id, func))
			return;

		ScriptCBWrapper* wrapper = new ScriptCBWrapper(id, func);
		m_wrapperList.push_back(wrapper);
	}

	void del(u_int id, const std::string& func)
	{
		if (!has(id, func))
			return;

		CBWrapperList::iterator itr;
		for (itr = m_wrapperList.begin(); itr != m_wrapperList.end(); ++itr)
		{
			ScriptCBWrapper* wrapper = *itr;
			if (wrapper->id() == id && wrapper->name() == func)
			{
				m_wrapperList.erase(itr);
				delete wrapper;
				return;
			}
		}
	}

	bool has(u_int id, const std::string& func)
	{
		CBWrapperList::const_iterator itr;
		for (itr = m_wrapperList.begin(); itr != m_wrapperList.end(); ++itr)
		{
			ScriptCBWrapper* wrapper = *itr;
			if (wrapper->id() == id && wrapper->name() == func)
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
	GM_CHECK_STRING_PARAM(callback, 1);

	g_scriptCbWrapperMgr.add(id, callback);

	return GM_OK;
}

static int GM_CDECL unbindEventCallback(gmThread* a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(id, 0);
	GM_CHECK_STRING_PARAM(callback, 1);

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
	// Lib (相当于注册全局函数)
	a_machine->RegisterLibrary(s_eventsLib, sizeof(s_eventsLib) / sizeof(s_eventsLib[0]));
}
