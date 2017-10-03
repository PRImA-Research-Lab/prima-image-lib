#ifndef REGIONPOINT_H
#define REGIONPOINT_H

#ifndef _MSC_VER
#include <stdint.h>
#else
//#include <pstdint.h>
#endif
#include "PointList.h"

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif


namespace PRImA 
{

class CPointList;

/*
 * Class CPolygonPoint
 *
 * Point that is part of a point list (i.e. a polygon)
 * 
 * CC 23/02/2016 - Renamed from CRegionPoint
 */

class DllExport CPolygonPoint
{
	// CONSTRUCTION
public:
	CPolygonPoint();
	CPolygonPoint(CPointList * parentList);
	CPolygonPoint(int x, int y);
	CPolygonPoint(int x, int y, CPointList * parentList);
	~CPolygonPoint();

	// METHODS
public:
	CPolygonPoint * GetNextPoint(bool includeHead = false);  //If includeHead==true, the head point is returned for call tailPoint.GetNextPoint()
	CPolygonPoint * GetPrevPoint(bool includeTail = false);  //If includeTail==true, the tail point is returned for call headPoint.GetNextPoint()
	int GetX();
	int GetX() const;
	int GetY();
	int GetY() const;
	void SetNextPoint(CPolygonPoint * NewNextPoint, bool updateHead = false); //If updateHead==true and the next point is the head of the list, then this point becomes the new head (only if not tail)
	void SetPrevPoint(CPolygonPoint * NewPrevPoint, bool updateTail = false); //If updateTail==true and the prev point is the tail of the list, then this point becomes the new tail (only if not head)
	void SetX(int NewX);
	void SetXY(int NewX, int NewY);
	void SetY(int NewY);
	CPolygonPoint & operator=(const CPolygonPoint & Old);
	bool Equals(CPolygonPoint * point2);
	inline void SetParentList(CPointList* parentList) { m_ParentList = parentList; };
	inline CPointList* GetParentList() { return m_ParentList; };

	// DATA
private:
	int m_X;
	int m_Y;
	CPolygonPoint * m_NextPoint;
	CPolygonPoint * m_PrevPoint;
	CPointList * m_ParentList;
};

}

#else // REGIONPOINT_H

namespace PRImA 
{
class CPolygonPoint;
}
#endif // REGIONPOINT_H
