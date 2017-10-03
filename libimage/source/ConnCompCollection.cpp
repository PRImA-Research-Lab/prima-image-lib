#include <math.h>
#include "StdAfx.h"
#include "ConnCompCollection.h"
#include "ExtraMath.h"

namespace PRImA 
{

/*
 * Class CConnCompCollection
 *
 * A collection of connected components.
 * Also provides functions to work with components.
 * 
 * Note:
 *    This class is a replacement for the old class CConnectedComponents.
 *
 * CC 02.09.2010 - Created
 */

/*
 * Constructor
 *
 * 'deleteComponentsOnDestruction' - If this parameter is true, all components within
 *                                   this collection will be delete on the destruction
 *                                   of the collection. (default is true)
 */
CConnCompCollection::CConnCompCollection(bool deleteComponentsOnDestruction /*= true*/)
{
	m_DeleteComponentsOnDestruction = deleteComponentsOnDestruction;
	m_Width = 0;
	m_Height = 0;
	//m_FourConnected = true;
}

/*
 * Destructor
 */
CConnCompCollection::~CConnCompCollection(void)
{
	//Delete components
	if (m_DeleteComponentsOnDestruction)
		DeleteAll();
}

/*
 * Deletes all components of this collection.
 */
void CConnCompCollection::DeleteAll()
{
	for (unsigned int i=0; i<m_Components.size(); i++)
	{
		//Delete the runs as well (is not done in the component)
		for (int j=0; j<m_Components[i]->GetRunCount(); j++)
			delete m_Components[i]->GetRun(j);
		delete m_Components[i];
	}
	m_Components.clear();
}

/*
 * Get the Histogram of BetweenLine in all components.
 */
CHistogram * CConnCompCollection::CreateBetweenLineNNDistHistogram(double RefAngle,double Tolerance)
{
	unsigned int i;
	int j;
	double Angle,Distance;
	CConnectedComponent * CC1, * CC2;
	CHistogram * Ret = new CHistogram(0);
	
	if(Tolerance < 0.0)
		Tolerance = -Tolerance;
	
	for(i = 0; i < m_Components.size(); i++)
	{
		CC1 = m_Components.at(i);
		for(j = 0; j < m_Components.at(i)->GetNeighbourCount(); j++)
		{
			CC2 = CC1->GetNeighbour(j);

			if(CC2 != NULL)
			{
				              
				Angle = CExtraMath::NewAngleTwoPoints(CC1->GetCentroidX(),CC1->GetCentroidY(),CC2->GetCentroidX(),CC2->GetCentroidY());
   
				if(Angle > 90.00 && Angle <= 180.00)
					Angle -= 90.00 ;
				else if(Angle > 180.00 && Angle <=270.00)
					Angle -= 180.00 ;
				else if(Angle > 270.00)
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

/*
 * Get the Histogram of Component Size in all components.
 */
CHistogram * CConnCompCollection::CreateComponentSizeNNHistogram()
{
	CHistogram * Hist = new CHistogram();
	
	unsigned int i;
	int Size;
	
	if(Hist == NULL)
	{
		return NULL;
	}
	
	for(i = 0; i < m_Components.size(); i++)
	{
		Size = m_Components.at(i)->GetSize();
		
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
 * Get the Histogram of Component Height in all components.
 */
CHistogram * CConnCompCollection::CreateComponentHeightNNHistogram()
{
	CHistogram * Hist = new CHistogram();
	
	unsigned int i;
	int Size;
	
	if(Hist == NULL)
		return NULL;
	
	for(i = 0; i < m_Components.size(); i++)
	{
		Size = m_Components.at(i)->GetHeight();
		
		if(Size >= Hist->GetValueCount())
		{
			if(Hist->Resize(Size + 1) == false)
				return false;
		}
		
		if(Hist->Increment(Size) == false)
			return false;
	}
	
	return Hist;
}

/*
* Get the Histogram of component width in all components.
*/
CHistogram * CConnCompCollection::CreateComponentWidthNNHistogram()
{
	CHistogram * hist = new CHistogram();

	unsigned int i;
	int size;

	if (hist == NULL)
		return NULL;

	for (i = 0; i < m_Components.size(); i++)
	{
		size = m_Components.at(i)->GetWidth();

		if (size >= hist->GetValueCount())
		{
			if (hist->Resize(size + 1) == false)
				return false;
		}

		if (hist->Increment(size) == false)
			return false;
	}

	return hist;
}

/*
 * Get the Histogram of Angle in all connected components.
 */
CHistogram * CConnCompCollection::CreateNNAngleHist(double BinAngle)
{
	CConnectedComponent * CC1, * CC2;
	CHistogram * Hist;
	double Angle;
	unsigned int i;
	int j;
	
	if(BinAngle <= 0.0)
	{
		return NULL;
	}
	else if(BinAngle > 180.0)
	{
		return NULL;
	}

	Hist = new CHistogram(int(180.0 / BinAngle));

	for(i = 0; i < m_Components.size(); i++)
	{
		CC1 = m_Components.at(i);
		
		for(j = 0; j < CC1->GetNeighbourCount(); j++)
		{
			CC2 = CC1->GetNeighbour(j);
			if(CC2 != NULL)
			{
				Angle = CExtraMath::NewAngleTwoPoints(CC1->GetCentroidX(),CC1->GetCentroidY(),CC2->GetCentroidX(),CC2->GetCentroidY());
				while(Angle > 180.0)	
					Angle -= 180.0;

				Hist->Increment(int((double(Hist->GetValueCount()) * Angle) / 180.0));
			}
		}
	}
	
	return Hist;
}

/*
 * Get the Histogram of WithinLine in all components.
 */
CHistogram * CConnCompCollection::CreateWithinLineNNDistHistogram(double RefAngle,double Tolerance)
{
	unsigned int i;
	int j;
	double Angle,Distance;
	CConnectedComponent * CC1, * CC2;
	CHistogram * Ret = new CHistogram(0);
	
	if(Tolerance < 0.0)
		Tolerance = -Tolerance;
	
	for(i = 0; i < m_Components.size(); i++)
	{
		CC1 = m_Components.at(i);
		
		for(j = 0; j < m_Components.at(i)->GetNeighbourCount(); j++)
		{
			CC2 = CC1->GetNeighbour(j);
			
			if(CC2 != NULL)
			{
				Angle = CExtraMath::NewAngleTwoPoints(CC1->GetCentroidX(),CC1->GetCentroidY(),CC2->GetCentroidX(),CC2->GetCentroidY());
				Distance = CExtraMath::DistanceTwoPoints(CC1->GetCentroidX(),CC1->GetCentroidY(),CC2->GetCentroidX(),CC2->GetCentroidY());
			   
				if(Angle > 90.00 && Angle <= 180.00)
					Angle -= 90.00 ;
				else if(Angle > 180.00 && Angle <=270.00)
					Angle -= 180.00 ;
				else if(Angle > 270.00)
					Angle -= 270.00 ;

				if(Angle <= RefAngle + Tolerance || (Angle <= RefAngle + Tolerance + 90.00 && Angle >= RefAngle + 90.00))
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

/*
 * Adds the given connected component to this collection
 */
void CConnCompCollection::AddComponent(CConnectedComponent * comp)
{
	if (comp != NULL)
	{
		m_Components.push_back(comp);
		//Update width and height
		if (comp->GetX2() > m_Width)
			m_Width = comp->GetX2();
		if (comp->GetY2() > m_Height)
			m_Height = comp->GetY2();
	}
}

/*
 * Creates a new connected component object for the given run and adds
 * it to this collection.
 */
void CConnCompCollection::AddComponent(CRun * run)
{
	CConnectedComponent * comp = new CConnectedComponent();
	comp->AddRun(run);
	m_Components.push_back(comp);

	//Update width and height
	if (comp->GetX2() > m_Width)
		m_Width = comp->GetX2();
	if (comp->GetY2() > m_Height)
		m_Height = comp->GetY2();
}

/*
 * Retunrs the number of connected components within this collection.
 */
int CConnCompCollection::GetSize()
{
	return (int)m_Components.size();
}

/*
 * Returns the connected component with the specified index.
 * Note: Returns NULL if the index is out of bounds.
 */
CConnectedComponent * CConnCompCollection::GetComponent(int index)
{
	//Boundary check
	if (index < 0 || index >= (int)m_Components.size())
		return NULL;
	return m_Components[index];
}

/*
 * Extracts all connected components of the given binary image
 * and stores them in this collection.
 *
 * 'image' - The source binary image.
 * 'fourConnected' - If true, a 4-connected neighbourhood is used, otherwise an 8-connected one.
 * 'lookForBlack' - If true, black components are extracted, otherwise white components.
 */
bool CConnCompCollection::ExtractComponentsFromImage(COpenCvBiLevelImage * image, 
													 bool fourConnected /*= true*/, 
													 bool lookForBlack /*= true*/)
{
	int width = image->GetWidth();
	int height = image->GetHeight();

	return ExtractComponentsFromImage(image, 0, 0, width-1, height-1, fourConnected, lookForBlack);
}

/*
 * Extracts all connected components of a specified area of the given binary image
 * and stores them in this collection.
 *
 * 'image' - The source binary image.
 * 'x1', 'y1' - Top left corner of selected area
 * 'x2', 'y2' - Bottom right corner of selected area
 * 'fourConnected' - If true, a 4-connected neighbourhood is used, otherwise an 8-connected one.
 * 'lookForBlack' - If true, black components are extracted, otherwise white components.
 */
bool CConnCompCollection::ExtractComponentsFromImage(COpenCvBiLevelImage * image, int x1, int y1, int x2, int y2, 
													 bool fourConnected /*= true*/, bool lookForBlack /*= true*/)
{
	int width = image->GetWidth();
	int height = image->GetHeight();
	//m_FourConnected = fourConnected;
	
	int x, y;
	bool componentColor = lookForBlack;
	bool inRun;

	vector<vector<CRun*>> runs(height);
	int xStart = x1;

	//Iterate through pixel lines
	for(y = y1; y <= y2; y++)
	{
		inRun = false;
		//Find runs in current line
		for(x = x1; x <= x2; x++) //Handle the last pixel after the loop
		{
			//Start of run
			if(image->IsBlack(x,y) == componentColor && !inRun) 
			{
				inRun = true;
				xStart = x;
			}
			//End of run
			else if(image->IsBlack(x,y) != componentColor && inRun)
			{
				inRun = false;
				if(!AddRun(runs, y, xStart, x - 1, fourConnected))
					return false;
			}
		}
		//Handle last pixel of the current line (better performance than doing than within the loop)
		x = x2;
		if (!inRun)
		{
			if (image->IsBlack(x,y)) //Last pixel is a run
			{
				if(!AddRun(runs,y,x,x, fourConnected))
					return false;
			}
		}
		else //inRun
		{
			if (image->IsBlack(x,y)) //Run ends with line
			{
				if(!AddRun(runs, y, xStart, x, fourConnected))
					return false;
			}
			else //Run ends one pixel before line end
			{
				if(!AddRun(runs, y, xStart, x-1, fourConnected))
					return false;
			}
		}
	}

	return true;
}

/*
 * Adds a run of black pixels to the internal data structure
 */
bool CConnCompCollection::AddRun(vector<vector<CRun*>> & runs, int y, int x1, int x2, bool fourConnected)
{
	//Create the new run
	CRun * newRun = new CRun();
	newRun->Create(y,x1,x2);

	runs[y].push_back(newRun);

	//### Connected Components ###
	CRun * currRun;
	CConnectedComponent * overlapComponent;
	
	int overlaps = 0;
	if(y == 0)
	{
		AddComponent(newRun);
	}
	else //if (y>0)
	{
		vector<CRun*> runsOfLastLine = runs[y-1];
		for(unsigned int i = 0; i < runsOfLastLine.size(); i++)
		{
			currRun = runsOfLastLine[i];
	
			if(currRun->GetY()  == newRun->GetY() - 1		//CC is this needed?
				&&
				((	currRun->GetX1() <= newRun->GetX2() &&
					currRun->GetX2() >= newRun->GetX1() &&
					fourConnected
				 )
				 ||
				 (
					currRun->GetX1() <= newRun->GetX2() + 1 &&
					currRun->GetX2() >= newRun->GetX1() - 1 &&
					!fourConnected
				 )
				))
			{
				overlaps++;
				
				if(overlaps == 1)
				{
					// Found First Overlap - Add Run to Above Run's CC
					overlapComponent = currRun->GetCC();
					if(overlapComponent == NULL)
					{
						return false;
					}
					overlapComponent->AddRun(newRun);
				}
				else
				{
					// Found Another Overlap - Merge CCs of two Runs
					if(newRun->GetCC() != currRun->GetCC())
					{
						CConnectedComponent * merged  = newRun->GetCC();
						CConnectedComponent * deleted = currRun->GetCC();
						merged->Merge(deleted);
						DeleteComponent(deleted);
					}
				}
			}
		}
		
		if(overlaps == 0)
		{
			AddComponent(newRun);
		}
	}
	
	return true;
}

/*
 * Removes the given component from the collection and deletes it.
 */
void CConnCompCollection::DeleteComponent(CConnectedComponent * component)
{
	for(unsigned int i = 0; i < m_Components.size(); i++)
	{
		if(m_Components[i] == component)
		{
			delete m_Components[i];
			m_Components.erase(m_Components.begin()+i);
			break;
		}
	}
}

/*
 * Removes the component at the given index from the collection and deletes it.
 */
void CConnCompCollection::DeleteComponent(int index)
{
	delete m_Components[index];
	m_Components.erase(m_Components.begin()+index);
}

/*
 * Creates a subset of this collection that only consists of
 * components that are within the given outline and that have 
 * an area greater or equal the specified minArea.
 *
 * By default the subset collection has the responsibility to delete
 * it's components on destruction.
 */
CConnCompCollection * CConnCompCollection::CreateSubSet(CPointList * outline, 
														 const int minArea /*= 0*/,
														 bool componentsMustBeCompletelyInside /*= true */)
{
	CConnectedComponent * currComp;
	CRun * currRun;
	int j;

	//Create new collection
	CConnCompCollection * subset = new CConnCompCollection();

	//For each component
	for(unsigned int i = 0; i < m_Components.size(); i++)
	{
		currComp = m_Components[i];

		//Check if inside
		bool addToSubset = false;
		if(currComp->GetRunCount() > 0)
		{
			if (componentsMustBeCompletelyInside)
			{
				addToSubset = true;
				for(j = 0; j < currComp->GetRunCount(); j++)
				{
					currRun = currComp->GetRun(j);
					if(!outline->IsPointInside(currRun->GetX1(), currRun->GetY(), true) 
						|| !outline->IsPointInside(currRun->GetX2(), currRun->GetY(), true))
					{
						addToSubset = false;
						break;
					}
				}
			}
			else //Only part of the component must be inside
			{
				addToSubset = false;
				for(j = 0; j < currComp->GetRunCount(); j++)
				{
					currRun = currComp->GetRun(j);
					if(outline->IsPointInside(currRun->GetX1(), currRun->GetY(), true) 
						|| outline->IsPointInside(currRun->GetX2(), currRun->GetY(), true))
					{
						addToSubset = true;
						break;
					}
				}
			}
		}

		//Check area
		if(currComp->GetArea() < minArea)
			addToSubset = false;

		//Add a copy of the current component to the subset
		if(addToSubset)
		{
			CConnectedComponent * copy = currComp->Clone();
			subset->AddComponent(copy);
		}
	}
	return subset;
}

/*
 * Creates a subset of this collection that only consists of
 * components that are within the given component.
 *
 * By default the subset collection has the responsibility to delete
 * it's components on destruction.
 */
CConnCompCollection * CConnCompCollection::CreateSubSet(CConnectedComponent * component, 
														 const int offsetX, const int offsetY)
{
	CConnectedComponent * currComp;
	CRun * currRun;
	int x;
	int j;

	//Create new collection
	CConnCompCollection * subset = new CConnCompCollection();

	//For each component
	for(unsigned int i = 0; i < m_Components.size(); i++)
	{
		currComp = m_Components[i];

		bool addToSubset = false;

		if(currComp->GetRunCount() > 0)
		{
			addToSubset = true;

			//Check if current component within the specified component
			for(j = 0; j < currComp->GetRunCount() && addToSubset; j++)
			{
				currRun = currComp->GetRun(j);

				//Check bounding box first
				if(currRun->GetY() - offsetY < component->GetY1() 
					|| currRun->GetY() - offsetY > component->GetY2() 
					|| currRun->GetX1() - offsetX < component->GetX1() 
					|| currRun->GetX2() - offsetX > component->GetX2())
				{
					addToSubset = false;
				}
				else //Check runs
				{
					for(x = currRun->GetX1(); x <= currRun->GetX2(); x++)
					{
						if(!component->IsIn(x - offsetX, currRun->GetY() - offsetY))
						{
							addToSubset = false;
							break;
						}
					}
				}
			}
		}

		if(addToSubset)
		{
			CConnectedComponent * copy = currComp->Clone();
			subset->AddComponent(copy);
		}
	}
	return subset;
}

/*
 * Find the k nearest neighbours for each component.
 * The neighbours are accessible through the components.
 */
bool CConnCompCollection::FindNearestNeighbours(int k)
{
	int j, x, y;
	bool foundPos;
	int currX,currY,compX,compY,compDist,MAXDIST;
	
	vector<int> dist(k);
	CConnectedComponent ** neigh = new CConnectedComponent * [k];
	
	MAXDIST = int(sqrt(double((m_Width * m_Width) + (m_Height * m_Height)))) + 1;
	
	//SortComponentsX(); //CC Why sort?
	
	for(unsigned i = 0; i < m_Components.size(); i++)
	{
		//Initialize
		for(j = 0; j < k; j++)
		{
			dist[j] = MAXDIST;
			neigh[j] = NULL;
		}

		currX = m_Components[i]->GetCentroidX();
		currY = m_Components[i]->GetCentroidY();
		
		if(m_Components[i]->SetNoNeighbours(k) == false)
		{
			return false;
		}
		
		for(j = 0; j < (int)m_Components.size(); j++)
		{
			if(i != j)
			{
				compX = m_Components[j]->GetCentroidX();
				compY = m_Components[j]->GetCentroidY();
				compDist = (int)sqrt(double(((compX - currX) * (compX - currX)) + ((compY - currY) * (compY - currY))));
				foundPos = false;
				x = 0;
				
				while(x < k && !foundPos)
				{
					if(compDist < dist[x])
					{
						foundPos = true;
						
						for(y = k - 1; y > x; y--)
						{
							dist[y] = dist[y - 1];
							neigh[y] = neigh[y - 1];
						}
						
						dist[x] = compDist;
						neigh[x] = m_Components[j];
					}
					
					x++;
				}
			}
		}
		
		for(j = 0; j < k; j++)
		{
			m_Components[i]->SetNeighbour(j, neigh[j]);
		}

		foundPos = false;
	}
	
	delete [] neigh;

	return true;
}

/*
 * Calculates the average area (width*height) over all connected components
 */
double CConnCompCollection::GetAverageArea()
{
	int total = 0;
	for(unsigned int i = 0; i < m_Components.size(); i++)
		total += m_Components[i]->GetArea();

	return double(total) / double(m_Components.size());
}

/*
 * Calculates the average height off all connected components.
 *
 * 'noiseFilter' - Use a value >= 0 to skip components with an area smaller or equal that value
 */
double CConnCompCollection::GetAverageArea(int noiseFilter /*= 0*/)
{
	unsigned int i;
	long TotalArea = 0;

	if (noiseFilter <= 0)
	{
		for(i = 0; i < m_Components.size(); i++)
			TotalArea += m_Components[i]->GetArea();
		return double(TotalArea) / double(m_Components.size());
	}
	else //Apply noise filter
	{
		int count = 0;
		for(i = 0; i < m_Components.size(); i++)
		{
			if (m_Components[i]->GetArea() > noiseFilter)
			{
				TotalArea += m_Components[i]->GetArea();
				count++;
			}
		}
		if (count > 0)
			return double(TotalArea) / double(count);
		else
			return 1.0;
	}
}

/*
 * Calculates the average height over all connected components
 */
double CConnCompCollection::GetAverageHeight()
{
	int total = 0;
	for(unsigned int i = 0; i < m_Components.size(); i++)
		total += m_Components[i]->GetHeight();

	return double(total) / double(m_Components.size());
}

/*
 * Calculates the average height off all connected components.
 *
 * 'noiseFilter' - Use a value >= 0 to skip components with an area smaller or equal that value
 */
double CConnCompCollection::GetAverageHeight(int noiseFilter /*= 0*/)
{
	unsigned int i;
	long TotalHeight = 0;

	if (noiseFilter <= 0)
	{
		for(i = 0; i < m_Components.size(); i++)
		{
			TotalHeight += m_Components[i]->GetHeight();
		}
		return double(TotalHeight) / double(m_Components.size());
	}
	else //Apply noise filter
	{
		int count = 0;
		for(i = 0; i < m_Components.size(); i++)
		{
			if (m_Components[i]->GetArea() > noiseFilter)
			{
				TotalHeight += m_Components[i]->GetHeight();
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
 * Calculates the average width over all connected components
 */
double CConnCompCollection::GetAverageWidth()
{
	int total = 0;
	for(unsigned int i = 0; i < m_Components.size(); i++)
		total += m_Components[i]->GetWidth();

	return double(total) / double(m_Components.size());
}

/*
 * Calculates the average width off all connected components.
 *
 * 'noiseFilter' - Use a value >= 0 to skip components with an area smaller or equal that value
 */
double CConnCompCollection::GetAverageWidth(int noiseFilter /*= 0*/)
{
   	unsigned int i;
	long TotalWidth = 0;
	double temp_CcCount = 0;

	if (noiseFilter <= 0)
	{
		for(i = 0; i < m_Components.size(); i++)
		{
			TotalWidth += m_Components[i]->GetWidth();
		}
		return double(TotalWidth) / double(m_Components.size());
	}
	else //Apply noise filter
	{
		int count = 0;
		for(i = 0; i < m_Components.size(); i++)
		{
			if (m_Components[i]->GetArea() > noiseFilter)
			{
				TotalWidth += m_Components[i]->GetWidth();
				count++;
			}
		}
		if (count > 0)
			return double(TotalWidth) / double(count);
		else
			return 1.0;
	}


}


/*
 * Generic sort function.
 * Sorts the connected components using the specified comparator function.
 *
 * Example function for sorting by area:
 *
 * bool ComponentComparatorForArea(CConnectedComponent * comp1, CConnectedComponent * comp2)
 * {
 *     return comp1->GetArea() < comp2->GetArea();
 * }
 */
void CConnCompCollection::SortComponents(bool (*comparatorFunction)(CConnectedComponent*,CConnectedComponent*))
{
	if (!m_Components.empty())
		sort(m_Components.begin(), m_Components.end(), comparatorFunction);
}

/*
 * Comparator function for sorting by x.
 */
bool ComponentComparatorByX(CConnectedComponent * comp1, CConnectedComponent * comp2)
{
	return comp1->GetX1()+comp1->GetX2() < comp2->GetX1()+comp2->GetX2();
}

/*
 * Sort the connected components by x.
 */
void CConnCompCollection::SortComponentsByX()
{
	SortComponents(ComponentComparatorByX);
}

/*
 * Returns the maximum x of all components.
 * Note: The value is not cached and will be calculated on each call of this function.
 */
int CConnCompCollection::GetMaxX()
{
	int max = 0;
	for(unsigned int i = 0; i < m_Components.size(); i++)
		if (m_Components[i]->GetX2() > max)
			max = m_Components[i]->GetX2();
	return max;
}

/*
 * Returns the minimum x of all components.
 * Note: The value is not cached and will be calculated on each call of this function.
 */
int CConnCompCollection::GetMinX()
{
	if (m_Components.empty())
		return 0;
	int min = MAXINT;
	for(unsigned int i = 0; i < m_Components.size(); i++)
		if (m_Components[i]->GetX1() < min)
			min = m_Components[i]->GetX1();
	return min;
}

/*
 * Returns the maximum y of all components.
 * Note: The value is not cached and will be calculated on each call of this function.
 */
int CConnCompCollection::GetMaxY()
{
	int max = 0;
	for(unsigned int i = 0; i < m_Components.size(); i++)
		if (m_Components[i]->GetY2() > max)
			max = m_Components[i]->GetY2();
	return max;
}

/*
 * Returns the minimum y of all components.
 * Note: The value is not cached and will be calculated on each call of this function.
 */
int CConnCompCollection::GetMinY()
{
	if (m_Components.empty())
		return 0;
	int min = MAXINT;
	for(unsigned int i = 0; i < m_Components.size(); i++)
		if (m_Components[i]->GetY1() < min)
			min = m_Components[i]->GetY1();
	return min;
}

/*
 * Static mehtod to draw a connected component into a black-and-white image
 */
void CConnCompCollection::DrawComponent(CConnectedComponent * comp, COpenCvBiLevelImage * image)
{
	CRun * run = NULL;
	int y=0;
	int x1=0;
	int x2=0;
	
	for(int r=0; r<comp->GetRunCount(); r++)
	{
		run = comp->GetRun(r);

		y=run->GetY();
		x1=run->GetX1();
		x2=run->GetX2();

		image->DrawLine(x1, y, x2, y, true);
	}
}

/*
 * Static mehtod to draw a connected component into a black-and-white image
 */
void CConnCompCollection::DrawComponent(CConnectedComponent * comp, COpenCvBiLevelImage * image, int offsetX, int offsetY)
{
	CRun * run = NULL;
	int y=0;
	int x1=0;
	int x2=0;
	
	for(int r=0; r<comp->GetRunCount(); r++)
	{
		run = comp->GetRun(r);

		y=run->GetY();
		x1=run->GetX1();
		x2=run->GetX2();

		image->DrawLine(x1+offsetX, y+offsetY, x2+offsetX, y+offsetY, true);
	}
}



} //end namespace