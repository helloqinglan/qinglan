
// *-----------------------------------------------------------------
//
// ≈‰÷√Œƒº˛∂¡–¥¿‡
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "Util/GameOption.h"

// default value
#define DEFAULT_DEVICE_TYPE RDT_DX9
#define DEFAULT_SCREEN_WIDTH 1024
#define DEFAULT_SCREEN_HEIGHT 768
#define DEFAULT_FULL_SCREEN false

GameOption::GameOption()
{
	ACE_OS_String::strcpy( m_configFile, ACE_TEXT("config.ini") );

	if (m_configHeap.open() == -1)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT ("GameOption::GameOption ACE_Configuration Open Error\n")));
		return;
	}
}

GameOption::~GameOption()
{
}

bool GameOption::loadOptions()
{
	ACE_Ini_ImpExp config_importer(m_configHeap);
	if (config_importer.import_config(m_configFile) == -1)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT ("GameOption::loadOptions %p\n"), m_configFile) );
		return false;
	}

	const ACE_Configuration_Section_Key & root = m_configHeap.root_section();
	ACE_Configuration_Section_Key commonSection;
	if (m_configHeap.open_section(root, ACE_TEXT("Common"), 0, commonSection) == -1)
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT ("GameOption::loadOptions Can't open Common section\n")));
		return false;
	}

	// LogFile
	ACE_TString logFile;
	if (m_configHeap.get_string_value( commonSection, ACE_TEXT("LogFile"), logFile) == -1 )
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT ("GameOption::loadOptions LogFile does not exist\n")));
		return false;
	}
	m_logFile = logFile.c_str();

	// Database
	ACE_TString database;
	if (m_configHeap.get_string_value( commonSection, ACE_TEXT("Database"), database) == -1 )
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT ("GameOption::loadOptions Database does not exist\n")));
		return false;
	}
	m_database = database.c_str();

	return true;
}

void GameOption::saveOptions()
{
	ACE_Ini_ImpExp config_exporter( m_configHeap );
	if (config_exporter.export_config(m_configFile) == -1 )
	{
		ACE_ERROR ((GAME_ERROR ACE_TEXT ("%p\n"), m_configFile));
	}
}
