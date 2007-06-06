
// *-----------------------------------------------------------------
//
// 日志输出辅助类
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "Util/Log.h"

LogManager::LogManager()
: m_logStream(0), m_outputStream(0)
{
}

LogManager::~LogManager()
{
	if (m_logStream)
		m_logStream->close();
	delete m_logStream;
}

bool LogManager::redirectToOStream(ACE_OSTREAM_TYPE* output)
{
	m_outputStream = output;
	ACE_LOG_MSG->msg_ostream(m_outputStream);
	ACE_LOG_MSG->clr_flags(ACE_Log_Msg::STDERR | ACE_Log_Msg::LOGGER);
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);

	return true;
}

bool LogManager::redirectToFile(const char* filename)
{
	m_logStream = new std::ofstream();
	m_logStream->open(filename, ios::out | ios::app);
	return redirectToOStream(m_logStream);
}
