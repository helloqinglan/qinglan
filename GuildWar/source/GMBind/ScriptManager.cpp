
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

ScriptManager::ScriptManager()
: m_scriptMachine(0)
{
}

ScriptManager::~ScriptManager()
{
	//delete m_scriptMachine;
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

	ACE_DEBUG ((GAME_DEBUG ACE_TEXT("ScriptManager::Initialize �ű���������ʼ���ɹ�.\n")));
	return true;
}
