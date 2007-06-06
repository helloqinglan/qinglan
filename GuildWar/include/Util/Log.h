
// *-----------------------------------------------------------------
//
// 日志输出辅助类
//
// *-----------------------------------------------------------------

#pragma once

#define GAME_DEBUG LM_DEBUG, ACE_TEXT ("DEBUG%I ")
#define GAME_ERROR LM_ERROR, ACE_TEXT ("ERROR%I ")

// usage
//ACE_DEBUG ((GAME_DEBUG ACE_TEXT ("Hi, Mom\n")))

class LogManager
{
	friend class ACE_Singleton<LogManager, ACE_Recursive_Thread_Mutex>;

public:
	bool redirectToOStream(ACE_OSTREAM_TYPE* output);
	bool redirectToFile(const char* filename);

protected:
	LogManager();
	~LogManager();

private:
	std::ofstream* m_logStream;
	ACE_OSTREAM_TYPE* m_outputStream;
};

typedef ACE_Singleton<LogManager, ACE_Recursive_Thread_Mutex> LogManagerSingleton;
#define LOG_MANAGER LogManagerSingleton::instance()
