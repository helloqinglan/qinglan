
// *-----------------------------------------------------------------
//
// �ű�������ʵ��
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
		ACE_ERROR ((GAME_ERROR ACE_TEXT("ScriptManager::Initialize �ű�����������γ�ʼ��.\n")));
		return false;
	}

	m_scriptMachine = new gmMachine();

	// GMĬ�Ͽ�
	gmBindMathLib(m_scriptMachine);
	gmBindStringLib(m_scriptMachine);
	gmBindArrayLib(m_scriptMachine);
	gmBindSystemLib(m_scriptMachine);
	gmBindVector3Lib(m_scriptMachine);
#if GMDEBUG_SUPPORT
	gmBindDebugLib(m_scriptMachine);
#endif

	// GuildWar���ӿ�
	gmBindPacketLib(m_scriptMachine);

	// ����һ��script��ִ��
	if (!executeString("a = WorldPacket(10);print(a.getOpcode());"))
		return false;

	// ����string�ĳ�������
	WorldPacket pkt(10, 100);
	pkt << "hello");
	int xxx = pkt.length();

	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("ScriptManager::Initialize �ű���������ʼ���ɹ�.\n")));
	return true;
}

bool ScriptManager::executeString(const std::string& script)
{
	int errors = m_scriptMachine->ExecuteString(script.c_str());
	if (errors)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT("ScriptManager::Initialize ���Խű�ִ��ʧ��, ԭ��: \n")));

		bool first = true;
		const char* message;
		while ((message = m_scriptMachine->GetLog().GetEntry(first)))
			ACE_ERROR ((GAME_ERROR ACE_TEXT("%s"), message));

		m_scriptMachine->GetLog().Reset();

		return false;
	}

	return true;
}
