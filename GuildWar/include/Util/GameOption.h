
// *-----------------------------------------------------------------
//
// 配置文件读写类
//
// *-----------------------------------------------------------------

#pragma once

class GameOption
{
	friend class ACE_Singleton<GameOption, ACE_Recursive_Thread_Mutex>;

public:
	bool loadOptions();
	void saveOptions();

	const std::string& getLogFile() const { return m_logFile; }
	const std::string& getDatabase() const { return m_database; }

	// Script相关
	const std::string& getScriptDir() const { return m_scriptDir; }
	std::string getTestScriptFile() const { return "./" + m_scriptDir + "/" + m_testScriptFile; }

protected:
	GameOption();
	~GameOption();

private:
	ACE_TCHAR m_configFile[MAXPATHLEN];
	ACE_Configuration_Heap m_configHeap;

	std::string m_logFile;
	std::string m_database;

	// Script相关
	std::string m_scriptDir;
	std::string m_testScriptFile;
};

typedef ACE_Singleton<GameOption, ACE_Recursive_Thread_Mutex> GameOptionSingleton;
#define GAME_OPTION GameOptionSingleton::instance()
