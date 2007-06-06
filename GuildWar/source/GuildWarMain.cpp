
#include "GuildWarPch.h"
#include "Util/GameOption.h"
#include "Database/Database.h"

int ACE_TMAIN(int, ACE_TCHAR *[])
{
	if (!GAME_OPTION->loadOptions())
		return -1;

	if (!LOG_MANAGER->redirectToFile(GAME_OPTION->getLogFile().c_str()))
		return -1;

	if (!DATABASE->initialize(GAME_OPTION->getDatabase().c_str()))
		return -1;

	return 0;
}
