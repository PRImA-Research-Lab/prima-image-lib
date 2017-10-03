#pragma once


#include <afxmt.h>


#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif


namespace PRImA 
{

/*
 * Class Template CSimpleThread
 *
 * Wrapper to start a worker thread.
 *
 * CC 18.09.2010 - created
 */

template <class T>
class DllExport CSimpleThread
{
public:
	CSimpleThread(bool stopThreadOnDestruction = false, bool deleteObjectOnDestruction = false);
	~CSimpleThread(void);

	void Run(T * obj, void (T::*threadFunc)());
	void Run(T * obj, bool (T::*threadFunc)());
	void Run(T * obj, int (T::*threadFunc)());
	bool IsRunning();
	void Stop();

private:
	static DWORD WINAPI ThreadInterfaceFunction(LPVOID lpParam);
	void RunThread();

private:
	bool m_StopThreadOnDestruction;
	bool m_DeleteObjectOnDestruction;
	bool m_Running;
	T * m_Object;

	void (T::*m_ThreadFuncVoid)();		//Thread function with void return type
	bool (T::*m_ThreadFuncBool)();		//Thread function with bool return type
	int (T::*m_ThreadFuncInt)();		//Thread function with int return type

	HANDLE				m_Thread;
	CCriticalSection	m_CriticalSect;
};

/*
 * Constructor
 *
 * 'stopThreadOnDestruction' - If set to true, a running thread will be stopped 
 *                             on destruction of this CSimpleThread object.
 */
template <class T>
CSimpleThread<T>::CSimpleThread(bool stopThreadOnDestruction /*= false*/, bool deleteObjectOnDestruction /*= false*/)
{
	m_StopThreadOnDestruction = stopThreadOnDestruction;
	m_DeleteObjectOnDestruction = deleteObjectOnDestruction;
	m_Running = false;
	m_Thread = NULL;
	m_ThreadFuncVoid = NULL;
	m_ThreadFuncBool = NULL;
	m_ThreadFuncInt = NULL;
}

/*
 * Destructor
 */
template <class T>
CSimpleThread<T>::~CSimpleThread()
{
	if (m_StopThreadOnDestruction)
	{
		Stop();
	}
	if (m_DeleteObjectOnDestruction)
	{
		delete m_Object;
	}
}

/*
 * Starts a thread.
 * Note: If an old thread is still running, the call will be ignored.
 * 
 * 'obj' - Object for which the thread function should be called.
 * 'threadFunc' - Pointer to the thread function. This function will be called, when the thread is started.
 */
template <class T>
void CSimpleThread<T>::Run(T * obj, void (T::*threadFunc)())
{
	CSingleLock singleLock(&m_CriticalSect, TRUE);
	if (!m_Running)
	{
		m_Object = obj;
		m_ThreadFuncVoid = threadFunc;

		RunThread();
	}
	singleLock.Unlock();
}

/*
 * Starts a thread.
 * Note: If an old thread is still running, the call will be ignored.
 * 
 * 'obj' - Object for which the thread function should be called.
 * 'threadFunc' - Pointer to the thread function. This function will be called, when the thread is started.
 */
template <class T>
void CSimpleThread<T>::Run(T * obj, bool (T::*threadFunc)())
{
	CSingleLock singleLock(&m_CriticalSect, TRUE);
	if (!m_Running)
	{
		m_Object = obj;
		m_ThreadFuncBool = threadFunc;

		RunThread();
	}
	singleLock.Unlock();
}

/*
 * Starts a thread.
 * Note: If an old thread is still running, the call will be ignored.
 * 
 * 'obj' - Object for which the thread function should be called.
 * 'threadFunc' - Pointer to the thread function. This function will be called, when the thread is started.
 */
template <class T>
void CSimpleThread<T>::Run(T * obj, int (T::*threadFunc)())
{
	CSingleLock singleLock(&m_CriticalSect, TRUE);
	if (!m_Running)
	{
		m_Object = obj;
		m_ThreadFuncInt = threadFunc;

		RunThread();
	}
	singleLock.Unlock();
}

/*
 * Creates a thread
 */
template <class T>
void CSimpleThread<T>::RunThread()
{
	DWORD idThread;

	m_Running = true;

	m_Thread = CreateThread( 
			NULL,							// default security attributes
			0,								// use default stack size  
			&CSimpleThread<T>::ThreadInterfaceFunction,	// thread function name
			this,							// argument to thread function 
			0,								// use default creation flags 
			&idThread);						// returns the thread identifier 

	if (m_Thread == NULL)
	{
		//TODO error
		m_Running = false;
	}
}

/*
 * Static function that will be passed to the CreatedThread system function.
 * This has to be static, because the CreateThread function doesn't allow
 * non-static member functions as parameter.
 */
template <class T>
DWORD WINAPI CSimpleThread<T>::ThreadInterfaceFunction(LPVOID lpParam)
{
	CSimpleThread<T> * simpleThread = ((CSimpleThread<T>*)lpParam);

	T * obj = simpleThread->m_Object;

	//Call the dedicated worker function
	if (simpleThread->m_ThreadFuncVoid != NULL)
		(obj->*simpleThread->m_ThreadFuncVoid)();
	else if (simpleThread->m_ThreadFuncBool != NULL)
		(obj->*simpleThread->m_ThreadFuncBool)();
	else if (simpleThread->m_ThreadFuncInt != NULL)
		(obj->*simpleThread->m_ThreadFuncInt)();

	CSingleLock singleLock(&(simpleThread->m_CriticalSect), TRUE);
	simpleThread->m_Running = false;
	singleLock.Unlock();

	return 0;
}

/*
 * Returns true, if a thread is running.
 */
template <class T>
bool CSimpleThread<T>::IsRunning()
{
	bool ret = false;
	CSingleLock singleLock(&m_CriticalSect, TRUE);
	ret = m_Running;  //Inside critical section
	singleLock.Unlock();
	return ret;
}

/*
 * Immediately stops the currently running thread.
 */
template <class T>
void CSimpleThread<T>::Stop()
{
	CSingleLock singleLock(&m_CriticalSect, TRUE);
	if (m_Running)
	{
		::TerminateThread(m_Thread, -1000);
		m_Running = false;
	}
	singleLock.Unlock();
}

}