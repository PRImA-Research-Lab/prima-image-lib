#include "RegionPoint.h"

namespace PRImA
{

/*
 * Class CPolygonPoint
 *
 * Point that is part of a point list (i.e. a polygon)
 * 
 * CC 23/02/2016 - Renamed from CRegionPoint
 */

/*
 * Constructor for empty point (0,0) without parent list
 */
CPolygonPoint::CPolygonPoint()
{
	m_X = 0;
	m_Y = 0;
	m_NextPoint = NULL;
	m_PrevPoint = NULL;
	m_ParentList = NULL;
}

/*
 * Constructor for empty point (0,0) with parent list
 */
CPolygonPoint::CPolygonPoint(CPointList * parentList)
{
	m_X = 0;
	m_Y = 0;
	m_NextPoint = NULL;
	m_PrevPoint = NULL;
	m_ParentList = parentList;
}

/*
 * Constructor for specified coordinates (x,y) without parent list
 */
CPolygonPoint::CPolygonPoint(int x, int y)
{
	m_NextPoint = NULL;
	m_PrevPoint = NULL;
	SetXY(x, y);
	m_ParentList = NULL;
}

/*
 * Constructor for specified coordinates (x,y) with parent list
 */
CPolygonPoint::CPolygonPoint(int x, int y, CPointList * parentList)
{
	m_NextPoint = NULL;
	m_PrevPoint = NULL;
	SetXY(x, y);
	m_ParentList = parentList;
}

/*
 * Destructor
 */
CPolygonPoint::~CPolygonPoint()
{
}

/*
 * Returns the next point in the linked list.
 * If no next point is defined (NULL), the head point of the parent list is returned (if there is a parent list)
 */
CPolygonPoint * CPolygonPoint::GetNextPoint(bool includeHead)
{
	if (includeHead && m_ParentList != NULL && m_NextPoint == NULL) //tail point
		return m_ParentList->GetHeadPoint();
	return m_NextPoint;
}

/*
 * Returns the previous point in the linked list.
 * If no previous point is defined (NULL), the tail point of the parent list is returned (if there is a parent list)
 */
CPolygonPoint * CPolygonPoint::GetPrevPoint(bool includeTail)
{
	if (includeTail && m_ParentList != NULL && m_PrevPoint == NULL) //head point
		return m_ParentList->GetTailPoint();
	return m_PrevPoint;
}

/*
 * The x position of this point
 */
int CPolygonPoint::GetX()
{
	return m_X;
}

/*
 * The x position of this point
 */
int CPolygonPoint::GetX() const
{
	return m_X;
}

/*
 * The y position of this point
 */
int CPolygonPoint::GetY()
{
	return m_Y;
}

/*
 * The y position of this point
 */
int CPolygonPoint::GetY() const
{
	return m_Y;
}

/*
 * Sets the next point in the linked list
 * 'updateHead' - Update the head point of the parent list (makes this point the new head point if the newNextPoint was the head point before)
 */
void CPolygonPoint::SetNextPoint(CPolygonPoint * newNextPoint, bool updateHead)
{
	if (updateHead && m_ParentList != NULL)
	{
		if (newNextPoint == m_ParentList->GetHeadPoint())
		{
			if (this != m_ParentList->GetTailPoint())
			{
				m_ParentList->SetHeadPoint(this);
				m_PrevPoint = NULL;
			}
			else
				return; //don't connect head and tail
		}
	}
	m_NextPoint = newNextPoint;
	if (m_NextPoint != NULL && this->m_ParentList != NULL)
		m_NextPoint->m_ParentList = this->m_ParentList;
}

/*
 * Sets the previous point in the linked list
 * 'updateTail' - Update the tail point of the parent list (makes this point the new tail point if the newPrevPoint was the tail point before)
 */
void CPolygonPoint::SetPrevPoint(CPolygonPoint * NewPrevPoint, bool updateTail)
{
	if (updateTail && m_ParentList != NULL)
	{
		if (NewPrevPoint == m_ParentList->GetTailPoint())
		{
			if (this != m_ParentList->GetHeadPoint())
			{
				m_ParentList->SetTailPoint(this);
				m_NextPoint = NULL;
			}
			else
				return; //don't connect head and tail
		}
	}
	m_PrevPoint = NewPrevPoint;
	if (m_PrevPoint != NULL && this->m_ParentList != NULL)
		m_PrevPoint->m_ParentList = this->m_ParentList;
}

/*
 * The x position of this point
 */
void CPolygonPoint::SetX(int NewX)
{
	m_X = NewX;
}

/*
 * The position of this point
 */
void CPolygonPoint::SetXY(int NewX, int NewY)
{
	m_X = NewX;
	m_Y = NewY;
}

/*
 * The y position of this point
 */
void CPolygonPoint::SetY(int NewY)
{
	m_Y = NewY;
}

/*
 * Assignment operator
 */
CPolygonPoint & CPolygonPoint::operator=(const CPolygonPoint & Old)
{
	m_X = Old.GetX();
	m_Y = Old.GetY();

	return *this;
}

/*
 * Checks if the given point is at the same position as this point (x and y)
 */
bool CPolygonPoint::Equals(CPolygonPoint * point2)
{
	if (point2 == NULL)
		return false;
	return (this->m_X == point2->GetX() && this->m_Y == point2->GetY());
}


} //end namespace