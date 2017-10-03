#pragma once


#include "parameter.h"
#include <vector>
#include <afxmt.h>
#include "BaseException.h"
#include "ProgressMonitor.h"
#include "AlgorithmListener.h"

using namespace std;

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif


namespace PRImA 
{
//Thread func (mustn't be a member function of CAlgorithm)
DWORD WINAPI RunAlgorithmThreadFunc(LPVOID lpParam);

///----------------------------------------------------------------------------------------------------
/// Class CAlgorithm
///
///<summary>
///
/// Base class for runnable algorithm modules.
/// Derived classes must implement the method DoRun().
///
///</summary>
/// CC 17.11.2009 - created
///
class DllExport CAlgorithm : public CProgressMonitor
{
public:
	CAlgorithm();
	CAlgorithm(CUniString name, CUniString description);
	virtual ~CAlgorithm(void);

	///<summary>Returns the number of parameters used for this algorithm</summary>
	inline int			GetParameterCount() { return m_Params.GetSize(); };

	inline CParameter *	GetParameter(int i) { return m_Params.Get(i); };
	int					GetProgress();
	bool				IsRunning();
	bool				IsSuccess();
	virtual bool		CanRun();  //Can the algorithm already be run or are some parameters missing

	///<summary>Label for UIs; default is 'Run'</summary>
	virtual inline const wchar_t * GetRunButtonLabel() { return L"Run"; };

	void				Run();
///----------------------------------------------------------------------------------------------------
/// <summary>
/// Executes the algorithm/method in its own thread.
/// Throws CBaseException if the algorithm is already running
/// </summary>
///
	void				RunAsync();
	virtual bool		IsStoppable() { return false; }; //Can the algorithm handle stop signals
	void				Stop(int timeLimit = 500);
	void				SendStopSignal();
	bool				HasStopSignal();

	///<summary>Returns all params used for this algorithm (note: The parameter map is managed by this CAlgorithm object)</summary>
	inline CParameterMap * GetParams() { return &m_Params; };

	virtual CUniString	GetLogMessage();
	
	CUniString			GetName();

	void AddListener(CAlgorithmListener * listener);
	void RemoveListener(CAlgorithmListener * listener);

protected:
	void			AddParameter(CParameter * param);
	virtual void	DoRun() = 0;
	void			Init();
	void			SetProgress(int progress);

private:
	CParameterMap	m_Params;
	bool			m_Running;
	CUniString		m_Name;
	CUniString		m_Description;
	HANDLE			m_Thread;
	bool			m_StopSignal;
	vector<CAlgorithmListener*> m_Listeners;

protected:
	bool				m_Success;
	CCriticalSection	m_CriticalSect;

};

}

