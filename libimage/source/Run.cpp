
#include "Run.h"

namespace PRImA
{

/*
 * Class CRun
 *
 * A simple horizontal run of pixels. used for connected components
 */

/*
 * Constructor
 */
CRun::CRun()
{
	m_pCC = 0;
	m_nY = -1;
	m_nX1 = -1;
	m_nX2 = -1;
}

/*
 * Destructor
 */
CRun::~CRun()
{
}

/*
 * Initialises this run
 */
void CRun::Create(int NewY, int NewX1, int NewX2)
{
	m_nY = NewY;
	m_nX1 = NewX1;
	m_nX2 = NewX2;
}

/*
 * Returns the connected component associated with this run
 */
CConnectedComponent * CRun::GetCC()
{
	return m_pCC;
}

/*
 * Returns the start of the run (left side)
 */
int CRun::GetX1()
{
	return m_nX1;
}

/*
 * Returns the end of the run (right side)
 */
int CRun::GetX2()
{
	return m_nX2;
}

/*
 * Returns the vertical position of this run
 */
int CRun::GetY()
{
	return m_nY;
}

/*
 * Sets the connected component associated with this run
 */
void CRun::SetCC(CConnectedComponent * CC)
{
	m_pCC = CC;
}


} //end namespace