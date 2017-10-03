#include "ConnectedComponent.h"

#include <afxcoll.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <algorithm.h>

#include "Run.h"

using namespace std;

namespace PRImA
{

/*
 * Class CConnectedComponent
 *
 * Represents a single connected component within a bitonal image.
 * The component is stored in a data structure containing pixel runs. 
 */

/*
 * Constructor
 */
CConnectedComponent::CConnectedComponent()
{
	m_RunCount = 0;
	m_Runs = new CRun * [RUN_INIT];
	m_RunsAlloc = RUN_INIT;
	m_nNeighbours = 0;
	m_pNeighbours = NULL;
}

/*
 * Destructor
 */
CConnectedComponent::~CConnectedComponent()
{
	delete [] m_pNeighbours;
	delete [] m_Runs;
}

/*
 * Creates a deep copy of this connected component.
 * Note: The neighbours will not be copied.
 */
CConnectedComponent * CConnectedComponent::Clone()
{
	CConnectedComponent * copy = new CConnectedComponent();

	for (int i=0; i<m_RunCount; i++)
	{
		CRun * run = new CRun();
		run->Create(m_Runs[i]->GetY(), m_Runs[i]->GetX1(), m_Runs[i]->GetX2());
		copy->AddRun(run);
	}

	return copy;
}

/*
 * Adds the given run to the internal data structure
 * The ownership of the run object is NOT transferred to this connected component.
 * (the runs will not be deleted at destruction)
 */
bool CConnectedComponent::AddRun(CRun * run, bool updateBoundingBox /*= true*/)
{
	//Allocate more memory if necessary
	while(m_RunCount + 1 > m_RunsAlloc)
	{
		CRun ** temp = new CRun * [m_RunsAlloc + RUN_INCR];
		memcpy(temp,m_Runs,sizeof(CRun *) * m_RunsAlloc);
		delete [] m_Runs;
		m_Runs = temp;
		m_RunsAlloc += RUN_INCR;
	}
	
	//Add the new run
	m_Runs[m_RunCount] = run;
	m_RunCount++;
	
	if (updateBoundingBox)
	{
		if(m_RunCount == 1)
		{
			m_X1 = run->GetX1();
			m_X2 = run->GetX2();
			m_Y1 = run->GetY();
			m_Y2 = run->GetY();
		}
		else
		{
			if(run->GetX1() < m_X1)
				m_X1 = run->GetX1();
			if(run->GetX2() > m_X2)
				m_X2 = run->GetX2();
			
			if(run->GetY() < m_Y1)
				m_Y1 = run->GetY();
			else if(run->GetY() > m_Y2)
				m_Y2 = run->GetY();
		}
	}
	
	run->SetCC(this);
	
	return true;
}

/*
 * Calculates the area of the bounding box
 */
int CConnectedComponent::GetArea()
{
	return (m_X2 - m_X1 + 1) * (m_Y2 - m_Y1 + 1);
}

/*
 * Calculates the centre of the bounding box
 */
int CConnectedComponent::GetCentroidX()
{
	return (m_X2 + m_X1) / 2;
}

/*
 * Calculates the centre of the bounding box
 */
int CConnectedComponent::GetCentroidY()
{
	return (m_Y2 + m_Y1) / 2;
}

/*
 * Calculates the centre of the bounding box
 */
int CConnectedComponent::GetCenterX()
{
	return m_X1 + (m_X2 - m_X1) / 2;
}

/*
 * Calculates the centre of the bounding box
 */
int CConnectedComponent::GetCenterY()
{
	return m_Y1 + (m_Y2 - m_Y1) / 2;
}

/*
 * Calculates the height of the bounding box
 */
int CConnectedComponent::GetHeight()
{
	return m_Y2 - m_Y1 + 1;
}

/*
 * Returns the neighbour component with the given index or NULL if out of bounds
 */
CConnectedComponent * CConnectedComponent::GetNeighbour(int index)
{
	if(index < 0 || index >= m_nNeighbours)
	{
		return NULL;
	}
	return m_pNeighbours[index];
}

/*  
 * Returns the number of neighbour components
 */
int CConnectedComponent::GetNeighbourCount()
{
	return m_nNeighbours;
}

/*
 * Returns the number of pixel runs that make up this connected component
 */
int CConnectedComponent::GetRunCount()
{
	return m_RunCount;
}

/*
 * Returns the run with the given index or NULL if out of bounds
 */
CRun * CConnectedComponent::GetRun(int index)
{
	if(index >= m_RunCount || index < 0)
		return NULL;
	else
		return m_Runs[index];
}

/*
 * Calculates the square root of the bounding box area
 */
unsigned CConnectedComponent::GetSize()
{
	return unsigned(sqrt(double((1 + m_X2 - m_X1) * (1 + m_Y2 - m_Y1))));
}

/*
 * Calculates the width of the bounding box
 */
int CConnectedComponent::GetWidth()
{
	return m_X2 - m_X1 + 1;
}

/*
 * Returns position of the left side of the bounding box
 */
int CConnectedComponent::GetX1()
{
	return m_X1;
}

/*
 * Returns position of the right side of the bounding box
 */
int CConnectedComponent::GetX2()
{
	return m_X2;
}

/*
 * Returns position of the top side of the bounding box
 */
int CConnectedComponent::GetY1()
{
	return m_Y1;
}

/*
 * Returns position of the bottom side of the bounding box
 */
int CConnectedComponent::GetY2()
{
	return m_Y2;
}

/*
 * Checks if the given point is inside this connected component.
 * Looks in the pixel runs (not just the bounding box)
 */
bool CConnectedComponent::IsIn(int x, int y)
{
	for(int i = 0; i < m_RunCount; i++)
	{
		if(
			GetRun(i)->GetY() == y
			&&
			GetRun(i)->GetX1() <= x
			&&
			GetRun(i)->GetX2() >= x
		)
			return true;
	}
	return false;
}

/*
 * Merges the given connected component with this one
 */
bool CConnectedComponent::Merge(CConnectedComponent * CC)
{
	int i;

	//Allocate enough memory in one go (otherwise this would be done in AddRun)
	if(m_RunCount + CC->GetRunCount() > m_RunsAlloc)
	{
		int incr = (((m_RunCount + CC->GetRunCount() - m_RunsAlloc) / RUN_INCR) + 1) * RUN_INCR;
		CRun ** temp = new CRun * [m_RunsAlloc + incr];
		memcpy(temp,m_Runs,sizeof(CRun *) * m_RunsAlloc);
		delete [] m_Runs;
		m_Runs = temp;
		m_RunsAlloc += incr;
	}
	
	//Add runs
	for(i = 0; i < CC->GetRunCount(); i++)
	{
		AddRun(CC->GetRun(i), false);
	}

	//Update bounding box
	m_X1 = min(m_X1, CC->GetX1());
	m_X2 = max(m_X2, CC->GetX2());
	m_Y1 = min(m_Y1, CC->GetY1());
	m_Y2 = max(m_Y2, CC->GetY2());

	return true; // Just a bug fix, not necessarily correct :-)
}

/*
 * Sets the neighbour at the given index
 */
bool CConnectedComponent::SetNeighbour(const int index, CConnectedComponent * neighbour)
{
	if(index < 0 || index >= m_nNeighbours)
		return false;
	else
	{
		m_pNeighbours[index] = neighbour;
		return true;
	}
}

/*
 * Sets the number of neighbours
 * Deprecated: Use SetNeighbourCount(...)
 */
bool CConnectedComponent::SetNoNeighbours(const int noNeighbours)
{
	return SetNeighbourCount(noNeighbours);
}

/*
 * Sets the number of neighbours
 * Returns false if the necessary memory could not be allocated
 */
bool CConnectedComponent::SetNeighbourCount(const int count)
{
	delete [] m_pNeighbours;
	m_pNeighbours = new CConnectedComponent * [count];
	
	if(m_pNeighbours == NULL)
	{
		return false;
	}
	
	m_nNeighbours = count;
	return true;
}

/*
 * Sets the number of runs
 * Deprecated: Use SetRunCount(...)
 */
bool CConnectedComponent::SetNoRuns(const int number)
{
	return SetRunCount(number);
}

/*
 * Sets the number of runs
 */
bool CConnectedComponent::SetRunCount(const int count)
{
	delete [] m_Runs;
	m_Runs = new CRun * [count];
	if(m_Runs == NULL)
	{
		return false;
	}
	m_RunCount = count;
	return true;
}

/*
 * Checks if this component is inside the given rectangle.
 */
bool CConnectedComponent::IsInsideRect(int x1, int y1, int x2, int y2)
{
	return m_X1 >= x1 && m_X2 <= x2 && m_Y1 >= y1 && m_Y2 <= y2;
}

/*
 * Checks if the bounding box of this component touches the given rectangle.
 */
bool CConnectedComponent::IsTouchingRect(int x1, int y1, int x2, int y2)
{
	if (	m_X1 >= x1 && m_X1 <= x2 && m_Y1 >= y1 && m_Y1 <= y2
		 || m_X2 >= x1 && m_X2 <= x2 && m_Y1 >= y1 && m_Y1 <= y2
		 || m_X1 >= x1 && m_X1 <= x2 && m_Y2 >= y1 && m_Y2 <= y2
		 || m_X2 >= x1 && m_X2 <= x2 && m_Y2 >= y1 && m_Y2 <= y2)
		 return true;
	if (	x1 >= m_X1 && x1 <= m_X2 && y1 >= m_Y1 && y1 <= m_Y2
		 || x2 >= m_X1 && x2 <= m_X2 && y1 >= m_Y1 && y1 <= m_Y2
		 || x1 >= m_X1 && x1 <= m_X2 && y2 >= m_Y1 && y2 <= m_Y2
		 || x2 >= m_X1 && x2 <= m_X2 && y2 >= m_Y1 && y2 <= m_Y2)
		 return true;
	return false;
}


} //end namespace