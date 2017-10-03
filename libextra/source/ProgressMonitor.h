#pragma once

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
 * Class CProgressMonitor
 *
 * Base class for progress monitoring
 * Not thread-safe
 */
class DllExport CProgressMonitor
{
public:
	CProgressMonitor(void);
	virtual ~CProgressMonitor(void);

	inline int		GetProgress() { return m_Progress; };
	inline int		GetMin() { return m_Min; };
	inline int		GetMax() { return m_Max; };
	inline int		GetStep() { return m_Step; };
	inline void		SetProgress(int prog) 
	{ 
		m_Progress = prog; 
	};
	void			Step();

protected:
	void			InitDefault();
	inline void		SetMin(int min) { m_Min = min; };
	inline void		SetMax(int max) { m_Max = max; };
	inline void		SetStep(int step) { m_Step = step; };
	inline void		Reset() { m_Progress = m_Min; };

private:
	int m_Progress;
	int m_Min;
	int m_Max;
	int m_Step;
};

}