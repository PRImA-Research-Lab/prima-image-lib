#pragma once

#include "ExtraMath.h"
#include "RegionPoint.h"
#include <afxmt.h>
#include <vector>
#include <map>

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
 * Class CPointList
 *
 * A linked list of 2D points, usually used for polygons
 *
 */
class DllExport CPointList
{
public:
	CPointList(void);
	virtual ~CPointList(void);

	CPolygonPoint *  AddPoint();
	CPolygonPoint *  AddPoint(int X, int Y);
	void            CopyPoints(CPointList & r);
	void            CopyPoints(CPointList * r);
	void            DeletePoint(CPolygonPoint * DeletePoint);
	void            DeletePoint(int Index);
	void            DeletePoints();
	
	bool			DoBoundingBoxesOverlap(CPointList * otherList);
	bool			DoBoundingBoxesOverlap(int x1, int y1, int x2, int y2);
	int				CalculateBoundingBoxOverlapArea(CPointList * otherList);
	int				CalculateBoundingBoxOverlapArea(int x1, int y1, int x2, int y2);

	bool            DoesLineExist(CPolygonPoint * A, CPolygonPoint * B);
	unsigned        GetArea();
	int             GetBBX1();
	int             GetBBX2();
	int             GetBBY1();
	int             GetBBY2();

	RECT			GetInboundRect();
	int             GetCentroidX();
	int             GetCentroidY();
	int             GetCenterX();
	int             GetCenterY();
	CPolygonPoint	GetCenterOfMass();
	CPolygonPoint *  GetHeadPoint();
	//CPolygonPoint ** GetHeadPointRef();
	unsigned        GetHeight();
	int             GetNoPoints();
	int             GetPointCount();
	CPolygonPoint *  GetTailPoint();
	//CPolygonPoint ** GetTailPointRef();
	unsigned        GetWidth();
	void            InsertAfter(CPolygonPoint * OriginalPoint, CPolygonPoint * NewPoint);
	bool            IsAreaInside(int x1, int y1, int x2, int y2);
	bool            IsPointInside(int x, int y, bool checkContour = false);
	bool            IsPointInsideBoundingBox(int x, int y);
	bool            IsPointOnLine(int x, int y);
	void            SetBBOutdated();
	void			SetHeadPoint(CPolygonPoint * headPoint);
	void			SetTailPoint(CPolygonPoint * tailPoint);
	bool            SetNoPoints(int NoPoints);
	void			SetSynchronized(bool sync);
	inline bool		IsSynchronized() { return m_Synchronize; };
	inline CCriticalSection * GetCriticalSection() { return m_CriticalSect; }; //For outside synchronization
	void            SimplifyPolygon();
	virtual CPointList * Clone();
	inline void		ResetModified() { m_Modified = false; };
	inline bool		IsModified() { return m_Modified; };
	inline void		IncPointCount() { m_PointCount++; }; //Increments the point count

	void			Move(int dx, int dy);

	//void			AppendPointList(CPointList * r);
	void			Clear();

	bool			IsRectangle();
	bool			IsIsothetic();

protected:
	void            RecalculateBoundingBox();
	void			RecalculateInboundRect();
	POINT			FindCenterMostPointInsidePolygon();
	bool			AreRectCornersInside(int left, int right, int top, int bottom);

	// DATA ITEMS
protected:
	bool			m_BbUpToDate;
	bool			m_InboundRectUpToDate;
	bool			m_Modified;
	int m_nArea, m_BbX1, m_BbX2, m_BbY1, m_BbY2;
	int				m_CentroidX;
	int				m_CentroidY;
	RECT			m_InboundRect;
	CPolygonPoint * m_HeadPoint;
	int            m_PointCount;
	CPolygonPoint * m_TailPoint;

	bool				m_Synchronize;
	CCriticalSection *	m_CriticalSect;
	CCriticalSection *	m_OldCriticalSect;

};

}