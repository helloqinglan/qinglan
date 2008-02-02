
// *-----------------------------------------------------------------
//
// 脚本管理器实现
//
// *-----------------------------------------------------------------

#pragma once

class gmMachine;

class ScriptManager
{
	friend class ACE_Singleton<ScriptManager, ACE_Recursive_Thread_Mutex>;

public:
	bool initialize();

	// 执行脚本
	bool executeString(const std::string& script);

	gmMachine* machine() const { return m_scriptMachine; }

protected:
	ScriptManager();
	~ScriptManager();

private:
	gmMachine* m_scriptMachine;
};

typedef ACE_Singleton<ScriptManager, ACE_Recursive_Thread_Mutex> ScriptManagerSingleton;
#define SCRIPTMANAGER ScriptManagerSingleton::instance()
