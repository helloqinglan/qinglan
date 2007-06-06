
// *-----------------------------------------------------------------
//
// ≈‰÷√Œƒº˛∂¡–¥¿‡
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

protected:
	GameOption();
	~GameOption();

private:
	ACE_TCHAR m_configFile[MAXPATHLEN];
	ACE_Configuration_Heap m_configHeap;

	std::string m_logFile;
	std::string m_database;
};

typedef ACE_Singleton<GameOption, ACE_Recursive_Thread_Mutex> GameOptionSingleton;
#define GAME_OPTION GameOptionSingleton::instance()
