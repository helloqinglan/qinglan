
// *-----------------------------------------------------------------
//
// 应用程序总入口
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "Util/GameOption.h"
#include "Database/Database.h"
#include "GMBind/ScriptManager.h"
#include "RealmThread/RealmThread.h"
#include "WorldThread/WorldThread.h"

void closeThreads()
{
	WORLD_THREAD->close();
	REALM_THREAD->close();
}

int ACE_TMAIN(int, ACE_TCHAR *[])
{
	if (!GAME_OPTION->loadOptions())
		return -1;

	if (!LOG_MANAGER->redirectToFile(GAME_OPTION->getLogFile().c_str()))
		return -1;

	if (!SCRIPT_MANAGER->initialize())
		return -1;

	if (!DATABASE->initialize(GAME_OPTION->getDatabase().c_str()))
		return -1;

	// 开启登录服务线程
	if (REALM_THREAD->open() == -1)
		return -1;

	// 开启世界服务线程
	if (WORLD_THREAD->open() == -1)
	{
		closeThreads();
		return -1;
	}

	while (1)
	{
		ACE_Time_Value tv(1);
		ACE_OS::sleep(tv);
	}

	closeThreads();

	return 0;
}
