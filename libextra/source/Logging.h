#include "ExtraString.h"
#include <process.h>
#include <afxmt.h>

namespace PRImA 
{

/*
 * Class CLogging
 *
 * Connects to the PRImA Logger tool (external executable) to write log messages to a text file.
 */
class CLogging
{
public:
	CLogging(CUniString logFilePath, CUniString appName, int maxMessageSize, int bufferSize = 0);

	bool SendMsg(CUniString msg);
	bool Startup(CUniString logFilePath, CUniString appName, int maxMessageSize, int bufferSize);

private:

	HANDLE m_RdPipe, m_WrPipe, m_CurrentProcessHandle;
	STARTUPINFO m_Si;
	PROCESS_INFORMATION m_Pi;
	SECURITY_ATTRIBUTES m_SaAttr;

	bool m_IsStarted;
	int m_MaxMessageSize;
	static CCriticalSection	s_CriticalSect;

};

}