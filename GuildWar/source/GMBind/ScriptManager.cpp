
// *-----------------------------------------------------------------
//
// 脚本管理器实现
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "GMBind/ScriptManager.h"
#include "GMBind/gmCall.h"
#include "Util/GameOption.h"

#include "GMBind/gmMathLib.h"
#include "GMBind/gmStringLib.h"
#include "GMBind/gmArrayLib.h"
#include "GMBind/gmSystemLib.h"
#include "GMBind/gmVector3Lib.h"
#include "gm/gmDebug.h"

#include "GMBind/gmPacketLib.h"
#include "GMBind/gmByteBufferLib.h"
#include "GMBind/gmEventsLib.h"

// test
#include "WorldThread/WorldPacket.h"
#include "Util/EventServer.h"

ScriptManager::ScriptManager()
: m_scriptMachine(0)
{
}

ScriptManager::~ScriptManager()
{
	delete m_scriptMachine;
}

bool ScriptManager::initialize()
{
	if (m_scriptMachine)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("ScriptManager::Initialize 脚本管理器被多次初始化.\n")));
		return false;
	}

	m_scriptMachine = new gmMachine();

	// GM默认库
	gmBindMathLib(m_scriptMachine);
	gmBindStringLib(m_scriptMachine);
	gmBindArrayLib(m_scriptMachine);
	gmBindSystemLib(m_scriptMachine);
	gmBindVector3Lib(m_scriptMachine);
#if GMDEBUG_SUPPORT
	gmBindDebugLib(m_scriptMachine);
#endif

	// GuildWar附加库
	gmBindPacketLib(m_scriptMachine);
	gmBindByteBufferLib(m_scriptMachine);
	gmBindEventsLib(m_scriptMachine);

	if (!runTestScript())
		return false;

	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("ScriptManager::Initialize 脚本管理器初始化成功.\n")));
	return true;
}

bool ScriptManager::executeString(const std::string& script)
{
	int errors = m_scriptMachine->ExecuteString(script.c_str());
	if (errors)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("ScriptManager::Initialize 测试脚本执行失败, 原因: \n")));

		bool first = true;
		const char* message;
		while ((message = m_scriptMachine->GetLog().GetEntry(first)))
			ACE_ERROR ((GAME_ERROR ACE_TEXT("%s"), message));

		m_scriptMachine->GetLog().Reset();

		return false;
	}

	return true;
}

bool ScriptManager::runTestScript()
{
	const std::string& fileName = GAME_OPTION->getTestScriptFile();
	ifstream testStream(fileName.c_str());
	if (!testStream)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("ScriptManager::runTestScript 测试脚本打开失败, 文件名:  %s.\n"),
			fileName.c_str()));
		return false;
	}

	std::string fileStr;
	while (testStream.good())
	{
		std::string lineStr;
		std::getline(testStream, lineStr);
		fileStr += lineStr;
	}

	if (!executeString(fileStr.c_str()))
		return false;
/*
	// 测试string参数传递的问题
	WorldPacket pkt(10, 100);
	pkt << "看到这段表示脚本注册成功";

	gmCall call;
	call.BeginGlobalFunction(m_scriptMachine, "initTestFunction");
	gmUserObject* obj = createGMWorldPacket(m_scriptMachine, pkt);
	call.AddParamUser(obj);
	call.End();
*/
	// 测试event
	ByteBuffer arg;
	arg << (int)1;
	arg << (float)102.35f;
	arg << "hello world";
	S_EventServer->emit(1, arg);

	return true;
}
