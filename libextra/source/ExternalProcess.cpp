#include "ExternalProcess.h"
#include <iostream>

namespace PRImA
{

/*
 * Class CExternalProcess
 *
 * Helper class to run external processes.
 *
 * CC 02.07.2012 - copied from Optimizer project
 */

CExternalProcess::CExternalProcess(void)
{
}

CExternalProcess::~CExternalProcess(void)
{
}



/*
* Runs a process using the given command line call.
* 'wait' - If true, the function returns after the process has finished, otherwise it returns immediately.
*/
bool CExternalProcess::RunProcess(CUniString commandLine, bool wait)
{
	int notUsed = 0;
	CUniString processOutput;
	return RunProcess(commandLine, wait, notUsed, false, processOutput, false, INFINITE);
}

/*
 * Runs a process using the given command line call and waits for the process to finish.
 * 'processReturnValue' (out)
 */
bool CExternalProcess::RunProcess(CUniString commandLine, int & processReturnCode, bool showConsole, DWORD waitFor /*= INFINITE*/,
								CExternalProcessListener * timeoutListener  /*= NULL*/)
{
	CUniString processOutput;
	return RunProcess(commandLine, true, processReturnCode, false, processOutput, showConsole, waitFor, timeoutListener);
}

/*
 * Runs a process using the given command line call and waits for the process to finish.
 * 'processReturnValue' (out)
 * 'processOutput' (out)
 */
bool CExternalProcess::RunProcess(CUniString commandLine, int & processReturnCode, CUniString & processOutput, 
								  DWORD waitFor /*= INFINITE*/, CExternalProcessListener * timeoutListener  /*= NULL*/)
{
	return RunProcess(commandLine, true, processReturnCode, true, processOutput, false, waitFor, timeoutListener);
}

/*
 * Runs a process using the given command line call.
 * 'wait' (in) - If true, the function returns after the process has finished, otherwise it returns immediately.
 * 'processReturnValue' (out)
 * 'processOutput' (out)
 * 'readOutput' - If true, the output of the called process will be read
 */
bool CExternalProcess::RunProcess(CUniString commandLine, bool wait, int & processReturnCode, bool readOutput, CUniString & processOutput, bool showConsole, DWORD waitFor,
								CExternalProcessListener * timeoutListener  /*= NULL*/)
{
	HANDLE currentProcessHandle;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES saAttr;

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 

	// Create a pipe for the child process's STDOUT
	HANDLE childStd_OUT_Rd = NULL;
	HANDLE childStd_OUT_Wr = NULL;

	bool copyProcessOutput = wait && readOutput;
	if (copyProcessOutput)
	{
		if (!CreatePipe(&childStd_OUT_Rd, &childStd_OUT_Wr, &saAttr, 0))
			copyProcessOutput = false;
		else
		{
			// Ensure the read handle to the pipe for STDOUT is not inherited.
			if (!SetHandleInformation(childStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
				copyProcessOutput = false;
		}
	}

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	si.hStdError = childStd_OUT_Wr;
	si.hStdOutput = childStd_OUT_Wr;
	if (copyProcessOutput)
		si.dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory( &pi, sizeof(pi) );

	currentProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, true, _getpid() );

			//Set timeout
		/*COMMTIMEOUTS timeoutParams;
		ZeroMemory(&timeoutParams, sizeof(COMMTIMEOUTS));
		timeoutParams.ReadIntervalTimeout = 0;
		timeoutParams.ReadTotalTimeoutMultiplier = 0;
		timeoutParams.ReadTotalTimeoutConstant = 100;
		timeoutParams.WriteTotalTimeoutMultiplier = 0;
		timeoutParams.WriteTotalTimeoutConstant = 0;

		if (!SetCommTimeouts(childStd_OUT_Rd, &timeoutParams))
		{
			DWORD err = GetLastError();
			copyProcessOutput = false;
		}*/

	if( !CreateProcess( NULL,             // No module name (use command line). 
						commandLine.GetBuffer(),// Command line. 
						NULL,             // Process handle not inheritable. 
						NULL,             // Thread handle not inheritable. 
						TRUE,             // Set handle inheritance to TRUE. 
						showConsole ? CREATE_NEW_CONSOLE : DETACHED_PROCESS,                // Creation flags
						NULL,             // Use parent's environment block. 
						NULL,             // Use parent's starting directory. 
						&si,              // Pointer to STARTUPINFO structure.
						&pi )             // Pointer to PROCESS_INFORMATION structure.
					) {
        //printf( "CreateProcess failed (%d).\n", GetLastError() );
        return false;
    }

	//Wait for the process to finish
   if (wait)
   {
	   DWORD res = 0;
	   while (1) 
	   {
			res = WaitForSingleObject(pi.hProcess, waitFor);
			if (timeoutListener == NULL || res != WAIT_TIMEOUT)
				break;
			//Timeout -> Wait or cancel
			if (!timeoutListener->OnExternalProcessTimeout())
				break;
			//int answer = parentForMessageBox->MessageBox(L"Do you want to continue waiting (OK) or cancel the operation?", L"Waiting for Operation to Finish", MB_OKCANCEL);
			//if (answer == IDCANCEL)
			//	break;
	   } 
	   if (res == WAIT_TIMEOUT)
	   {
		   processReturnCode = TIMEOUT;
		
		   if (copyProcessOutput)
			   processOutput = ReadFromPipe(childStd_OUT_Rd);

		   TerminateProcess(pi.hProcess, TIMEOUT);
		   return false;
	   }

	   if (copyProcessOutput)
		   processOutput = ReadFromPipe(childStd_OUT_Rd);

	   DWORD code = -1;
	   if (GetExitCodeProcess(pi.hProcess, &code))
		   processReturnCode = code;
   }


   return true;
}

// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT. 
// Stop when there is no more data. 
CUniString CExternalProcess::ReadFromPipe(HANDLE childStd_OUT_Rd) 
{ 

	DWORD dwRead; //, dwWritten; 
	const int BUFSIZE = 4096;
	CHAR chBuf[BUFSIZE]; 
	BOOL success = FALSE;
	//HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CUniString text;

	//for (;;) 
	//{ 

	//OVERLAPPED overlapped;
	//ZeroMemory(&overlapped, sizeof(OVERLAPPED));
	
	DWORD bytesAvail = 0;
    if (!PeekNamedPipe(childStd_OUT_Rd, NULL, 0, NULL, &bytesAvail, NULL)) {
        std::cout << "Failed to call PeekNamedPipe" << std::endl;
    }
    if (bytesAvail) 
	{
		success = ReadFile( childStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL /*&overlapped*/);
		//if(!success || dwRead == 0) 
		//	break; 

		if (success)
		{
			WaitForSingleObject(childStd_OUT_Rd, 100);

			if (dwRead > 0)
				text.Append(chBuf);
		}

		//success = WriteFile(hParentStdOut, chBuf, 
        //                   dwRead, &dwWritten, NULL);
		//if (!success) 
		//	break; 
	} 
	return text;
} 




} //End namespace