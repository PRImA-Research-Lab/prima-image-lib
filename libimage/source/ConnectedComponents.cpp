#include <math.h>

#include "ConnectedComponents.h"

#include "ConnectedComponent.h"
#include "ExtraMath.h"
#include "Run.h"

using namespace PRImA;

/*
 * Class CConnectedComponents
 *
 * A collection of connected components.
 * Also provides functions to work with components.
 * 
 * Deprecated:
 *    Replaced by CConnCompCollection
 */

CConnectedComponents::CConnectedComponents()
{
	m_bFourConnected = true;
	m_CcCount = 0;
	m_CCsAlloc = 0;
	m_pCCs = NULL;
	m_nHeight = 0;
	m_RunCount = NULL;
	m_RunAlloc = NULL;
	m_pRuns = NULL;
}

CConnectedComponents::~CConnectedComponents()
{
	DeleteArrays(false);
}

/*
 * deleteUnusedElementsOnly==true: Deletes only the unused allocated connected components and runs (and the arrays).
 */
void CConnectedComponents::DeleteArrays(bool deleteUnusedElementsOnly)
{
	int y;
	int i;
	int start = 0;

	if (m_pRuns != NULL) 
	{
		for(y = 0; y < m_nHeight; y++)
		{
			start = deleteUnusedElementsOnly ? m_RunCount[y] : 0;
			for(i = start; i < m_RunAlloc[y]; i++)
			{
				delete m_pRuns[y][i];
			}
			delete [] m_pRuns[y];
		}
		delete [] m_pRuns;
		delete [] m_RunCount;
		delete [] m_RunAlloc;
		m_pRuns = NULL;
	}

	if (m_pCCs != NULL)
	{
		start = deleteUnusedElementsOnly ? m_CcCount : 0;
		for(i = start; i < m_CCsAlloc; i++)
			delete m_pCCs[i];
		
		delete [] m_pCCs;
		m_pCCs = NULL;
	}
}

// METHODS

bool CConnectedComponents::AddCC()
{
	int i;
	
	while(m_CcCount + 1 > m_CCsAlloc)
	{
		CConnectedComponent ** temp = new CConnectedComponent * [m_CCsAlloc + CC_INCR];
		
		if(temp == NULL)
		{
			return false;
		}
		
		memcpy(temp,m_pCCs,sizeof(CConnectedComponent *) * m_CCsAlloc);
		
		for(i = 0; i < CC_INCR; i++)
		{
			temp[m_CCsAlloc + i] = new CConnectedComponent();
			
			if(temp[m_CCsAlloc + i] == NULL)
			{
				return false;
			}
		}
		
		delete [] m_pCCs;
		m_pCCs = temp;
		
		m_CCsAlloc += CC_INCR;
	}

	m_CcCount++;
	
	return true;
}

bool CConnectedComponents::AddCC(CConnectedComponent * CC)
{
	int i;
	
	while(m_CcCount + 1 > m_CCsAlloc)
	{
		CConnectedComponent ** temp = new CConnectedComponent * [m_CCsAlloc + CC_INCR];
		
		if(temp == NULL)
		{
			return false;
		}
		
		memcpy(temp,m_pCCs,sizeof(CConnectedComponent *) * m_CCsAlloc);
		
		for(i = 0; i < CC_INCR; i++)
		{
			temp[m_CCsAlloc + i] = new CConnectedComponent();
			
			if(temp[m_CCsAlloc + i] == NULL)
			{
				return false;
			}
		}
		
		delete [] m_pCCs;
		m_pCCs = temp;
		
		m_CCsAlloc += CC_INCR;
	}

	delete m_pCCs[m_CcCount];
	m_pCCs[m_CcCount] = CC;

	m_CcCount++;
	
	return true;
}

bool CConnectedComponents::AddNewCC(CConnectedComponent * CC , COpenCvBiLevelImage * Image)
{
	int i;
	
	m_nWidth = Image->GetWidth() ;
	m_nHeight = Image->GetHeight() ;

	while(m_CcCount + 1 > m_CCsAlloc)
	{
		CConnectedComponent ** temp = new CConnectedComponent * [m_CCsAlloc + CC_INCR];
		
		if(temp == NULL)
		{
			return false;
		}
		
		memcpy(temp,m_pCCs,sizeof(CConnectedComponent *) * m_CCsAlloc);
		
		for(i = 0; i < CC_INCR; i++)
		{
			temp[m_CCsAlloc + i] = new CConnectedComponent();
			
			if(temp[m_CCsAlloc + i] == NULL)
			{
				return false;
			}
		}
		
		delete [] m_pCCs;
		m_pCCs = temp;
		
		m_CCsAlloc += CC_INCR;
	}

	delete m_pCCs[m_CcCount];
	m_pCCs[m_CcCount] = CC;

	m_CcCount++;
	
	return true;
}


bool CConnectedComponents::AddCC(CRun * Run)
{
	int i;
	
	while(m_CcCount + 1 > m_CCsAlloc)
	{
		CConnectedComponent ** temp = new CConnectedComponent * [m_CCsAlloc + CC_INCR];
		
		if(temp == NULL)
		{
			return false;
		}
		
		memcpy(temp,m_pCCs,sizeof(CConnectedComponent *) * m_CCsAlloc);
		
		for(i = 0; i < CC_INCR; i++)
		{
			temp[m_CCsAlloc + i] = new CConnectedComponent();
			
			if(temp[m_CCsAlloc + i] == NULL)
			{
				return false;
			}
		}
		
		delete [] m_pCCs;
		m_pCCs = temp;
		
		m_CCsAlloc += CC_INCR;
	}
	
	if(m_pCCs[m_CcCount]->AddRun(Run) == false)
	{
		return false;
	}
	
	m_CcCount++;
	
	return true;
}

/*
 * Adds the given run to the internal list of runs.
 * If the run touches an existing connected component, it is added to this component.
 * Otherwise a new component is added for the single run.
 */
bool CConnectedComponents::AddRun(int y, int x1, int x2, bool trackCCs /*= false*/)
{
	int i;
	
	if(m_RunCount == NULL)
		m_RunCount = new int[m_nHeight];
	
	//Allocate memory
	while(m_RunCount[y] + 1 > m_RunAlloc[y])
	{
		//Create new array of runs (with increased size)
		CRun ** temp = new CRun * [m_RunAlloc[y] + RUN_INCR];
		
		if(temp == NULL)
		{
			return false;
		}
		
		//Copy the old runs to the new array
		memcpy(temp,m_pRuns[y],sizeof(CRun *) * m_RunAlloc[y]);
		
		//Initialize the remaining array slots with new CRuns
		for(i = 0; i < RUN_INCR; i++)
		{
			temp[m_RunAlloc[y] + i] = new CRun();
			
			if(temp[m_RunAlloc[y] + i] == NULL)
			{
				return false;
			}
		}
		
		//Delete the old array
		delete [] m_pRuns[y];

		m_pRuns[y] = temp;
		m_RunAlloc[y] += RUN_INCR;
	}
	
	//Create the new run
	m_pRuns[y][m_RunCount[y]]->Create(y,x1,x2);
	m_RunCount[y]++;
	
	//### Connected Components ###
	CRun * ThisRun, * CompRun;
	CConnectedComponent * CC;
	
	ThisRun = m_pRuns[y][m_RunCount[y] - 1];
	
	int overlaps = 0;
	
	if(y == 0) //First line -> always add a connected component for the run
	{
		if(!AddCC(ThisRun))
			return false;
		if (trackCCs)
			m_CompsOfCurrentScanline.insert(m_pCCs[m_CcCount-1]);
	}
	else //if (y>0)  //Check for overlaps
	{
		for(i = 0; i < m_RunCount[y - 1]; i++)
		{
			CompRun = m_pRuns[y - 1][i];
	
			if (CompRun->GetY()  == ThisRun->GetY() - 1		//CC is this needed?
				&& ( (   CompRun->GetX1() <= ThisRun->GetX2()
					  && CompRun->GetX2() >= ThisRun->GetX1() 
					  && m_bFourConnected
					 ) 
					||
					 (
						 CompRun->GetX1() <= ThisRun->GetX2() + 1
					  && CompRun->GetX2() >= ThisRun->GetX1() - 1
					  && !m_bFourConnected
					 )
				   ))	//Found overlap
			{
				overlaps++;
				if(overlaps == 1) // Found First Overlap -> Add Run to Above Run's CC
				{
					CC = CompRun->GetCC();
					if(CC == NULL)
						return false;
					CC->AddRun(ThisRun);
					if (trackCCs)
						m_CompsOfCurrentScanline.insert(CC);
				}
				else // Found Another Overlap -> Merge CCs of two Runs
				{
					if(ThisRun->GetCC() != CompRun->GetCC())
					{
						CConnectedComponent * Merged  = ThisRun->GetCC();
						CConnectedComponent * Deleted = CompRun->GetCC();
						Merged->Merge(Deleted);
						if (trackCCs && !m_CompsOfLastScanline.empty())
						{
							set<CConnectedComponent*>::iterator it = m_CompsOfLastScanline.find(Deleted);
							if (it != m_CompsOfLastScanline.end()) //Component to delete is in set
								m_CompsOfLastScanline.erase(it);
						}
						if (trackCCs && !m_CompsOfCurrentScanline.empty())
						{
							set<CConnectedComponent*>::iterator it = m_CompsOfCurrentScanline.find(Deleted);
							if (it != m_CompsOfCurrentScanline.end()) //Component to delete is in set
								m_CompsOfCurrentScanline.erase(it);
						}
						DeleteCC(Deleted);
					}
				}
			}
		}
		
		if(overlaps == 0) //No overlap found -> Create connected component for this run
		{
			if(!AddCC(ThisRun))
				return false;
			if (trackCCs)
				m_CompsOfCurrentScanline.insert(m_pCCs[m_CcCount-1]);
		}
	}
	
	return true;
}


// Po modified the function with new Angle
CHistogram * CConnectedComponents::CreateBetweenLineNNDistHistogramWithNewAngle(double RefAngle,double Tolerance)
{
	int i, j;
	double Angle,Distance;
	CConnectedComponent * CC1, * CC2;
	CHistogram * Ret = new CHistogram(0);
	
	if(Tolerance < 0.0)
		Tolerance = -Tolerance;
	
	for(i = 0; i < m_CcCount; i++)
	{
		CC1 = m_pCCs[i];
		for(j = 0; j < m_pCCs[i]->GetNeighbourCount(); j++)
		{
			CC2 = CC1->GetNeighbour(j);

			if(CC2 != NULL)
			{
				//Angle = CExtraMath::AngleTwoPoints(CC1->GetCentroidX(),CC1->GetCentroidY(),CC2->GetCentroidX(),CC2->GetCentroidY());
                
				Angle = CExtraMath::NewAngleTwoPoints(CC1->GetCentroidX(),CC1->GetCentroidY(),CC2->GetCentroidX(),CC2->GetCentroidY());
   
				if(Angle > 90.00 && Angle <= 180.00)
					Angle -= 90.00 ;
				else if(Angle > 180.00 && Angle <=270.00)
					Angle -= 180.00 ;
				else
					Angle -= 270.00 ;



				Distance = CExtraMath::DistanceTwoPoints(CC1->GetCentroidX(),CC1->GetCentroidY(),CC2->GetCentroidX(),CC2->GetCentroidY());
			
				if(Angle >= RefAngle - Tolerance && Angle <= RefAngle + Tolerance)
				{
					if(int(Distance) >= Ret->GetValueCount())
					{
						Ret->Resize(int(Distance) + 1);
					}
					Ret->Increment(int(Distance));
				}
			}
		}
	}
	return Ret;
}




CHistogram * CConnectedComponents::CreateBetweenLineNNDistHistogram(double RefAngle,double Tolerance)
{
	int i, j;
	double Angle,Distance;
	CConnectedComponent * CC1, * CC2;
	CHistogram * Ret = new CHistogram(0);
	
	if(Tolerance < 0.0)
		Tolerance = -Tolerance;
	
	for(i = 0; i < m_CcCount; i++)
	{
		CC1 = m_pCCs[i];
		for(j = 0; j < m_pCCs[i]->GetNeighbourCount(); j++)
		{
			CC2 = CC1->GetNeighbour(j);

			if(CC2 != NULL)
			{
				Angle = CExtraMath::AngleTwoPoints(CC1->GetCentroidX(),CC1->GetCentroidY(),CC2->GetCentroidX(),CC2->GetCentroidY());
				Distance = CExtraMath::DistanceTwoPoints(CC1->GetCentroidX(),CC1->GetCentroidY(),CC2->GetCentroidX(),CC2->GetCentroidY());
			
				if(Angle >= RefAngle - Tolerance && Angle <= RefAngle + Tolerance)
				{
					if(int(Distance) >= Ret->GetValueCount())
					{
						Ret->Resize(int(Distance) + 1);
					}
					Ret->Increment(int(Distance));
				}
			}
		}
	}
	return Ret;
}

CHistogram * CConnectedComponents::CreateComponentSizeHistogram()
{
	CHistogram * Hist = new CHistogram();
	
	int i;
	int Size;
	
	if(Hist == NULL)
	{
		return NULL;
	}
	
	for(i = 0; i < m_CcCount; i++)
	{
		Size = m_pCCs[i]->GetSize();
		
		if(Size >= Hist->GetValueCount())
		{
			if(Hist->Resize(Size + 1) == false)
			{
				return false;
			}
		}
		
		if(Hist->Increment(Size) == false)
		{
			return false;
		}
	}
	
	return Hist;
}

/*
 * Connected component analysis for given image
 *
 * 'stopObject' (optional) - CAlgorithm object that is to be checked if the analysis has been cancelled.
 */
bool CConnectedComponents::CreateFromImage(COpenCvBiLevelImage * Image, bool FourConnected /* =true */, 
										   bool LookForBlack /* =true */, int minArea /* =0 */,
										   CAlgorithm * stopObject /*= NULL*/,
										   CProgressMonitor * progressMonitor /*= NULL*/)
{
	int Width = Image->GetWidth();
	int Height = Image->GetHeight();
	
	int x, y;
	int i;
	bool LookForColour = LookForBlack;


	// DELETE PREVIOUSLY ALLOCATED RUNS & CCS
	for(y = 0; y < m_nHeight; y++)
	{
		for(i = 0; i < m_RunCount[y]; i++)
		{
			delete m_pRuns[y][i];
		}
		delete [] m_pRuns[y];
	}
	delete [] m_pRuns;
	delete [] m_RunCount;
	delete [] m_RunAlloc;
	
	for(i = 0; i < m_CcCount; i++)
		delete m_pCCs[i];
	delete [] m_pCCs;
	
	// MAKE NEW ALLOCATIONS
	
	m_nHeight = Height;
	m_nWidth  = Width;
	
	m_pRuns = new CRun ** [Height];
	
	if(m_pRuns == NULL)
	{
		return false;
	}
	
	m_RunCount = new int[Height];
	
	if(m_RunCount == NULL)
	{
		return false;
	}
	
	m_RunAlloc = new int[Height];
	
	if(m_RunAlloc == NULL)
	{
		return false;
	}
	
	for(y = 0; y < m_nHeight; y++)
	{
		m_pRuns[y] = new CRun * [RUN_INIT];
		if(m_pRuns[y] == NULL)
		{
			return false;
		}

		for(i = 0; i < RUN_INIT; i++)
		{
			m_pRuns[y][i] = new CRun();
			if(m_pRuns[y][i] == NULL)
			{
				return false;
			}
		}
		
		m_RunCount[y] = 0;
		m_RunAlloc[y] = RUN_INIT;
	}
	
	m_pCCs = new CConnectedComponent * [CC_INIT];
	if(m_pCCs == NULL)
	{
		return false;
	}
	
	for(i = 0;i < CC_INIT; i++)
	{
		m_pCCs[i] = new CConnectedComponent();
		if(m_pCCs[i] == NULL)
		{
			return false;
		}
	}
	m_CCsAlloc = CC_INIT;


	//Find components
	try
	{
		bool InColour;
		int xStart = 0;
		m_bFourConnected = FourConnected;
		// Look for runs line by line
		for(y = 0; y < m_nHeight; y++)
		{
			//Cancelled?
			if (stopObject != NULL && stopObject->HasStopSignal())
			{
				return false;
			}

			InColour = false;
			if (minArea > 0) //(for size filter)
			{
				m_CompsOfLastScanline.clear();
				for (set<CConnectedComponent*>::iterator it=m_CompsOfCurrentScanline.begin(); it != m_CompsOfCurrentScanline.end(); it++)
					m_CompsOfLastScanline.insert(*it);
				m_CompsOfCurrentScanline.clear();
			}
			
			for(x = 0; x < Width; x++)
			{
				if(Image->IsBlack(x,y) == LookForColour && !InColour)
				{
					// Found START OF RUN
					if(x == Width - 1) // RUN of WIDTH 1 at END of LINE
					{
						if(!AddRun(y,x,x,minArea>0))
						{
							return false;
						}
					}
					else
					{
						InColour = true;
						xStart = x;
					}
				}
				else if(x == Width - 1 && InColour && Image->IsBlack(x,y) == LookForColour)
				{
					//End of line == END OF RUN
					InColour = false;
					if(!AddRun(y, xStart, x, minArea>0))
					{
						return false;
					}
				}
				else if(InColour && Image->IsBlack(x,y) != LookForColour)
				{
					//Found END OF RUN
					InColour = false;
					if(!AddRun(y, xStart, x - 1, minArea>0))
					{
						return false;
					}
				}
			}

			//Check for components to filter
			if (minArea > 0)
			{
				list<CConnectedComponent*> toDelete;
				// Look for components in the last scanline that are not in the current scanline
				for (set<CConnectedComponent*>::iterator itLast=m_CompsOfLastScanline.begin(); itLast != m_CompsOfLastScanline.end(); itLast++)
				{
					CConnectedComponent * compInLast = (*itLast);
					bool found = false;
					for (set<CConnectedComponent*>::iterator itCurr=m_CompsOfCurrentScanline.begin(); itCurr != m_CompsOfCurrentScanline.end(); itCurr++)
					{
						if ((*itCurr) == compInLast)
						{
							found = true;
							break;
						}
					}
					if (!found) //Component not in current scanline -> Check if to filter
					{
						if (compInLast->GetArea() < minArea)
							toDelete.push_back(compInLast);
					}
				}
				//Delete components
				for (list<CConnectedComponent*>::iterator it = toDelete.begin(); it != toDelete.end(); it++)
				{
					CConnectedComponent * comp = (*it);
					//Delete runs first
					for (int i=0; i<comp->GetRunCount(); i++)
						DeleteRun(comp->GetRun(i));

					DeleteCC(comp);
				}
			}

			//Progress
			if (progressMonitor != NULL)
			{
				progressMonitor->SetProgress((int)((double)y/double(m_nHeight)*100.0));
			}
		}
	}
	catch (CMemoryException * )
	{
		return false;
	}

	return true;
}

CHistogram * CConnectedComponents::CreateNNAngleHist(double BinAngle)
{
	CConnectedComponent * CC1, * CC2;
	CHistogram * Hist;
	double Dist;
	int i, j;
	
	if(BinAngle <= 0.0)
	{
		return NULL;
	}
	else if(BinAngle > 180.0)
	{
		return NULL;
	}

	Hist = new CHistogram(int(180.0 / BinAngle));

	for(i = 0; i < m_CcCount; i++)
	{
		CC1 = m_pCCs[i];
		
		for(j = 0; j < CC1->GetNeighbourCount(); j++)
		{
			CC2 = CC1->GetNeighbour(j);
			if(CC2 != NULL)
			{
				Dist = CExtraMath::AngleTwoPoints(CC1->GetCentroidX(),CC1->GetCentroidY(),CC2->GetCentroidX(),CC2->GetCentroidY());
				while(Dist < 0.0)
					Dist += 180.0;
				while(Dist >= 180.0)
					Dist -= 180.0;

				Hist->Increment(int((double(Hist->GetValueCount()) * Dist) / 180.0));
			}
		}
	}
	
	return Hist;
}


// Create NNAngleHist with new Angle 
CHistogram * CConnectedComponents::CreateNNNewAngleHist(double BinAngle)
{
	CConnectedComponent * CC1, * CC2;
	CHistogram * Hist;
	double Dist;
	int i, j;
	
	if(BinAngle <= 0.0)
	{
		return NULL;
	}
	else if(BinAngle > 180.0)
	{
		return NULL;
	}

	Hist = new CHistogram(int(180.0 / BinAngle));

	for(i = 0; i < m_CcCount; i++)
	{
		CC1 = m_pCCs[i];
		
		for(j = 0; j < CC1->GetNeighbourCount(); j++)
		{
			CC2 = CC1->GetNeighbour(j);
			if(CC2 != NULL)
			{
				Dist = CExtraMath::AngleTwoPoints(CC1->GetCentroidX(),CC1->GetCentroidY(),CC2->GetCentroidX(),CC2->GetCentroidY());
				while(Dist < 0.0)
					Dist += 180.0;
				while(Dist >= 180.0)
					Dist -= 180.0;

				Hist->Increment(int((double(Hist->GetValueCount()) * Dist) / 180.0));
			}
		}
	}
	
	return Hist;
}

bool CConnectedComponents::CreateSubSet(CConnectedComponents * CC, const int LowerBound, const int UpperBound)
{
	int Width = CC->GetWidth();
	int Height = CC->GetHeight();
	int Size;
	CConnectedComponent * TempCC;
	CRun * TempRun;
	int y;
	int i, j;

	// DELETE PREVIOUSLY ALLOCATED RUNS & CCS
	for(y = 0; y < m_nHeight; y++)
	{
		for(i = 0; i < m_RunCount[y]; i++)
		{
			delete m_pRuns[y][i];
		}
		delete [] m_pRuns[y];
	}
	delete [] m_pRuns;
	delete [] m_RunCount;
	delete [] m_RunAlloc;
	
	for(i = 0; i < m_CcCount; i++)
		delete m_pCCs[i];
	delete [] m_pCCs;
	
	// MAKE NEW ALLOCATIONS
	
	m_nHeight = Height;
	m_nWidth  = Width;
	
	m_pRuns = new CRun ** [Height];
	
	if(m_pRuns == NULL)
	{
		return false;
	}
	
	m_RunCount = new int[Height];
	
	if(m_RunCount == NULL)
	{
		return false;
	}
	
	m_RunAlloc = new int[Height];
	
	if(m_RunAlloc == NULL)
	{
		return false;
	}
	
	for(y = 0; y < m_nHeight; y++)
	{
		m_pRuns[y] = new CRun * [RUN_INIT];
		if(m_pRuns[y] == NULL)
		{
			return false;
		}

		for(i = 0; i < RUN_INIT; i++)
		{
			m_pRuns[y][i] = new CRun();
			if(m_pRuns[y][i] == NULL)
			{
				return false;
			}
		}
		
		m_RunCount[y] = 0;
		m_RunAlloc[y] = RUN_INIT;
	}
	
	m_pCCs = new CConnectedComponent * [CC_INIT];
	if(m_pCCs == NULL)
	{
		return false;
	}
	
	for(i = 0;i < CC_INIT; i++)
	{
		m_pCCs[i] = new CConnectedComponent();
		if(m_pCCs[i] == NULL)
		{
			return false;
		}
	}
	m_CCsAlloc = CC_INIT;

	m_bFourConnected = CC->GetFourConnected();

	for(i = 0; i < CC->GetCcCount(); i++)
	{
		TempCC = CC->GetCC(i);
		if(TempCC == NULL)
		{
			return false;
		}
		Size = TempCC->GetSize();
		
		if(Size >= LowerBound && Size <= UpperBound)
		{
			for(j = 0; j < TempCC->GetRunCount(); j++)
			{
				TempRun = TempCC->GetRun(j);
				AddRun(TempRun->GetY(),TempRun->GetX1(),TempRun->GetX2());
			}
		}
	}

	return true;
}

/*
 * Creates a subset of the given compontens using the specified polygon.
 * All connected components completely lying inside the polygon will be
 * part of the subset.
 */
bool CConnectedComponents::CreateSubSet(CConnectedComponents * CC, CPointList * Reg, const int MinArea /*=0*/,
										bool componentsMustBeCompletelyInside /*= true*/)
{
	int Width = CC->GetWidth();
	int Height = CC->GetHeight();
	CConnectedComponent * TempCC;
	CRun * TempRun;
	int y;
	int i, j;

	// DELETE PREVIOUSLY ALLOCATED RUNS & CCS
	for(y = 0; y < m_nHeight; y++)
	{
		for(i = 0; i < m_RunCount[y]; i++)
		{
			delete m_pRuns[y][i];
		}
		delete [] m_pRuns[y];
	}
	delete [] m_pRuns;
	delete [] m_RunCount;
	delete [] m_RunAlloc;
	
	for(i = 0; i < m_CcCount; i++)
		delete m_pCCs[i];
	delete [] m_pCCs;
	
	// MAKE NEW ALLOCATIONS
	
	m_nHeight = Height;
	m_nWidth  = Width;
	
	m_pRuns = new CRun ** [Height];
	
	if(m_pRuns == NULL)
	{
		return false;
	}
	
	m_RunCount = new int[Height];
	
	if(m_RunCount == NULL)
	{
		return false;
	}
	
	m_RunAlloc = new int[Height];
	
	if(m_RunAlloc == NULL)
	{
		return false;
	}
	
	for(y = 0; y < m_nHeight; y++)
	{
		m_pRuns[y] = new CRun * [RUN_INIT];
		if(m_pRuns[y] == NULL)
		{
			return false;
		}

		for(i = 0; i < RUN_INIT; i++)
		{
			m_pRuns[y][i] = new CRun();
			if(m_pRuns[y][i] == NULL)
			{
				return false;
			}
		}
		
		m_RunCount[y] = 0;
		m_RunAlloc[y] = RUN_INIT;
	}
	
	m_pCCs = new CConnectedComponent * [CC_INIT];
	if(m_pCCs == NULL)
	{
		return false;
	}
	
	for(i = 0;i < CC_INIT; i++)
	{
		m_pCCs[i] = new CConnectedComponent();
		if(m_pCCs[i] == NULL)
		{
			return false;
		}
	}
	m_CCsAlloc = CC_INIT;

	//Compute the subset
	m_bFourConnected = CC->GetFourConnected();

	//For each component
	for(i = 0; i < CC->GetCcCount(); i++)
	{
		TempCC = CC->GetCC(i);
		if(TempCC == NULL)
		{
			return false;
		}
		
		bool IsIn;

		if(TempCC->GetRunCount() > 0)
		{
			if (componentsMustBeCompletelyInside)
			{
				IsIn = true;
				for(j = 0; j < TempCC->GetRunCount(); j++)
				{
					TempRun = TempCC->GetRun(j);
					if(Reg->IsPointInside(TempRun->GetX1(), TempRun->GetY(), true) 
							&& Reg->IsPointInside(TempRun->GetX2(), TempRun->GetY(), true))
						continue;
					else
					{
						IsIn = false;
						break;
					}
				}
			}
			else //Only part of the component must be inside
			{
				IsIn = false;
				for(j = 0; j < TempCC->GetRunCount(); j++)
				{
					TempRun = TempCC->GetRun(j);
					if(Reg->IsPointInside(TempRun->GetX1(), TempRun->GetY(), true) 
							|| Reg->IsPointInside(TempRun->GetX2(), TempRun->GetY(), true))
					{
						IsIn = true;
						break;
					}
				}
			}
		}
		else
			IsIn = false;

		if(TempCC->GetArea() < MinArea)
			IsIn = false;

		if(IsIn)
		{
			AddCC();
			CConnectedComponent * CC = GetCC(GetCcCount() - 1);
			for(j = 0; j < TempCC->GetRunCount(); j++)
				CC->AddRun(TempCC->GetRun(j));
		}
	}

	return true;
}

bool CConnectedComponents::CreateSubSet(CConnectedComponents * CCs, CConnectedComponent * CC, const int OffsetX, const int OffsetY)
{
	printf("OffsetX = %d\tOffsetY = %d\n", OffsetX, OffsetY);
	int Width = CCs->GetWidth();
	int Height = CCs->GetHeight();
	CConnectedComponent * TempCC;
	CRun * TempRun;
	int x, y;
	int i, j;

	// DELETE PREVIOUSLY ALLOCATED RUNS & CCS
	for(y = 0; y < m_nHeight; y++)
	{
		for(i = 0; i < m_RunCount[y]; i++)
		{
			delete m_pRuns[y][i];
		}
		delete [] m_pRuns[y];
	}
	delete [] m_pRuns;
	delete [] m_RunCount;
	delete [] m_RunAlloc;
	
	for(i = 0; i < m_CcCount; i++)
		delete m_pCCs[i];
	delete [] m_pCCs;
	
	// MAKE NEW ALLOCATIONS
	
	m_nHeight = Height;
	m_nWidth  = Width;
	
	m_pRuns = new CRun ** [Height];
	
	if(m_pRuns == NULL)
	{
		return false;
	}
	
	m_RunCount = new int[Height];
	
	if(m_RunCount == NULL)
	{
		return false;
	}
	
	m_RunAlloc = new int[Height];
	
	if(m_RunAlloc == NULL)
	{
		return false;
	}
	
	for(y = 0; y < m_nHeight; y++)
	{
		m_pRuns[y] = new CRun * [RUN_INIT];
		if(m_pRuns[y] == NULL)
		{
			return false;
		}

		for(i = 0; i < RUN_INIT; i++)
		{
			m_pRuns[y][i] = new CRun();
			if(m_pRuns[y][i] == NULL)
			{
				return false;
			}
		}
		
		m_RunCount[y] = 0;
		m_RunAlloc[y] = RUN_INIT;
	}
	
	m_pCCs = new CConnectedComponent * [CC_INIT];
	if(m_pCCs == NULL)
	{
		return false;
	}
	
	for(i = 0; i < CC_INIT; i++)
	{
		m_pCCs[i] = new CConnectedComponent();
		if(m_pCCs[i] == NULL)
		{
			return false;
		}
	}
	m_CCsAlloc = CC_INIT;

	m_bFourConnected = CCs->GetFourConnected();

	for(i = 0; i < CCs->GetCcCount(); i++)
	{
		TempCC = CCs->GetCC(i);
		if(TempCC == NULL)
		{
			return false;
		}
		
		bool IsIn;

		if(TempCC->GetRunCount() > 0)
		{
			IsIn = true;

			for(j = 0; j < TempCC->GetRunCount() && IsIn; j++)
			{
				TempRun = TempCC->GetRun(j);

				if(TempRun->GetY() - OffsetY < CC->GetY1() || TempRun->GetY() - OffsetY > CC->GetY2() || TempRun->GetX1() - OffsetX < CC->GetX1() || TempRun->GetX2() - OffsetX > CC->GetX2())
					IsIn = false;
				else
				{
					for(x = TempRun->GetX1(); x <= TempRun->GetX2() && IsIn; x++)
					{
						if(!CC->IsIn(x - OffsetX, TempRun->GetY() - OffsetY))
							IsIn = false;
					}
				}
			}
		}
		else
			IsIn = false;

		if(IsIn)
		{
			AddCC();
			CConnectedComponent * CC = GetCC(GetCcCount() - 1);
			for(j = 0; j < TempCC->GetRunCount(); j++)
				CC->AddRun(TempCC->GetRun(j));
		}
	}

	return true;
}

CHistogram * CConnectedComponents::CreateWithinLineNNDistHistogram(double RefAngle,double Tolerance)
{
	int i, j;
	double Angle,Distance;
	CConnectedComponent * CC1, * CC2;
	CHistogram * Ret = new CHistogram(0);
	
	if(Tolerance < 0.0)
		Tolerance = -Tolerance;
	
	for(i = 0; i < m_CcCount; i++)
	{
		CC1 = m_pCCs[i];
		
		for(j = 0; j < m_pCCs[i]->GetNeighbourCount(); j++)
		{
			CC2 = CC1->GetNeighbour(j);
			
			if(CC2 != NULL)
			{
				Angle = CExtraMath::AngleTwoPoints(CC1->GetCentroidX(),CC1->GetCentroidY(),CC2->GetCentroidX(),CC2->GetCentroidY());
				Distance = CExtraMath::DistanceTwoPoints(CC1->GetCentroidX(),CC1->GetCentroidY(),CC2->GetCentroidX(),CC2->GetCentroidY());
			
				if(Angle >= RefAngle - Tolerance && Angle <= RefAngle + Tolerance)
				{
					if(int(Distance) >= Ret->GetValueCount())
						Ret->Resize(int(Distance) + 1);

					Ret->Increment(int(Distance));
				}
			}
		}
	}
	return Ret;
}

void CConnectedComponents::DeleteRun(CRun * run)
{
	int y = run->GetY();
	for(int i = 0; i < m_RunCount[y]; i++)
	{
		if(m_pRuns[y][i] == run)
		{
			delete m_pRuns[y][i];
			
			for(int x = i + 1; x < m_RunCount[y]; x++)
			{
				m_pRuns[y][x - 1] = m_pRuns[y][x];
			}
			m_RunCount[y]--;
			m_pRuns[y][m_RunCount[y]] = new CRun();
			return;
		}
	}
}

void CConnectedComponents::DeleteCC(CConnectedComponent * CC)
{
	int i;
	
	for(i = 0; i < m_CcCount; i++)
	{
		if(m_pCCs[i] == CC)
		{
			delete m_pCCs[i];
			
			//CC 28.10.2009  if the CC aren't sorted, we don't need to shift each array element
			//for(x = i + 1; x < m_nCCs; x++)
			//{
			//	m_pCCs[x - 1] = m_pCCs[x];
			//}

			//Move the last CC the the deleted position
			m_CcCount--;
			if (i < m_CcCount)
				m_pCCs[i] = m_pCCs[m_CcCount];
			
			break;
		}
	}
	
	m_pCCs[m_CcCount] = new CConnectedComponent();
}

void CConnectedComponents::DeleteCC(int index)
{
	delete m_pCCs[index];
	m_CcCount--;
	//Move the last CC the the deleted position
	if (index < m_CcCount)
		m_pCCs[index] = m_pCCs[m_CcCount];

	m_pCCs[m_CcCount] = new CConnectedComponent();
}

bool CConnectedComponents::FindNearestNeighbours(int K)
{
	int i, j, x, y;
	bool FoundPos;
	int ThisX,ThisY,CompX,CompY,CompDist,MAXDIST;
	
	int * Dist = new int[K];
	CConnectedComponent ** Neigh = new CConnectedComponent * [K];
	
	MAXDIST = int(sqrt(double((m_nWidth * m_nWidth) + (m_nHeight * m_nHeight)))) + 1;
	
	//SortComponentsX(); //CC Why sort?
	
	for(i = 0; i < m_CcCount; i++)
	{
		for(j = 0; j < K; j++)
		{
			Dist[j] = MAXDIST;
			Neigh[j] = NULL;
		}

		ThisX = m_pCCs[i]->GetCentroidX();
		ThisY = m_pCCs[i]->GetCentroidY();
		
		if(m_pCCs[i]->SetNoNeighbours(K) == false)
		{
			return false;
		}
		
		for(j = 0; j < m_CcCount; j++)
		{
			if(i != j)
			{
				CompX = m_pCCs[j]->GetCentroidX();
				CompY = m_pCCs[j]->GetCentroidY();
				CompDist = int(sqrt(double(((CompX - ThisX) * (CompX - ThisX)) + ((CompY - ThisY) * (CompY - ThisY)))));
				FoundPos = false;
				x = 0;
				
				while(x < K && !FoundPos)
				{
					if(CompDist < Dist[x])
					{
						FoundPos = true;
						
						for(y = K - 1; y > x; y--)
						{
							Dist[y] = Dist[y - 1];
							Neigh[y] = Neigh[y - 1];
						}
						
						Dist[x] = CompDist;
						Neigh[x] = m_pCCs[j];
					}
					
					x++;
				}
			}
		}
		
		for(j = 0; j < K; j++)
		{
			m_pCCs[i]->SetNeighbour(j,Neigh[j]);
//			printf("Neighbour %d - Dist %d\n", j, Dist[j]);
		}

//		Dist = int(sqrt(double(((CompX - ThisX) * (CompX - ThisX)) + ((CompY - ThisY) * (CompY - ThisY)))));
		FoundPos = false;
	}
	
	delete [] Neigh;
	delete [] Dist;

	return true;
}

/*
 * Calculates the average height off all connected components.
 *
 * 'noiseFilter' - Use a value >= 0 to skip components with an area smaller or equal that value
 */
double CConnectedComponents::GetAverageArea(int noiseFilter /*= 0*/)
{
	int i;
	long TotalArea = 0;

	if (noiseFilter <= 0)
	{
		for(i = 0; i < m_CcCount; i++)
			TotalArea += m_pCCs[i]->GetArea();
		return double(TotalArea) / double(m_CcCount);
	}
	else //Apply noise filter
	{
		int count = 0;
		for(i = 0; i < m_CcCount; i++)
		{
			if (m_pCCs[i]->GetArea() > noiseFilter)
			{
				TotalArea += m_pCCs[i]->GetArea();
				count++;
			}
		}
		if (count > 0)
			return double(TotalArea) / double(count);
		else
			return 1.0;
	}
}

// Po did some modification on GetAverageArea
double CConnectedComponents::GetAverageAreaN()
{
	int i;
	long TotalArea = 0;

	int temp_his[30], temp_num[30];

	for(i = 0 ; i < 30 ; i ++)
	{
	   temp_his[i] = i * 100 ;
	   temp_num[i] = 0 ;
	}

    temp_his[29] = 20000000 ;

	for(i = 0; i < m_CcCount; i++)
	{
		TotalArea += m_pCCs[i]->GetArea();
	
		for(int j = 1 ; j < 30 ; j ++)
		{
		     if(m_pCCs[i]->GetArea() >= temp_his[j-1] && m_pCCs[i]->GetArea() < temp_his[j])
				 temp_num[j]++ ;
		}
	
	}

	return double(TotalArea) / double(m_CcCount);
}

/*
 * Calculates the average height off all connected components.
 *
 * 'noiseFilter' - Use a value >= 0 to skip components with an area smaller or equal that value
 */
double CConnectedComponents::GetAverageHeight(int noiseFilter /*= 0*/)
{
	int i;
	long TotalHeight = 0;

	if (noiseFilter <= 0)
	{
		for(i = 0; i < m_CcCount; i++)
		{
			TotalHeight += m_pCCs[i]->GetY2() - m_pCCs[i]->GetY1() + 1;
		}
		return double(TotalHeight) / double(m_CcCount);
	}
	else //Apply noise filter
	{
		int count = 0;
		for(i = 0; i < m_CcCount; i++)
		{
			if (m_pCCs[i]->GetArea() > noiseFilter)
			{
				TotalHeight += m_pCCs[i]->GetY2() - m_pCCs[i]->GetY1() + 1;
				count++;
			}
		}
		if (count > 0)
			return double(TotalHeight) / double(count);
		else
			return 1.0;
	}
}


/*
 * Calculates the average width off all connected components.
 *
 * 'noiseFilter' - Use a value >= 0 to skip components with an area smaller or equal that value
 */
double CConnectedComponents::GetAverageWidth(int noiseFilter /*= 0*/)
{
   	int i;
	long TotalWidth = 0;
	double temp_CcCount = 0;

	if (noiseFilter <= 0)
	{
		for(i = 0; i < m_CcCount; i++)
		{
			TotalWidth += m_pCCs[i]->GetX2() - m_pCCs[i]->GetX1() + 1;
		}
		return double(TotalWidth) / double(m_CcCount);
	}
	else //Apply noise filter
	{
		int count = 0;
		for(i = 0; i < m_CcCount; i++)
		{
			if (m_pCCs[i]->GetArea() > noiseFilter)
			{
				TotalWidth += m_pCCs[i]->GetX2() - m_pCCs[i]->GetX1() + 1;
				count++;
			}
		}
		if (count > 0)
			return double(TotalWidth) / double(count);
		else
			return 1.0;
	}


}
CConnectedComponent * CConnectedComponents::GetCC(int Index)
{
	if(Index >= m_CcCount)
		return NULL;
	else
		return m_pCCs[Index];
}

bool CConnectedComponents::GetFourConnected() const
{
	return m_bFourConnected;
}

int CConnectedComponents::GetHeight()
{
	return m_nHeight;
}

int CConnectedComponents::GetIndex(CConnectedComponent * CC)
{
	int i;

	for(i = 0; i < m_CcCount; i++)
	{
		if(m_pCCs[i] == CC)
			return i;
	}

	return -1;
}

int CConnectedComponents::GetCcCount()
{
	return m_CcCount;
}

int CConnectedComponents::GetRunCount(int Y)
{
	return m_RunCount[Y];
}

CRun * CConnectedComponents::GetRun(int Y, int Index)
{
	return m_pRuns[Y][Index];
}

int CConnectedComponents::GetWidth() const
{
	return m_nWidth;
}

int CConnectedComponents::GetCCMaxX()
{
	 int temp_Value = -1 ;
     
	 for(int i = 0 ; i < m_CcCount ; i++ )
	 {
		 if(this->GetCC(i)->GetX2() > temp_Value)
			 temp_Value = this->GetCC(i)->GetX2() ;
	 
	 }

	 return temp_Value ;

}

int CConnectedComponents::GetCCMinX()
{
	 int temp_Value = 1000000 ;
     
	 for(int i = 0 ; i < m_CcCount ; i++ )
	 {
		 if(this->GetCC(i)->GetX1() < temp_Value)
			 temp_Value = this->GetCC(i)->GetX1() ;
	 
	 }

	 return temp_Value ;

}

int CConnectedComponents::GetCCMaxY()
{
	 int temp_Value = -1 ;
     
	 for(int i = 0 ; i < m_CcCount ; i++ )
	 {
		 if(this->GetCC(i)->GetY2() > temp_Value)
			 temp_Value = this->GetCC(i)->GetY2() ;
	 
	 }

	 return temp_Value ;

}

int CConnectedComponents::GetCCMinY()
{
     int temp_Value = 1000000 ;
     
	 for(int i = 0 ; i < m_CcCount ; i++ )
	 {
		 if(this->GetCC(i)->GetY1() < temp_Value)
			 temp_Value = this->GetCC(i)->GetY1() ;
	 
	 }

	 return temp_Value ;
}



void CConnectedComponents::SetSize(const unsigned NewWidth, const unsigned NewHeight)
{
	int i;
	
	for(i = 0; i < m_nHeight; i++)
	{
		delete [] m_pRuns[i];
	}
	delete [] m_pRuns;
	delete [] m_RunCount;
	delete [] m_RunAlloc;
		
	m_nHeight = NewHeight;
	m_nWidth  = NewWidth;
	m_RunCount = new int[m_nHeight];
	m_RunAlloc = new int[m_nHeight];
	m_pRuns = new CRun ** [m_nHeight];
	memset(m_RunCount, 0, sizeof(int) * m_nHeight);
	memset(m_RunAlloc, 0, sizeof(int) * m_nHeight);
	memset(m_pRuns, 0, sizeof(CRun **) * m_nHeight);
}

/*
 * Sort by x center
 */
void CConnectedComponents::SortComponentsX()
{
	bool Changed = true;
	int i;
	CConnectedComponent * temp, * CC1, * CC2;
	
	while(Changed)
	{
		Changed = false;
		
		for(i = 0; i < m_CcCount - 1; i++)
		{
			CC1 = m_pCCs[i    ];
			CC2 = m_pCCs[i + 1];
			
			if
			(
				(CC1->GetX2() + CC1->GetX1())  >  
				(CC2->GetX2() + CC2->GetX1()) 
			)
			{
				temp          = m_pCCs[i];
				m_pCCs[i    ] = m_pCCs[i + 1];
				m_pCCs[i + 1] = temp;
				Changed = true;
			}
		}
	}
}

/*
 * Sorts by the deviation from the average height.
 */
void CConnectedComponents::SortComponentsSizeMedium()
{
	bool Changed = true;
	int i;
	CConnectedComponent * temp, * CC1, * CC2;
	
	while(Changed)
	{
		Changed = false;
		
		for(i = 0; i < m_CcCount - 1; i++)
		{
			CC1 = m_pCCs[i    ];
			CC2 = m_pCCs[i + 1];
			
			//CC 03.08.2010 - Changed GetAverageHeight to GetAveragewidth because
			//                the GetAverageHeight function was faulty and returned
			//                the average width. To have no impact on existing code
			//                when correcting the function, I changed here to 
			//                GetAverageWidth, though this is obviously wrong.
			if
			(
				abs(CC1->GetY2() - CC1->GetY1() - GetAverageWidth()) >
				abs(CC2->GetY2() - CC2->GetY1() - GetAverageWidth())
			)
			{
				temp          = m_pCCs[i];
				m_pCCs[i    ] = m_pCCs[i + 1];
				m_pCCs[i + 1] = temp;
				Changed = true;
			}
		}
	}
}

/*
 * Retunts the connected component that is touching the given connected compontent (or NULL).
 * 'cc' - the component to use
 * 'bestMatch' - If true and the given component overlaps more than one component
 *               The component will be returned which contains the center of the given component.
 * 'xOffset' - x offset added to the given component
 * 'yOffset' - y offset added to the given component
 */
CConnectedComponent * CConnectedComponents::FindOverlappingComponent(CConnectedComponent * cc, 
																	 bool bestMatch,
																	 int xOffset, int yOffset)
{
	int i,j,k;
	int xCenter = cc->GetX1() + (cc->GetX2()-cc->GetX1()) / 2 + xOffset;
	int yCenter = cc->GetY1() + (cc->GetY2()-cc->GetY1()) / 2 + yOffset;
	CConnectedComponent * curr;
	CRun *run1, *run2;
	CConnectedComponent * firstOverlapComponent = NULL;
	for (i=0; i<m_CcCount; i++)
	{
		curr = m_pCCs[i];
		//Check bounding box
		if (cc->GetX1()+xOffset > curr->GetX2()
			|| curr->GetX1() > cc->GetX2()+xOffset
			|| cc->GetY1()+yOffset > curr->GetY2()
			|| curr->GetY1() > cc->GetY2()+yOffset)
			continue;
		//Check for overlap
		bool foundOverlap = false;
		bool centerInside = false;
		for (j=0; j<curr->GetRunCount(); j++)
		{
			run1 = curr->GetRun(j);
			if (!foundOverlap)
			{
				for (k=0; k<cc->GetRunCount(); k++)
				{
					run2 = cc->GetRun(k);
					if (run2->GetY()+yOffset > run1->GetY()) //cannot reach run1 in this loop
						break;
					if (run1->GetY() > run2->GetY()+yOffset) //y doesn't match -> continue
						continue;
					//y does match -> check x
					if (   run1->GetX1() >= run2->GetX1()+xOffset && run1->GetX1() <= run2->GetX2()+xOffset
						|| run1->GetX2() >= run2->GetX1()+xOffset && run1->GetX2() <= run2->GetX2()+xOffset
						|| run2->GetX1()+xOffset >= run1->GetX1() && run2->GetX1()+xOffset <= run1->GetX2()
						|| run2->GetX2()+xOffset >= run1->GetX1() && run2->GetX2()+xOffset <= run1->GetX2())
					{
						//Overlapping connected component found
						if (!bestMatch || centerInside)
							return curr; 
						foundOverlap = true;
						if (firstOverlapComponent==NULL) //Keep the first component for later (we need it, the center is not inside any component)
							firstOverlapComponent = curr;
						break;
					}
				}
			}
			//If bestMatch is wanted, look if the center is inside the current run
			if (bestMatch && !centerInside)
			{
				//Check if center is inside run
				if (yCenter == run1->GetY() && xCenter >= run1->GetX1() && xCenter <= run1->GetX2())
				{
					centerInside = true;
					if (foundOverlap) //Overlap already found
						return curr;  //  We can return immedeately
				}
			}
		}
	}
	//If bestMatch is wanted and the center is not inside any component,
	//we return the first found overlapping component (or NULL, if there is none).
	return firstOverlapComponent;
}

/*
 * Looks for a connected component at the given position
 * 'areaFilter' - Minimum size of connected component (width by height)
 * 'fastSearch' - If true, only the bounding box of the connected components are checked, otherwise the content (black pixels)
 */
CConnectedComponent * CConnectedComponents::FindComponent(int x, int y, int xOffset /*= 0*/, int yOffset /*= 0*/,
														  int areaFilter /*= 0*/, bool fastSearch /*= true*/)
{
	if (!fastSearch) //Detailed search
		return FindComponentUsingRuns(x, y, xOffset, yOffset, areaFilter);

	//Fast search
	CConnectedComponent * curr;
	for (int i=0; i<m_CcCount; i++)
	{
		curr = m_pCCs[i];
		//Check bounding box
		if (   x+xOffset >= curr->GetX1()
			&& x+xOffset <= curr->GetX2() 
			&& y+yOffset >= curr->GetY1()
			&& y+yOffset <= curr->GetY2())
		{
			if (areaFilter <= 0 || curr->GetWidth() * curr->GetHeight() < areaFilter)
				return curr;
		}
	}
	return NULL;
}

/*
 * Looks for a connected component at the given position. Use the black pixel runs to check.
 * 'areaFilter' - Minimum size of connected component (width by height)
 */
CConnectedComponent * CConnectedComponents::FindComponentUsingRuns(int x, int y, int xOffset /*= 0*/, int yOffset /*= 0*/,
																int areaFilter /*= 0*/)
{
	CConnectedComponent * curr;
	for (int i = 0; i<m_CcCount; i++)
	{
		curr = m_pCCs[i];
		//Check bounding box first
		if (x + xOffset >= curr->GetX1()
			&& x + xOffset <= curr->GetX2()
			&& y + yOffset >= curr->GetY1()
			&& y + yOffset <= curr->GetY2())
		{
			if (areaFilter <= 0 || curr->GetWidth() * curr->GetHeight() < areaFilter)
			{
				//Now check runs
				for (int r = 0; r < curr->GetRunCount(); r++)
				{
					CRun * run = curr->GetRun(r);
					if (run == NULL)
						continue;
					if (y + yOffset == run->GetY()
						&& x + xOffset >= run->GetX1()
						&& x + xOffset <= run->GetX2())
						return curr;
				}
			}
		}				
	}
	return NULL;
}