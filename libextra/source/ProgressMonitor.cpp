#include "StdAfx.h"
#include "ProgressMonitor.h"


/*
 * Class CProgressMonitor
 *
 * Base class for progress monitoring.
 * Not thread-safe
 */

namespace PRImA
{

/*
 * Constructor
 */
CProgressMonitor::CProgressMonitor(void)
{
	InitDefault();
}

/*
 * Initialises all fields with default values
 */
void CProgressMonitor::InitDefault()
{
	m_Progress = 0;
	m_Min = 0;
	m_Max = 100;
	m_Step = 1;
}

/*
 * Destructor
 */
CProgressMonitor::~CProgressMonitor(void)
{
}

/*
 * Increase progress by the predefined step width
 */
void CProgressMonitor::Step()
{
	 m_Progress =+ m_Step;
	 if (m_Progress > m_Max)
		 m_Progress = m_Max;
}

} //end namespace