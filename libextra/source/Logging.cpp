#include "Logging.h"

namespace PRImA
{

/*
 * Class CLogging
 *
 * Connects to the PRImA Logger tool (external executable) to write log messages to a text file.
 */

CCriticalSection CLogging::s_CriticalSect;

/*
 * Constructor
 * 
 * 'logFilePath' - Target file path
 * 'bufferSize' - Buffer size of the pipe that connects with the log process
 */
CLogging::CLogging(CUniString logFilePath, CUniString appName, int maxMessageSize, int bufferSize /*= 0*/)
{
	m_IsStarted = false;
	m_MaxMessageSize = maxMessageSize; 
	Startup(logFilePath,appName,m_MaxMessageSize, bufferSize);
}

/*
 * Starts the PRImALogger executable in a new process and connects to it via a pipe
 */
bool CLogging::Startup(CUniString logFilePath, CUniString appName, int maxMessageSize, int bufferSize)
{
	if(m_IsStarted)
	{
		//TODO: Do some checks to see if still running
		return 1;
	}

	m_SaAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	m_SaAttr.bInheritHandle = TRUE; 
	m_SaAttr.lpSecurityDescriptor = NULL; 

	if (! CreatePipe(&m_RdPipe, &m_WrPipe, &m_SaAttr, bufferSize)) {
		printf("CLogging: Stdout pipe creation failed\n"); 
		return 0;
	}

	//CC 11.05.2011 - Set pipe to not to wait if the buffer is full (otherwise the WriteFile freezes when the log process has stopped for some reason)
	//CC 20.09.2012 - Update: WriteFileEx always runs asynchronously and therfore should't block. So we don't need the PIPE_NOWAIT (which might lead to loss of log messages)
	//DWORD mode = PIPE_NOWAIT;
	//SetNamedPipeHandleState(m_WrPipe,  &mode, 0, 0);

	//printf("rdPipe=0x%X, wrPipe=0x%X\n", (LONG)rdPipe, (LONG)wrPipe);

    ZeroMemory( &m_Si, sizeof(m_Si) );
    m_Si.cb = sizeof(m_Si);
    ZeroMemory( &m_Pi, sizeof(m_Pi) );

	m_CurrentProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, true, _getpid() );

	TCHAR cmdLine[128] = { 0 };

	LPWSTR logFilePathStr = logFilePath.GetBuffer();//TEXT("Logs");
	LPWSTR appNameStr = appName.GetBuffer();//TEXT("TestApp");

	wsprintf(cmdLine, L"PRImALogger.exe %d %d %s %s", m_RdPipe, m_CurrentProcessHandle, logFilePathStr, appNameStr );

    if( !CreateProcess( NULL,             // No module name (use command line). 
						cmdLine,          // Command line. 
						NULL,             // Process handle not inheritable. 
						NULL,             // Thread handle not inheritable. 
						TRUE,             // Set handle inheritance to TRUE. 
						DETACHED_PROCESS /*CREATE_NEW_CONSOLE*/,                // No creation flags. 
						NULL,             // Use parent's environment block. 
						NULL,             // Use parent's starting directory. 
						&m_Si,              // Pointer to STARTUPINFO structure.
						&m_Pi )             // Pointer to PROCESS_INFORMATION structure.
					) {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return 0;
    }

	m_IsStarted = true;

	Sleep(10); //Give the Process a moment to start

	return 1;
}

/*
 * Sends a message to the external PRImALogger to write it to a log file
 */
bool CLogging::SendMsg(CUniString msg)
{

	_OVERLAPPED overlapped;
	overlapped.Offset=0;
	overlapped.OffsetHigh=0;

	wchar_t *charptr = new wchar_t[m_MaxMessageSize];

	if (msg.GetLength() >= m_MaxMessageSize)
		msg = msg.Left(m_MaxMessageSize-1);

	wcscpy_s( charptr, m_MaxMessageSize, msg.GetBuffer() );

	CSingleLock lock(&s_CriticalSect, TRUE);
	BOOL res = WriteFileEx(m_WrPipe, (wchar_t *)( charptr  ), sizeof( wchar_t )*m_MaxMessageSize, &overlapped, NULL);
	lock.Unlock();

	if(!res)
	{
		printf("Error writing\n");
		delete [] charptr;
		return 0;
	}
	else
	{
		//printf("Write successful\n");
		delete [] charptr;
		return 1;
	}
}


} //end namespace