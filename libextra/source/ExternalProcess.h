#pragma once

#include <process.h>
#include "extrastring.h"

namespace PRImA
{

/*
* Class CExternalProcessListener
*
* Abstract class for listeners that handle OnExternalProcessTimeout()
*
* CC 27/05/2016 - added
*/
class CExternalProcessListener
{
public:
	//Returns true if to continue waiting
	virtual bool OnExternalProcessTimeout() = 0;
};



/*
 * Class CExternalProcess
 *
 * Helper class to run external processes.
 *
 * CC 02.07.2012 - copied from Optimizer project
 */
class CExternalProcess
{
public:
	CExternalProcess(void);
	~CExternalProcess(void);

	static bool RunProcess(CUniString commandLine, bool wait);
	static bool RunProcess(CUniString commandLine, int & processReturnCode, bool showConsole, DWORD waitFor = INFINITE, CExternalProcessListener * timeoutListener = NULL);
	static bool RunProcess(CUniString commandLine, int & processReturnCode, CUniString & processOutput, DWORD waitFor = INFINITE, CExternalProcessListener * timeoutListener = NULL);

	static const int TIMEOUT = 99999999;

private:
	static bool RunProcess(CUniString commandLine, bool wait, int & processReturnCode, bool readOutput, CUniString & processOutput, 
							bool showConsole, DWORD waitFor, CExternalProcessListener * timeoutListener = NULL);

	static CUniString ReadFromPipe(HANDLE childStd_OUT_Rd);

};




} //end namespace