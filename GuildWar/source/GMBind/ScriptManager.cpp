
// *-----------------------------------------------------------------
//
// 脚本管理器实现
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "GMBind/ScriptManager.h"

#include "GMBind/gmMathLib.h"
#include "GMBind/gmStringLib.h"
#include "GMBind/gmArrayLib.h"
#include "GMBind/gmSystemLib.h"
#include "GMBind/gmVector3Lib.h"
#include "gm/gmDebug.h"

#include "GMBind/gmPacketLib.h"

// test
#include "WorldThread/WorldPacket.h"

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

	// 测试一段script的执行
	if (!executeString("a = WorldPacket(10);print(a.getOpcode());"))
		return false;

	// 测试string的长度问题
	WorldPacket pkt(10, 100);
	pkt << "hello");
	int xxx = pkt.length();

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
