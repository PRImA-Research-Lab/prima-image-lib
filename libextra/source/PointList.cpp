#include "PointList.h"

namespace PRImA
{

/*
 * Class CPointList
 *
 * A linked list of 2D points, usually used for polygons
 *
 */

/*
 * Constructor
 */
CPointList::CPointList(void)
{
	m_BbUpToDate = false;
	m_InboundRectUpToDate = false;
	m_Modified = false;
	m_HeadPoint = NULL;
	m_PointCount = 0;
	m_TailPoint = NULL;
	m_BbX1 = -1;
	m_BbX2 = -1;
	m_BbY1 = -1;
	m_BbY2 = -1;
	m_nArea = -1;
	m_CentroidX = -1;
	m_CentroidY = -1;
	m_Synchronize = false;
	m_CriticalSect = NULL;
	m_OldCriticalSect = NULL;
}

/*
 * Destructor
 */
CPointList::~CPointList(void)
{
	DeletePoints();
	if (m_CriticalSect != m_OldCriticalSect)
		delete m_OldCriticalSect;
	delete m_CriticalSect;
}

/*
 * Adds and returns a new point. (thread-safe)
 * Ownership of the point stays with this list (it will be deleted at destruction)
 * Updates point count and invalidates the cached bounding box
 */
CPolygonPoint * CPointList::AddPoint()
{
    CPolygonPoint * NewPoint = new CPolygonPoint(this);
    NewPoint->SetNextPoint(NULL);

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

    if(m_PointCount == 0)
    {
        m_HeadPoint = NewPoint;
        NewPoint->SetPrevPoint(NULL);
    }
    else
    {
        m_TailPoint->SetNextPoint(NewPoint);
        NewPoint->SetPrevPoint(m_TailPoint);
    }

    m_TailPoint = NewPoint;

    m_PointCount++;
	SetBBOutdated();

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

    return NewPoint;
}

/*
 * Adds and returns a new point with the given coordinates. (thread-safe)
 * Ownership of the point stays with this list (it will be deleted at destruction)
 * Updates point count and invalidates the cached bounding box
 */
CPolygonPoint * CPointList::AddPoint(int X, int Y)
{
	CPolygonPoint * NewPoint = AddPoint();
	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}
	NewPoint->SetX(X);
	NewPoint->SetY(Y);
	SetBBOutdated();
	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

    return NewPoint;
}

/*
 * Copies all points from the given list to this list (points are cloned; this list is emptied before copying).
 * (thread-safe)
 */
void CPointList::CopyPoints(CPointList & r)
{
	CPolygonPoint * Pt;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	DeletePoints();

	Pt = r.GetHeadPoint();

	while(Pt != NULL)
	{
		AddPoint(Pt->GetX(), Pt->GetY());
		Pt = Pt->GetNextPoint();
	}
	SetBBOutdated();

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}
}

//AppendPointList - 23/08/11 AJF
/*
 * Adds all points of the given point list
 */
/*void CPointList::AppendPointList(CPointList * r)
{
	//If list is empty copy in the list in its entirety
	if(m_HeadPoint==NULL)
		CopyPoints(r);
	else
	{
		int totalNewPoints = r->GetNoPoints();
		m_TailPoint->SetNextPoint( r->GetHeadPoint() );		
		m_PointCount += totalNewPoints;
		SetBBOutdated();
	}
}*/

//Clear - 07/09/11 AJF
//Clears a point list but leaves the contents alone so if they are a part of another list they will not be affected
void CPointList::Clear()
{
	m_BbUpToDate = false;
	m_InboundRectUpToDate = false;
	m_Modified = false;
	m_HeadPoint = NULL;
	m_PointCount = 0;
	m_TailPoint = NULL;
	m_BbX1 = -1;
	m_BbX2 = -1;
	m_BbY1 = -1;
	m_BbY2 = -1;
	m_nArea = -1;
	m_Synchronize = false;
	m_CriticalSect = NULL;
	m_OldCriticalSect = NULL;
}

/*
 * Copies the points of the given list into this list. (thread-safe)
 * Note: Deletes the old points before copying.
 */
void CPointList::CopyPoints(CPointList * r)
{
	CPolygonPoint * Pt;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	DeletePoints();

	Pt = r->GetHeadPoint();

	while(Pt != NULL)
	{
		AddPoint(Pt->GetX(), Pt->GetY());
		Pt = Pt->GetNextPoint();
	}
	SetBBOutdated();

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}
}

/*
 * Removes the given point from the list and deletes it. (thread-safe)
 */
void CPointList::DeletePoint(CPolygonPoint * deletePoint)
{
	CPolygonPoint * NextPoint, * PrevPoint;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	NextPoint = deletePoint->GetNextPoint();
	PrevPoint = deletePoint->GetPrevPoint();
	if(NextPoint == NULL)
		m_TailPoint = PrevPoint;
	else
		NextPoint->SetPrevPoint(PrevPoint);
	if(PrevPoint == NULL)
		m_HeadPoint = NextPoint;
	else
		PrevPoint->SetNextPoint(NextPoint);

	//CC 26.01.2010 - somewhere the parentlist of points is NULL - added this as workaround
	if (PrevPoint != NULL)
		PrevPoint->SetParentList(this);
	if (NextPoint != NULL)
		NextPoint->SetParentList(this);
	delete deletePoint;
	m_PointCount--;
	SetBBOutdated();

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}
}

/*
 * Removes the point at the given position from the list and deletes it. (thread-safe)
 */
void CPointList::DeletePoint(int Index)
{
	int i = 0;

	CPolygonPoint * CurrPoint, * NextPoint, * PrevPoint;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	if(m_PointCount == 0 || Index >= m_PointCount || Index < 0)
	{
		if (unlock)
		{
			singleLock->Unlock();
			delete singleLock;
		}
		return;
	}
	else if(Index == 0 && m_PointCount == 1)
	{
		CurrPoint = m_HeadPoint;
		m_HeadPoint = NULL;
		m_TailPoint = NULL;
		delete CurrPoint;
	}
	else if(Index == 0)
	{
		CurrPoint = m_HeadPoint;
		m_HeadPoint = CurrPoint->GetNextPoint();
		delete CurrPoint;
		m_HeadPoint->SetPrevPoint(NULL);
	}
	else if(Index == m_PointCount - 1)
	{
		CurrPoint = m_TailPoint;
		m_TailPoint = CurrPoint->GetPrevPoint();
		delete CurrPoint;
		m_TailPoint->SetNextPoint(NULL);
	}
	else
	{
		CurrPoint = m_HeadPoint;

		while(i < Index)
		{
			CurrPoint = CurrPoint->GetNextPoint();
			i++;
		}

		PrevPoint = CurrPoint->GetPrevPoint();
		NextPoint = CurrPoint->GetNextPoint();

		NextPoint->SetPrevPoint(PrevPoint);
		PrevPoint->SetNextPoint(NextPoint);
		delete CurrPoint;
	}

	if(m_PointCount > 0)
		m_PointCount--;
	SetBBOutdated();

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}
}

/*
 * Deletes all points (thread-safe)
 */
void CPointList::DeletePoints()
{
	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	CPolygonPoint * CurrPoint = m_HeadPoint;

	while(CurrPoint != NULL)
	{
		CPolygonPoint * NextPoint = CurrPoint->GetNextPoint();
		delete CurrPoint;
		CurrPoint = NextPoint;
	}

	m_HeadPoint = NULL;
	m_TailPoint = NULL;
	m_PointCount = 0;
	SetBBOutdated();
	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}
}

/*
 * Checks if a line segment with the exact given points (same objects) exists within this point list. (thread-safe)
 */
bool CPointList::DoesLineExist(CPolygonPoint * A, CPolygonPoint * B)
{
	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	CPolygonPoint * CurrPoint = m_HeadPoint, * NextPoint;

	do
	{
		NextPoint = CurrPoint->GetNextPoint();
		if(NextPoint == NULL)
			NextPoint = m_HeadPoint;
		if
		(
			(
				CurrPoint == A
				&&
				NextPoint == B
			)
			||
			(
				CurrPoint == B
				&&
				NextPoint == A
			)
		)
		{
			if (unlock)
			{
				singleLock->Unlock();
				delete singleLock;
			}
			return true;
		}
		CurrPoint = CurrPoint->GetNextPoint();
	}
	while(CurrPoint != NULL);

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

	return false;
}

/*
 * Returns the polygon area
 */
unsigned CPointList::GetArea()
{
	int area = 0;
	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	RecalculateBoundingBox();
	area = m_nArea;

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

	return area;
}

/*
 * Returns the position of the left side of the bounding box. Recalculates the box if it is outdated.
 */
int CPointList::GetBBX1()
{
	int x=0;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	RecalculateBoundingBox();
	x = m_BbX1;

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

	return x;
}

/*
 * Returns the position of the right side of the bounding box. Recalculates the box if it is outdated.
 */
int CPointList::GetBBX2()
{
	int x=0;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}
	RecalculateBoundingBox();
	x = m_BbX2;

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

	return x;
}

/*
 * Returns the position of the top side of the bounding box. Recalculates the box if it is outdated.
 */
int CPointList::GetBBY1()
{
	int y=0;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	RecalculateBoundingBox();
	y = m_BbY1;

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

	return y;
}

/*
 * Returns the position of the bottom side of the bounding box. Recalculates the box if it is outdated.
 */
int CPointList::GetBBY2()
{
	int y=0;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	RecalculateBoundingBox();
	y = m_BbY2;

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

	return y;
}

/*
 * Calculates the largest rectangle that fits inside the polygon (excluding the outline)
 * by starting from a seed point with a ony-by-one rectangle which is enlarged step by step.
 */
RECT CPointList::GetInboundRect()
{
	RECT ret;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	RecalculateInboundRect();
	ret = m_InboundRect;

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

	return ret;
}

/*
 * Returns the x value of the centroid of this polygon (Note: the centroid might be outside the polygon)
 * See: https://en.wikipedia.org/wiki/Centroid
 */
int CPointList::GetCentroidX()
{
	RecalculateBoundingBox();
	return m_CentroidX;
	
	/*int c=0;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	RecalculateBoundingBox();
	c = (m_BbX2 + m_BbX1) / 2;

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

	return c;*/
}

/*
 * Returns the y value of the centroid of this polygon (Note: the centroid might be outside the polygon)
 * See: https://en.wikipedia.org/wiki/Centroid
 */
int CPointList::GetCentroidY()
{
	RecalculateBoundingBox();
	return m_CentroidY;

	/*int c=0;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	RecalculateBoundingBox();
	c = (m_BbY2 + m_BbY1) / 2;
	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

	return c;*/
}

/*
 * Returns the starting point of the linked list
 */
CPolygonPoint * CPointList::GetHeadPoint()
{
	return m_HeadPoint;
}

/*
 * Returns a pointer to the the starting point of the linked list (a pointer itself)
 */
/*CPolygonPoint ** CPointList::GetHeadPointRef()
{
	return &m_HeadPoint;
}*/

/*
 * Returns the height of the bounding box. Recalculates the box if it is outdated.
 */
unsigned CPointList::GetHeight()
{
	int h=0;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	RecalculateBoundingBox();
	h = m_BbY2 - m_BbY1 + 1;

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

	return h;
}

/*
 * Returns the number of points within this list.
 * Deprecated: Use GetPointCount()
 */
int CPointList::GetNoPoints()
{
	return m_PointCount;
}

/*
 * Returns the number of points within this list
 * Note: Replaces GetNoPoints()
 */
int CPointList::GetPointCount()
{
	return m_PointCount;
}

/*
 * Returns the end point of the linked list
 */
CPolygonPoint * CPointList::GetTailPoint()
{
	return m_TailPoint;
}

/*CPolygonPoint ** CPointList::GetTailPointRef()
{
	return &m_TailPoint;
}*/

/*
 * Returns the width of the bounding box. Recalculates the box if it is outdated.
 */
unsigned CPointList::GetWidth()
{
	int w=0;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	RecalculateBoundingBox();
	w = m_BbX2 - m_BbX1 + 1;

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

	return w;
}

/*
 * Inserts the given new point into the list, after the specified point that already is in this list
 */
void CPointList::InsertAfter(CPolygonPoint * originalPoint, CPolygonPoint * newPoint)
{
	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	if(originalPoint == NULL)
	{
		if(m_HeadPoint == NULL)
		{
			m_HeadPoint = newPoint;
			m_TailPoint = newPoint;
		}
		else
		{
			newPoint->SetPrevPoint(NULL);
			newPoint->SetNextPoint(m_HeadPoint);
			newPoint->SetParentList(this);
			m_HeadPoint->SetPrevPoint(newPoint);
			m_HeadPoint = newPoint;
			m_PointCount++;
		}
	}
	else
	{
		if(originalPoint->GetNextPoint() == NULL)
			m_TailPoint = newPoint;
		else
			originalPoint->GetNextPoint()->SetPrevPoint(newPoint);
		newPoint->SetNextPoint(originalPoint->GetNextPoint());
		newPoint->SetPrevPoint(originalPoint);
		originalPoint->SetNextPoint(newPoint);
		m_PointCount++;
		originalPoint->SetParentList(this);
	}
	SetBBOutdated();

	newPoint->SetParentList(this);
	m_HeadPoint->SetParentList(this);
	m_TailPoint->SetParentList(this);

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}
}

/*
 * Checks if the corner points of the given rectangle are inside this polygon (not including the outline).
 */
bool CPointList::IsAreaInside(int x1, int y1, int x2, int y2)
{
	bool ret = false;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	ret = (IsPointInside(x1, y1) && IsPointInside(x2, y1) && IsPointInside(x2, y2) && IsPointInside(x1, y2));

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

	return ret;
}

/*
 * Is Point on Polygon Line?
 * CC 02.12.2009
 */
bool CPointList::IsPointOnLine(int x, int y)
{
	CPolygonPoint * P1, * P2;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	P1 = m_HeadPoint;
	bool ret = false;
	while(P1 != NULL)
	{
		P2 = P1->GetNextPoint(true);

		if (P1->GetX() == P2->GetX()) //vertical
		{
			if (x == P1->GetX() && y>=min(P1->GetY(), P2->GetY()) && y<=max(P1->GetY(), P2->GetY()))
			{
				ret = true;
				break;
			}
		}
		else if (P1->GetY() == P2->GetY()) //horizontal
		{
			if (y == P1->GetY() && x>=min(P1->GetX(), P2->GetX()) && x<=max(P1->GetX(), P2->GetX()))
			{
				ret = true;
				break;
			}
		}
		else //diagonal
		{
			double dist = 0.0;
			//CC 24.05.2010 - changed from  <= 1.0  to  <= 0.5
			if ((dist=CExtraMath::DistancePointLine(x, y, P1->GetX(), P1->GetY(), P2->GetX(), P2->GetY())) <= 0.5)
			{
				ret = true;
				break;
			}
		}
		P1 = P1->GetNextPoint();
	}

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}
	return ret;
}

/*
 * Checks if the given coordinates are inside the bounding box (including the border)
 */
bool CPointList::IsPointInsideBoundingBox(int x, int y)
{
	if (m_PointCount <= 0)
		return false;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	RecalculateBoundingBox();

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

	//Check bounding box
	if (x < m_BbX1 || x > m_BbX2 || y < m_BbY1 || y > m_BbY2)
	{
		return false;
	}
	return true;
}

/*
 * Checks if the given coordinates are inside the polygon represented by this point list
 */
bool CPointList::IsPointInside(int x, int y, bool checkContour)
{
	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	if (m_PointCount <= 0)
	{
		if (unlock)
		{
			singleLock->Unlock();
			delete singleLock;
		}
		return false;
	}

	RecalculateBoundingBox();

	//Check bounding box first
	if (x < m_BbX1 || x > m_BbX2 || y < m_BbY1 || y > m_BbY2)
	{
		if (unlock)
		{
			singleLock->Unlock();
			delete singleLock;
		}
		return false;
	}

	//Check if point on contour
	if (checkContour && IsPointOnLine(x,y))
	{
		if (unlock)
		{
			singleLock->Unlock();
			delete singleLock;
		}
		return true;
	}

	//Is point inside algorithm:
	// (See http://www.codeproject.com/KB/recipes/geometry.aspx )

	int  j, inside_flag, xflag0 ;
	double dv0 ;
	int     crossings, yflag0, yflag1 ;
	double vertex0x, vertex0y, vertex1x, vertex1y;
	CPolygonPoint * vertex0, * vertex1, * P;
 
	vertex0 = m_TailPoint;
	vertex0x = vertex0->GetX();
	vertex0y = vertex0->GetY();
 
    //Get test bit for above/below Y axis 
    yflag0 = ( dv0 = vertex0y - y ) >= 0.0;
 
    crossings = 0;
	P = m_HeadPoint;
	j = 0;
	while(P != NULL)
	{
        // cleverness:  bobble between filling endpoints of edges, so
		// that the previous edge's shared endpoint is maintained.
		if ( j & 0x1 ) 
		{
			vertex0 = P;
			vertex0x = vertex0->GetX();
			vertex0y = vertex0->GetY();
			yflag0 = ( dv0 = vertex0y - y ) >= 0.0 ;
		} 
		else 
		{
			vertex1 = P;
			vertex1x = vertex1->GetX();
			vertex1y = vertex1->GetY();
			yflag1 = ( vertex1y >= y ) ;
		}
 
		// check if points not both above/below X axis - can't hit ray 
		if (yflag0 != yflag1) 
		{
            // check if points on same side of Y axis 
            if ( ( xflag0 = ( vertex0x >= x ) ) == ( vertex1x >= x ) ) 
			{
                if ( xflag0 ) 
					crossings++;
            } 
			else 
			{
                // compute intersection of pgon segment with X ray, note
                // if > point's X.
                //
                crossings += (vertex0x - dv0 * (vertex1x-vertex0x)/(vertex1y-vertex0y)) >= x;
            }
        }
		P = P->GetNextPoint();
		j++;
    }
 
    // test if crossings is odd
    // if all we care about is winding number > 0, then just:
    //       inside_flag = crossings > 0;
 
	inside_flag = crossings & 0x01;
 
	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

    return inside_flag != 0;
}

/*
 * Calculates and chaches the enclosing rectangle for this point list 
 */
void CPointList::RecalculateBoundingBox()
{
	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	if(!m_BbUpToDate)
	{
		CPolygonPoint * Pt = m_HeadPoint;

		if(Pt != NULL)
		{
			m_BbX1 = Pt->GetX();
			m_BbX2 = Pt->GetX();
			m_BbY1 = Pt->GetY();
			m_BbY2 = Pt->GetY();
		}

		while(Pt != NULL)
		{
			if(Pt->GetX() < m_BbX1)
				m_BbX1 = Pt->GetX();
			else if(Pt->GetX() > m_BbX2)
				m_BbX2 = Pt->GetX();

			if(Pt->GetY() < m_BbY1)
				m_BbY1 = Pt->GetY();
			else if(Pt->GetY() > m_BbY2)
				m_BbY2 = Pt->GetY();
				
			Pt = Pt->GetNextPoint();
		}

		//Calculate area
		CPolygonPoint * P1 = m_HeadPoint, * P2;
		m_nArea = 0;

		while(P1 != NULL)
		{
			P2 = (P1->GetNextPoint() == NULL ? m_HeadPoint : P1->GetNextPoint());

			m_nArea += P1->GetX() * P2->GetY() - P2->GetX() * P1->GetY();
			P1 = P1->GetNextPoint();
		}
		m_nArea = m_nArea / 2;

		//Calculate centroid
		m_CentroidX = 0;
		m_CentroidY = 0;
		double centrX = 0.0, centrY = 0.0; //Had to switch to double, integer caused overflow for high x,y values
		P1 = m_HeadPoint;

		while(P1 != NULL)
		{
			P2 = (P1->GetNextPoint() == NULL ? m_HeadPoint : P1->GetNextPoint());

			centrX += (double)(P1->GetX() + P2->GetX()) * ((double)P1->GetX() * (double)P2->GetY() - (double)P2->GetX() * (double)P1->GetY());
			centrY += (double)(P1->GetY() + P2->GetY()) * ((double)P1->GetX() * (double)P2->GetY() - (double)P2->GetX() * (double)P1->GetY());
			
			P1 = P1->GetNextPoint();
		}
		centrX = 1.0 / (6.0 * m_nArea) * (double)centrX;
		centrY = 1.0 / (6.0 * m_nArea) * (double)centrY;
		m_CentroidX = (int)(centrX + 0.5);
		m_CentroidY = (int)(centrY + 0.5);

		//Area could be negative depending on if it's clockwise or not
		m_nArea = abs(m_nArea);

		m_BbUpToDate = true;
	}

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}
}

/*
 * Calculates and chaches the largest rectanlge that fits into the polygon defined by this point list 
 */
void CPointList::RecalculateInboundRect()
{
	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	if(!m_InboundRectUpToDate)
	{
		//Find start point
		POINT startPoint = FindCenterMostPointInsidePolygon();

		//Extend initial rect (1x1) in all directions
		int incrLeft = max(1,(m_BbX2 - m_BbX1) / 20);
		int incrRight = incrLeft;
		int incrTop = max(1,(m_BbY2 - m_BbY1) / 20);
		int incrBottom = incrTop;

		RECT rect;
		rect.left = startPoint.x;
		rect.right = startPoint.x;
		rect.top = startPoint.y;
		rect.bottom = startPoint.y;
		bool extendLeft = true;
		bool extendRight = true;
		bool extendTop = true;
		bool extendBottom = true;

		while (extendLeft || extendRight || extendTop || extendBottom)
		{
			//Extend left
			if (extendLeft && AreRectCornersInside(rect.left-incrLeft, rect.right, rect.top, rect.bottom))
				rect.left = rect.left-incrLeft;
			else if (incrLeft > 1)
				incrLeft /= 2; //Reduce increment
			else
				extendLeft = false;
			//Extend right
			if (extendRight && AreRectCornersInside(rect.left, rect.right+incrRight, rect.top, rect.bottom))
				rect.right = rect.right+incrRight;
			else if (incrRight > 1)
				incrRight /= 2; //Reduce increment
			else
				extendRight = false;
			//Extend top
			if (extendTop && AreRectCornersInside(rect.left, rect.right, rect.top-incrTop, rect.bottom))
				rect.top = rect.top-incrTop;
			else if (incrTop > 1)
				incrTop /= 2; //Reduce increment
			else
				extendTop = false;
			//Extend bottom
			if (extendBottom && AreRectCornersInside(rect.left, rect.right, rect.top, rect.bottom+incrBottom))
				rect.bottom = rect.bottom+incrBottom;
			else if (incrBottom > 1)
				incrBottom /= 2; //Reduce increment
			else
				extendBottom = false;
		}
		m_InboundRect = rect;

		m_InboundRectUpToDate = true;
	}

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}
}

/*
 * Checks if the corner points of the given rectangle are all inside the polygon defined by this point list
 * Update: Now also checks the middle points half-way between the corners
 */
bool CPointList::AreRectCornersInside(int left, int right, int top, int bottom)
{
	if (left < m_BbX1 || m_BbY1 < 0 || right > m_BbX2 || bottom > m_BbY2)
		return false;
	return IsPointInside(left, top) 
		&& IsPointInside(right, top) 
		&& IsPointInside(left, bottom)
		&& IsPointInside(right, bottom)
		&& IsPointInside((right+left)/2, top)		//top middle
		&& IsPointInside((right+left)/2, bottom)	//bottom middle
		&& IsPointInside(left, (bottom+top)/2)		//left middle
		&& IsPointInside(right, (bottom+top)/2);	//right middle
}

/*
 * Finds a point that is at the centre (of the bounding box) or closeby AND inside the polygon defined by this point list.
 */
POINT CPointList::FindCenterMostPointInsidePolygon()
{
	int xc = GetCenterX();
	int yc = GetCenterY();

	int x = xc;
	int y = yc;

	if (!IsPointInside(x,y))
	{
		int r = 1;
		int rxmax = (m_BbX2 - m_BbX1) / 2;
		int rymax = (m_BbY2 - m_BbY1) / 2;
		int rmax = max(rxmax, rymax);
		while (r < rmax)
		{
			if (r < rxmax)
			{
				y = yc;
				x = xc+r; 
				if (IsPointInside(x,y))
					break;
				x = xc-r; 
				if (IsPointInside(x,y))
					break;
			}
			if (r < rymax)
			{
				x = xc;
				y = yc+r; 
				if (IsPointInside(x,y))
					break;
				y = yc-r; 
				if (IsPointInside(x,y))
					break;
			}
			r++;
		}
		if (r >= rmax) //No point found -> use center
		{
			x = xc;
			y = yc;
		}
	}
	POINT ret;
	ret.x = x;
	ret.y = y;
	return ret;
}

/*
 * Invalidates the bounding box and inbound rectangles so that they will be recalculated on demand.
 */
void CPointList::SetBBOutdated()
{
	m_BbUpToDate = false;
	m_InboundRectUpToDate = false;
	m_Modified = true;
}

/*
 * Deletes all points and resizes the list to the given number of (uninitialised) points
 */
bool CPointList::SetNoPoints(int NoPoints)
{
	int i;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	DeletePoints();

	CPolygonPoint * CurrPoint = new CPolygonPoint(this);
	CurrPoint->SetPrevPoint(NULL);
	CPolygonPoint * NewPoint;
	m_HeadPoint = CurrPoint;

	for(i = 1; i < NoPoints; i++)
	{
		NewPoint = new CPolygonPoint(this);
		NewPoint->SetPrevPoint(CurrPoint);
		CurrPoint->SetNextPoint(NewPoint);
		CurrPoint = NewPoint;
	}
	CurrPoint->SetNextPoint(NULL);
	m_TailPoint = CurrPoint;
	
	m_PointCount = NoPoints;
	SetBBOutdated();
	
	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

	return true;
}

/*
 * Moves all points by the given values.
 */
void CPointList::Move(int dx, int dy)
{
	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	CPolygonPoint * p = m_HeadPoint;
	while (p != NULL)
	{
		p->SetXY(p->GetX() + dx, p->GetY() + dy);
		p = p->GetNextPoint();
	}

	//Update the bounding box (save some processing)
	if (m_BbUpToDate)
	{
		m_BbX1 += dx;
		m_BbY1 += dy;
		m_BbX2 += dx;
		m_BbY2 += dy;
		m_CentroidX += dx;
		m_CentroidY += dy;
	}

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}
}

/*
 * Removes points that do not influence the shape of the polygon (i.e. that are on a straight line)
 */
void CPointList::SimplifyPolygon()
{
	bool Changed;
	CPolygonPoint * P1, * P2, * P3;

	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	do
	{
		Changed = false;

		P1 = m_HeadPoint;

		while(P1 != NULL)
		{
			P2 = P1->GetNextPoint();
			if(P2 == NULL)	
				P2 = m_HeadPoint;

			if(P1->GetX() == P2->GetX() && P1->GetY() == P2->GetY())
			{
				DeletePoint(P2);
				Changed = true;
				break;
			}

			P3 = P2->GetNextPoint();
			if(P3 == NULL)
				P3 = m_HeadPoint;

			if
			(
				(
					P1->GetX() == P2->GetX()
					&&
					P2->GetX() == P3->GetX()
					&&
					P2->GetY() > CExtraMath::Min(P1->GetY(), P3->GetY())
					&&
					P2->GetY() < CExtraMath::Max(P1->GetY(), P3->GetY())
				)
				||
				(
					P1->GetY() == P2->GetY()
					&&
					P2->GetY() == P3->GetY()
					&&
					P2->GetX() > CExtraMath::Min(P1->GetX(), P3->GetX())
					&&
					P2->GetX() < CExtraMath::Max(P1->GetX(), P3->GetX())
				)
			)
			{
				DeletePoint(P2);
				Changed = true;
				break;
			}

			P1 = P1->GetNextPoint();
		}
	}
	while(Changed);

	if (Changed)
		SetBBOutdated();

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}
}

/*
 * Sets the starting point of this list.
 * Note: Essentially changes the list. Make sure all unused points are deleted.
 */
void CPointList::SetHeadPoint(CPolygonPoint * headPoint)
{
	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	m_HeadPoint = headPoint;

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}
}

/*
 * Checks if the bounding box of this polygon overlaps the bounding box of the given other polygon
 */
bool CPointList::DoBoundingBoxesOverlap(CPointList * otherList)
{
	int x1 = otherList->GetBBX1();
	int x2 = otherList->GetBBX2();
	int y1 = otherList->GetBBY1();
	int y2 = otherList->GetBBY2();

	return DoBoundingBoxesOverlap(x1, y1, x2, y2);
}

/*
 * Checks if the bounding box of this polygon overlaps the given rectangle
 */
bool CPointList::DoBoundingBoxesOverlap(int x1, int y1, int x2, int y2)
{
	RecalculateBoundingBox();
	if (	m_BbX1 >= x1 && m_BbX1 <= x2 && m_BbY1 >= y1 && m_BbY1 <= y2
		 || m_BbX2 >= x1 && m_BbX2 <= x2 && m_BbY1 >= y1 && m_BbY1 <= y2
		 || m_BbX1 >= x1 && m_BbX1 <= x2 && m_BbY2 >= y1 && m_BbY2 <= y2
		 || m_BbX2 >= x1 && m_BbX2 <= x2 && m_BbY2 >= y1 && m_BbY2 <= y2)
		 return true;
	if (	x1 >= m_BbX1 && x1 <= m_BbX2 && y1 >= m_BbY1 && y1 <= m_BbY2
		 || x2 >= m_BbX1 && x2 <= m_BbX2 && y1 >= m_BbY1 && y1 <= m_BbY2
		 || x1 >= m_BbX1 && x1 <= m_BbX2 && y2 >= m_BbY1 && y2 <= m_BbY2
		 || x2 >= m_BbX1 && x2 <= m_BbX2 && y2 >= m_BbY1 && y2 <= m_BbY2)
		 return true;
	return false;
}

/*
 * Calculates the area of the bounding box of this polygon overlapping the bounding box of the given other polygon
 */
int CPointList::CalculateBoundingBoxOverlapArea(CPointList * otherList)
{
	int x1 = otherList->GetBBX1();
	int x2 = otherList->GetBBX2();
	int y1 = otherList->GetBBY1();
	int y2 = otherList->GetBBY2();

	return CalculateBoundingBoxOverlapArea(x1, y1, x2, y2);
}

/*
 * Calculates the area of the bounding box of this polygon overlapping the given rectangle
 */
int CPointList::CalculateBoundingBoxOverlapArea(int x1, int y1, int x2, int y2)
{
	//First, check if any overlap at all
	if (!DoBoundingBoxesOverlap(x1,y1,x2,y2))
		return 0;

	RecalculateBoundingBox();

	int intersection = max(0, min(x2, m_BbX2) - max(x1, m_BbX1) + 1) * max(0, min(y2, m_BbY2) - max(y1, m_BbY1) + 1);

	return intersection;
}

/*
 * Sets tha end point of this list
 */
void CPointList::SetTailPoint(CPolygonPoint * tailPoint)
{
	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	m_TailPoint = tailPoint;

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}
}

/*
 * Make this point list thread-safe (at least all methods that use the critical section).
 */
void CPointList::SetSynchronized(bool sync) 
{ 
	if (sync == m_Synchronize) //no change
		return;

	//Have to lock here too, because another thread could be within a critical section
	CSingleLock * singleLock = NULL;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
	}
	m_Synchronize = sync; 
	if (m_CriticalSect != NULL)
	{
		singleLock->Unlock();
		delete singleLock;
	}

	//Wait again
	if (m_CriticalSect != NULL)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		singleLock->Unlock();
		delete singleLock;
	}
	//Now it should be safe to delete m_CriticalSect 
	if (sync)
	{
		m_CriticalSect = new CCriticalSection();
		delete m_OldCriticalSect;
		m_OldCriticalSect = m_CriticalSect;
	}
	else
	{	
		m_CriticalSect = NULL;
	}
};

/*
 * Returns the center of the bounding box
 */
int CPointList::GetCenterX()
{
	const int bbx1 = GetBBX1();
	const int bbx2 = GetBBX2();
	return bbx1 + (bbx2-bbx1) / 2;
}

/*
 * Returns the center of the bounding box
 */
int CPointList::GetCenterY()
{
	const int bby1 = GetBBY1();
	const int bby2 = GetBBY2();
	return bby1 + (bby2-bby1) / 2;
}

/*
 * Calculates the centre of mass for this polygon.
 * Note: The GetCentroid() function returns the center of the bounding box, rather than the centre of mass!
 * See: http://local.wasp.uwa.edu.au/~pbourke/geometry/polyarea/
 */
CPolygonPoint CPointList::GetCenterOfMass()
{
	CSingleLock * singleLock = NULL;
	bool unlock = false;
	if (m_Synchronize)
	{
		singleLock = new CSingleLock(m_CriticalSect);
		singleLock->Lock();
		unlock = true;
	}

	CPolygonPoint * p = GetHeadPoint();
	CPolygonPoint * q;
	double cx = 0, cy = 0;
	while (p != NULL)
	{
		q = p->GetNextPoint(true);
		cx += (p->GetX() + q->GetX()) * (p->GetX()*q->GetY() - q->GetX()*p->GetY());
		cy += (p->GetY() + q->GetY()) * (p->GetX()*q->GetY() - q->GetX()*p->GetY());
		p = p->GetNextPoint();
	}
	if (GetArea() != 0)
	{
		cx /= (6.0 * GetArea());
		cy /= (6.0 * GetArea());
	}
	CPolygonPoint res((int)cx,(int)cy);

	if (unlock)
	{
		singleLock->Unlock();
		delete singleLock;
	}

	return res;
}

/*
 * Creates a clone (deep copy) of this region point list.
 */
CPointList * CPointList::Clone()
{
	CPointList * copy = new CPointList();

	CPolygonPoint * pOrig = this->GetHeadPoint();
	CPolygonPoint * pCopy;
	while (pOrig != NULL)
	{
		pCopy = copy->AddPoint(pOrig->GetX(), pOrig->GetY());
		pOrig = pOrig->GetNextPoint();
	}
	return copy;
}

/*
 * Checks if the polygon represented by this point list is rectangular.
 */
bool CPointList::IsRectangle()
{
	if (m_PointCount != 4)
		return false;
	return IsIsothetic(); //If the polgon has four points and is isothetic, then it must be a rectangle.
}

/*
 * Checks if the polygon represented by this point list is isothetic (only horizontal and vertical lines).
 */
bool CPointList::IsIsothetic()
{
	CPolygonPoint * q = m_HeadPoint;
	CPolygonPoint * p;
	for (int i=0; i<m_PointCount; i++)
	{
		p = q;
		if (p != NULL)
		{
			q = q->GetNextPoint(true);
			if (q != NULL)
			{
				//One coordinate of two adjacent points has to be the same for both points.
				//If both x and y are different, it is not an isothetic polygon.
				if (p->GetX() != q->GetX() && p->GetY() != q->GetY()) 
					return false;
			}
		}
	}
	return true;
}


} //end namespace