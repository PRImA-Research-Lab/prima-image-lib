#include "StdAfx.h"
#include "Algorithm.h"

namespace PRImA
{

/*
 * Class CAlgorithm
 *
 * Base class for runnable algorithm modules.
 * Derived classes must implement the method DoRun().
 *
 * CC 17.11.2009 - created
 */

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Constructor to create a named algorithm
///
/// <para> 'name' - Name of algorithm/method </para>
/// <para> 'description' - Description of algorithm/method (possibly explaining parameters) </para>
/// </summary>
///
CAlgorithm::CAlgorithm(CUniString name, CUniString description)
{
	m_Name = name;
	m_Description = description;
	Init();
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Constructor (empty name and description)
/// </summary>
///
CAlgorithm::CAlgorithm()
{
	Init();
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Destructor
/// </summary>
///
CAlgorithm::~CAlgorithm(void)
{
	for (int i=0; i<m_Params.GetSize(); i++)
		delete m_Params.Get(i);
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Initialises all fields
/// </summary>
///
void CAlgorithm::Init()
{
	m_Success = false;
	m_Running = false;
	CProgressMonitor::InitDefault();
	m_Thread = NULL;
	m_StopSignal = false;
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Used to determine if the algorithm is ready to run or some parameters are missing.
/// Override to change behaviour. This implementation always returns true.
/// </summary>
///
bool CAlgorithm::CanRun()
{
	return true;
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Adds a parameter for the algorithm/method. Needs to be called before 'Run...'
/// </summary>
///
void CAlgorithm::AddParameter(CParameter * param)
{
	//	param->SetId(m_Params.GetSize()); //CC - Why did I do that?
	m_Params.Add(param);
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Executes the algorithm/method in its own thread.
/// Throws CBaseException if the algorithm is already running
/// </summary>
///
void CAlgorithm::RunAsync()
{
	if (IsRunning())
		throw CBaseException(L"Algorithm already running");
	DWORD idThread;

	m_Running = true;

	m_Thread = CreateThread( 
			NULL,						// default security attributes
            0,							// use default stack size  
            RunAlgorithmThreadFunc,		// thread function name
            this,						// argument to thread function 
            0,							// use default creation flags 
            &idThread);					// returns the thread identifier 

	if (m_Thread == NULL)
	{
		//TODO error
		m_Running = false;
	}
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Stops the algorithm. Tries to stop it gracefully first by sending a signal.
///
/// <para>'timeLimit' - milliseconds the algorithm has to finish in a controlled way.
///                     If the time limit is reached, the thread will be terminated.
///                     Use value -1 to forcfully stop the algorithm </para>
/// </summary>
///
void CAlgorithm::Stop(int timeLimit /*= 500*/)
{
	if (IsRunning())
	{
		if (timeLimit <= 0) //Kill it immedeately
			::TerminateThread(m_Thread, -1000);
		else
		{
			SendStopSignal();
			int waited = 0;
			while (IsRunning() && waited < timeLimit)
			{
				Sleep(50);
				waited += 50;
			}
			if (IsRunning())
				::TerminateThread(m_Thread, -1000); //Waited too long, kill it
		}
		m_Running = false;
	}
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Executes the algorithm/method synchronously (in the current thread).
/// </summary>
///
void CAlgorithm::Run()
{
	CSingleLock singleLock(&m_CriticalSect);
	singleLock.Lock();
	m_Running = true;  //Inside critical section
	m_StopSignal = false;
	CProgressMonitor::Reset();
	singleLock.Unlock();

	try
	{
		DoRun();
	}
	catch (...)
	{
		//TODO
	}

	if (m_Thread != NULL)
		::CloseHandle(m_Thread);

	singleLock.Lock();
	m_Running = false;  //Inside critical section
	singleLock.Unlock();

	//Notify listeners
	for (unsigned int i = 0; i < m_Listeners.size(); i++)
	{
		m_Listeners[i]->AlgorithmFinished(this);
	}
}

/*
 * Thread function that calls 'Run' of the given CAlgorithm object
 * 'lpParam' - Pointer to CAlgorithm object
 */
///----------------------------------------------------------------------------------------------------
/// <summary>
/// Thread function that calls 'Run' of the given CAlgorithm object
///
/// <para>'lpParam' - - Pointer to CAlgorithm object</para>
/// </summary>
///
DWORD WINAPI RunAlgorithmThreadFunc( LPVOID lpParam )
{
	((CAlgorithm*)lpParam)->Run();
	return 0;
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Checks if this algoritm is currently running (for use with 'RunAsync()')
/// </summary>
///
bool CAlgorithm::IsRunning()
{
	bool ret = false;
	CSingleLock singleLock(&m_CriticalSect);
	singleLock.Lock();
	ret = m_Running;  //Inside critical section
	singleLock.Unlock();
	return ret;
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Activates a flag to indicate to the algorithm that it should stop.
/// If the algorithm doesn't react quickly, it might be stopped forcefully.
/// </summary>
///
void CAlgorithm::SendStopSignal()
{
	CSingleLock singleLock(&m_CriticalSect);
	singleLock.Lock();
	m_StopSignal = true;  //Inside critical section
	singleLock.Unlock();
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Checks if a 'stop' flag has been raised. The flag indicates to the algorithm that it should stop.
/// If the algorithm doesn't react quickly, it might be stopped forcefully.
/// </summary>
///
bool CAlgorithm::HasStopSignal()
{
	bool ret = false;
	CSingleLock singleLock(&m_CriticalSect);
	singleLock.Lock();
	ret = m_StopSignal;  //Inside critical section
	singleLock.Unlock();
	return ret;
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Can be called after the algorithm/method has finished to check if it was successful.
/// Note: The algorithm has to set the 'm_Success' field (default is 'false').
/// </summary>
///
bool CAlgorithm::IsSuccess()
{
	bool ret = false;
	CSingleLock singleLock(&m_CriticalSect);
	singleLock.Lock();
	ret = m_Success;  //Inside critical section
	singleLock.Unlock();
	return ret;
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Thread-safe call to GetProgress of CProgressMonitor parent class
/// </summary>
///
int CAlgorithm::GetProgress()
{
	int ret = 0;
	CSingleLock singleLock(&m_CriticalSect);
	singleLock.Lock();
	ret = CProgressMonitor::GetProgress();  //Inside critical section
	singleLock.Unlock();
	return ret;
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Thread-safe call to SetProgress of CProgressMonitor parent class
/// </summary>
///
void CAlgorithm::SetProgress(int progress)
{
	CSingleLock singleLock(&m_CriticalSect);
	singleLock.Lock();
	CProgressMonitor::SetProgress(progress);  //Inside critical section
	singleLock.Unlock();
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Composes a message for logging, including the algorithm name and all parameters
/// </summary>
///
CUniString CAlgorithm::GetLogMessage()
{
	CUniString msg;
	if (!m_Name.IsEmpty())
		msg.Append(m_Name);
	else
		msg.Append(_T("CAlgorithm"));
	for (int i=0; i<m_Params.GetSize(); i++)
	{
		msg.Append(_T(" "));
		msg.Append(m_Params.Get(i)->GetLogMessage());
	}
	return msg;
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Returns the name of the algorithm.
/// </summary>
///
CUniString CAlgorithm::GetName()
{
	return m_Name;
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Add listener.
/// </summary>
///
void CAlgorithm::AddListener(CAlgorithmListener * listener)
{
	m_Listeners.push_back(listener);
}

///----------------------------------------------------------------------------------------------------
/// <summary>
/// Remove listener.
/// </summary>
///
void CAlgorithm::RemoveListener(CAlgorithmListener * listener)
{
	for (unsigned int i=0; i< m_Listeners.size(); i++)
		if (m_Listeners[i] == listener)
		{
			m_Listeners.erase(m_Listeners.begin() + i);
			break;
		}
}



} //end namespace