#ifndef EXTRAMATH_H
#define EXTRAMATH_H

//#include "RegionPoint.h"
#include "stdafx.h"
#include <list>

using namespace std;

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace PRImA 
{

struct LineSegment;

enum EdgeType
{
	COLOCATED,
	VERTICAL,
	HORIZONTAL,
	DIAGONALV,
	DIAGONALH
};

/*
 * Struct Point
 *
 * Simple 2D point with x and y
 */
struct Point
{
	int x;
	int y;
};


/*
 * Class CExtraMath
 *
 * Helper class with static functions for mathematical problems.
 */
class DllExport CExtraMath
{
	// DATA TYPES
public:
	struct Line
	{
		double Slope;
		double Intercept;
	};

	// METHODS
public:
	static double AngleTwoPoints(int X1, int Y1, int X2, int Y2);
	static double NewAngleTwoPoints(int X1, int Y1, int X2, int Y2);  // The angle is horizontal  
	static double AngleLineXAxis(int x1, int y1, int x2, int y2);
	static int    CompareLineSegments(const LineSegment * A, const LineSegment * B);

	static double DegreesToRadians(double Degrees);
	static double DistancePointLine(const int PX, const int PY, const int StartX, const int StartY, 
									const int EndX, const int EndY);
	static double DistancePointLine(const int PX, const int PY, const int StartX, const int StartY, 
									const int EndX, const int EndY, int * pointOnLineX, int * pointOnLineY);

	static double DistancePointInfiniteLine(const int px, const int py, const int lineX1, const int lineY1, const int lineX2, const int lineY2);

	static double DistanceTwoPoints(int X1, int Y1, int X2, int Y2);
	static double DistanceTwoPoints(double X1, double Y1, double X2, double Y2);

	static bool   DoLineSegmentsIntersect(	double AX1, double AY1, double AX2, double AY2, 
											double BX1, double BY1, double BX2, double BY2,
											double tolerance = 0.0);

	static bool GetInterceptionPointOfTwoLines(	double ax1, double ay1, double ax2, double ay2, 
												double bx1, double by1, double bx2, double by2,
												double & intersectX, double & intersectY);

	static double FMin(double A, double B);
	static EdgeType GetEdgeType(int X1, int Y1, int X2, int Y2);
	//static int    IsGreater(const int A, const int B);
	//static int    IsGreaterP(const int * A, const int * B);
	//static int    IsSmaller(const int A, const int B);
	//static int    IsSmallerP(const int * A, const int * B);
	//static bool   IsIn(int SearchFor,int * InArray, int NoArrayMembers);
	static int      Min(int A, int B);
	static long     Min(long A, long B);
	static unsigned Min(unsigned A, unsigned B);
	static int    Min(int A, int B, int C);
	static int    Max(int A, int B);
	static double Max(double A, double B);
	static int    Max(int A, int B, int C);
	static double RadiansToDegrees(double Radians);

	static list<Point> * GetPointsOfLine(int x1, int y1, int x2, int y2);
};



typedef Point Vertex;

struct Edge
{
	int StartV;
	int EndV;
};

struct LineSegment
{
	int x1;
	int y1;
	int x2;
	int y2;
};

//CC 24/02/2016 - Commented out, seems not to be used anywhere
/*struct Fraction
{
	inline double Amount()
	{
		if(Denominator == 0.0)
		{
			if(Numerator == 0.0)
				return 1.0;
			else
				return 9999.9;
		}
		else
			return double(Numerator) / double(Denominator);
	};

	inline void Initialise()
	{
		Numerator = 0;
		Denominator = 0;
	};

	double Numerator;
	double Denominator;
};
*/

typedef LineSegment Rect;


}

#endif // EXTRAMATH_H
