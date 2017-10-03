#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

#include "ExtraMath.h"

namespace PRImA
{

/*
 * Class CExtraMath
 *
 * Helper class with static functions for mathematical problems.
 */

/*
 * Calculates the angle between two points
 */
double CExtraMath::AngleTwoPoints(int X1, int Y1, int X2, int Y2)
{
	int Xd = X2 - X1;
	int Yd = Y2 - Y1;

	if(Xd == 0)
	{
		if(Yd <= 0)
			return 0.0;
		else //if(Yd > 0)
			return 180.0;
		//else
		//{
		//	fprintf(stderr,"AngleTwoPoints(%d,%d , %d,%d): ERROR!\n",X1,Y1,X2,Y2);
		//	return -1;
		//}
	}
	else if(Xd < 0)
	{
		if(Yd == 0)
			return 270.0;
		else if(Yd < 0)
		{
			if(abs(Xd) > abs(Yd))
				return 270.0 + RadiansToDegrees(acos(double(abs(Xd)) / double(sqrt(pow((double) Xd,2)+pow((double) Yd,2)))));
			else
				return 360.0 - RadiansToDegrees(acos(double(abs(Yd)) / double(sqrt(pow((double) Xd,2)+pow((double) Yd,2)))));
		}
		else //if(Yd > 0)
		{
			if(abs(Xd) > Yd)
				return 270.0 - RadiansToDegrees(acos(double(abs(Xd)) / double(sqrt(pow((double) Xd,2)+pow((double) Yd,2)))));
			else
				return 180.0 + RadiansToDegrees(acos(double(Yd) / double(sqrt(pow((double) Xd,2)+pow((double) Yd,2)))));
		}
		//else
		//{
		//	fprintf(stderr,"AngleTwoPoints(%d,%d , %d,%d): ERROR!\n",X1,Y1,X2,Y2);
		//	return -1;
		//}
	}
	else //if(Xd > 0)
	{
		if(Yd == 0)
			return 90.0;
		else if(Yd < 0)
		{
			if(abs(Yd) >= Xd)
			{
				return 0.0 + RadiansToDegrees(acos(double(abs(Yd)) / double(sqrt(pow((double) Xd,2)+pow((double) Yd,2)))));
			}
			else
			{
				return 90.0 - RadiansToDegrees(acos(double(Xd) / double(sqrt(pow((double) Xd,2)+pow((double) Yd,2)))));
			}
		}
		else //if(Yd > 0)
		{
			if(Yd >= Xd)
			{
				return 90.0 + RadiansToDegrees(acos(double(Xd) / double(sqrt(pow((double) Xd,2)+pow((double) Yd,2)))));
			}
			else
			{
				return 180.0 - RadiansToDegrees(acos(double(Yd) / double(sqrt(pow((double) Xd,2)+pow((double) Yd,2)))));
			}
		}
		//else
		//{
		//	fprintf(stderr,"AngleTwoPoints(%d,%d , %d,%d): ERROR!\n",X1,Y1,X2,Y2);
		//	return -1;
		//}
	}
	//else
	//{
	//	fprintf(stderr,"AngleTwoPoints(%d,%d , %d,%d): ERROR!\n",X1,Y1,X2,Y2);
	//	return -1;
	//}
}

/* 
 * Calculates angle in radians between two points and x-axis.
 */
double CExtraMath::AngleLineXAxis(int x1, int y1, int x2, int y2)
{
	return atan2(y1-y2, x2- x1);
}

/*
 * Calculates the angle between two points
 */
double CExtraMath::NewAngleTwoPoints(int X1, int Y1, int X2, int Y2)  // The angle is horizontal , angle is between 0 --- 360 
{
	int Xd = X2 - X1;
	int Yd = Y2 - Y1;

	if(Xd == 0)
	{
		if(Yd <= 0)
			return 90.0;
		else if(Yd > 0)
			return 270.0;
		else
		{
			fprintf(stderr,"AngleTwoPoints(%d,%d , %d,%d): ERROR!\n",X1,Y1,X2,Y2);
			return -1;
		}
	}
	else if(Xd < 0)  // the angle is between 90 -- 270 
	{
		if(Yd == 0)
			return 180.0;
		else if(Yd < 0)   // angle is between 90 - 180
		{
			if(abs(Xd) > abs(Yd))
				return 90.0 + RadiansToDegrees(asin(double(abs(Xd)) / double(sqrt(pow((double) Xd,2)+pow((double) Yd,2)))));
			else
				return 180.0 - RadiansToDegrees(asin(double(abs(Yd)) / double(sqrt(pow((double) Xd,2)+pow((double) Yd,2)))));
		}
		else if(Yd > 0)  // angle is between 180 -- 270 
		{
			if(abs(Xd) > Yd)
				return 180.0 + RadiansToDegrees(acos(double(abs(Xd)) / double(sqrt(pow((double) Xd,2)+pow((double) Yd,2)))));
			else
				return 270.0 - RadiansToDegrees(acos(double(Yd) / double(sqrt(pow((double) Xd,2)+pow((double) Yd,2)))));
		}
		else
		{
			fprintf(stderr,"AngleTwoPoints(%d,%d , %d,%d): ERROR!\n",X1,Y1,X2,Y2);
			return -1;
		}
	}
	else if(Xd > 0)  // the angle is 0 -- 90 && 270 -- 360
	{
		if(Yd == 0)
			return 0.0; 
		else if(Yd < 0)   // the angle is 0 -- 90 
		{
			if(abs(Yd) >= Xd)
			{
				return 90.0 - RadiansToDegrees(acos(double(abs(Yd)) / double(sqrt(pow((double) Xd,2)+pow((double) Yd,2)))));
			}
			else
			{
				return 0.0 + RadiansToDegrees(acos(double(Xd) / double(sqrt(pow((double) Xd,2)+pow((double) Yd,2)))));
			}
		}
		else if(Yd > 0)  // the angle is 270 -- 360 
		{
			if(Yd >= Xd)
			{
				return 270.0 + RadiansToDegrees(asin(double(Xd) / double(sqrt(pow((double) Xd,2)+pow((double) Yd,2)))));
			}
			else
			{
				return 360.0 - RadiansToDegrees(asin(double(Yd) / double(sqrt(pow((double) Xd,2)+pow((double) Yd,2)))));
			}
		}
		else
		{
			fprintf(stderr,"AngleTwoPoints(%d,%d , %d,%d): ERROR!\n",X1,Y1,X2,Y2);
			return -1;
		}
	}
	else
	{
		fprintf(stderr,"AngleTwoPoints(%d,%d , %d,%d): ERROR!\n",X1,Y1,X2,Y2);
		return -1;
	}
}

/*
 * Computes the distance of a point to a line segment.
 * 'PX', 'PY' - Coordinates of the point
 * 'StartX', 'StartY', 'EndX', 'EndY' - Specifies the line
 */
double CExtraMath::DistancePointLine(const int PX, const int PY, const int StartX, const int StartY, 
									 const int EndX, const int EndY)
{
	return DistancePointLine(PX, PY, StartX, StartY, EndX, EndY, NULL, NULL);
}

/*
 * Computes the distance of a point to a line segment and also passes the nearest point on the line.
 * 'PX', 'PY' (in) - Coordinates of the point
 * 'StartX', 'StartY', 'EndX', 'EndY' (in) - Specifies the line
 * 'pointOnLineX', 'pointOnLineY' (out) - Will be filled with the coordinates of the nearest point on the line
 */
double CExtraMath::DistancePointLine(const int PX, const int PY, const int StartX, const int StartY, 
									 const int EndX, const int EndY, 
									 int * pointOnLineX, int * pointOnLineY)
{
	double LineMag;
	double U;
 
	int XDiff = EndX - StartX;
	int YDiff = EndY - StartY;

	LineMag = sqrt((double) (XDiff * XDiff + YDiff * YDiff));
 
	U = (((PX - StartX) * XDiff) +
	     ((PY - StartY) * YDiff)) /
             (LineMag * LineMag);
 
	if(U < 0.0)
		return DistanceTwoPoints(PX, PY, StartX, StartY);
	else if(U > 1.0)
		return DistanceTwoPoints(PX, PY, EndX, EndY);
 
	int IntX = (int) ((StartX + U * XDiff)+0.5); //CC 24.05.2010 - added +0.5 to round
	int IntY = (int) ((StartY + U * YDiff)+0.5);

	//Return also the intersection point
	if (pointOnLineX != NULL)
	{
		*pointOnLineX = IntX;
		*pointOnLineY = IntY;
	}
 
	XDiff = PX - IntX;
	YDiff = PY - IntY;

	return sqrt((double) (XDiff * XDiff + YDiff * YDiff));
}

/*
 * Computes the distance of a point to a line (infinite, not a line segment).
 * 'px', 'px' (in) - Coordinates of the point
 * 'lineX1', 'lineY1', 'lineX2', 'lineY2' (in) - Specifies the line
*/
double CExtraMath::DistancePointInfiniteLine(const int px, const int py, const int lineX1, const int lineY1, const int lineX2, const int lineY2)
{
	// find the slope
	/*double slope_of_line = (double)(lineY1 - lineY2) / (double)(lineX1 - lineX2);
	    
	// find the perpendicular slope
	double perpendicular_slope = (double)(lineX1 - lineX2) / (double)(lineY1 - lineY2) * -1;
	    
	// find the y_intercept of line BC
	double y_intercept = slope_of_line * lineX2 - lineY2;
	    
	// find the y_intercept of line AX
	double new_line_y_intercept = perpendicular_slope * -1 * px - py;
	    
	// get the x_coordinate of point X
	// equation of BC is    y = slope_of_line * x + y_intercept;
	// equation of AX is    y = perpendicular_slope * x + new_line_y_intercept;
	//   perpendicular_slope * x + new_line_y_intercept == slope_of_line * x + y_intercept;
	//   perpendicular_slope * x == slope_of_line * x + y_intercept - new_line_y_intercept;
	//   (perpendicular_slope - slope_of_line) * x == (y_intercept - new_line_y_intercept);
	intersectX = (y_intercept - new_line_y_intercept) / (perpendicular_slope - slope_of_line);
	    
	// get the y_coordinate of point X
	intersectY = slope_of_line * intersectX + y_intercept;
	    
	// measure the distance between A and X
	return DistanceTwoPoints((double)px, (double)py, intersectX, intersectY);*/

	double A = px - lineX1;
	double B = py - lineY1;
	double C = lineX2 - lineX1;
	double D = lineY2 - lineY1;

	return abs(A * D - C * B) / sqrt(C * C + D * D);
}

/*
 * Converts degrees to radians
 */
double CExtraMath::DegreesToRadians(double Degrees)
{
	return M_PI * Degrees / 180.0;
}

/*
 * Calculates the (Euclidian) distance between two points
 */
double CExtraMath::DistanceTwoPoints(int X1, int Y1, int X2, int Y2)
{
	return sqrt(pow(double(X2 - X1),2) + pow(double(Y2 - Y1),2));
}

/*
 * Calculates the (Euclidian) distance between two points
 */
double CExtraMath::DistanceTwoPoints(double X1, double Y1, double X2, double Y2)
{
	return sqrt(pow(X2 - X1,2) + pow(Y2 - Y1,2));
}

/*
 * adapted from Darel Rex Finley, 2006: http://alienryderflex.com/intersect/
 *
 * Determines the intersection point of the line segment defined by points A1 and A2
 * with the line segment defined by points B1 and B2.
 *
 * CC 01.12.2009 replaced Daves code
 */
bool CExtraMath::DoLineSegmentsIntersect(double ax1, double ay1, double ax2, double ay2, 
										 double bx1, double by1, double bx2, double by2,
										 double tolerance)
{
    double distAB;
    double theCos;
    double theSin;
    double newX;
    double posAB;

    //  Fail if either line segment is zero-length.
    if (ax1==ax2 && ay1==ay2 || bx1==bx2 && by1==by2) 
		return false;  //NULL;

    if (ax1 == bx1 && ay1 == by1 || ax2 == bx1 && ay2 == by1) 
	{
        return true;  //new Point(bx1, by1);
    }            
    if (ax1==bx2 && ay1==by2 || ax2==bx2 && ay2==by2) 
	{
        return true;  //new Point(bx2, by2);
    }

    //  (1) Translate the system so that point A1 is on the origin.
    ax2-=ax1; ay2-=ay1;
    bx1-=ax1; by1-=ay1;
    bx2-=ax1; by2-=ay1;

    //  Discover the length of segment A1-A2.
    distAB=sqrt((double)(ax2*ax2+ay2*ay2));

    //  (2) Rotate the system so that point A2 is on the positive X ax1is.
    theCos = ax2 / distAB;            
    theSin = ay2 / distAB;            
    newX = bx1 * theCos + by1 * theSin;            
    by1  = by1 * theCos - bx1 * theSin; bx1 = newX;
    newX = bx2 * theCos + by2 * theSin;
    by2  = by2 * theCos - bx2 * theSin; bx2 = newX;
    
    //  Fail if segment B1-B2 doesn't cross line A1-A2.
    if (by1<0.0 && by2<0.0 || by1>=0.0 && by2>=0.0) 
		return false; //null;

    //  (3) Discover the position of the intersection point along line A1-A2.
    posAB = bx2+(bx1-bx2)*by2/(by2-by1);

    //  Fail if segment B1-B2 crosses line A1-A2 outside of segment A1-A2.
    if (posAB < 0.0-tolerance || posAB > distAB + tolerance) 
		return false;  //null;

    //  (4) Apply the discovered position to line A1-A2 in the original coordinate system.
    return true; //new Point(ax1 + posAB * theCos, ay1 + posAB * theSin);
}


EdgeType CExtraMath::GetEdgeType(int X1, int Y1, int X2, int Y2)
{
	if(X1 == X2)
	{
		if(Y1 == Y2)
			return COLOCATED;
		else
			return VERTICAL;
	}
	else
	{
		if(Y1 == Y2)
			return HORIZONTAL;
		else
		{
			if(abs(X2-X1) > abs(Y2-Y1))
				return DIAGONALH;
			else
				return DIAGONALV;
		}
	}
}

/*int CExtraMath::IsGreater(const int A, const int B)
{
	return B - A;
}

int CExtraMath::IsSmaller(const int A, const int B)
{
	return A - B;
}

int CExtraMath::IsGreaterP(const int * A, const int * B)
{
	return *B - *A;
}*/

/*int CExtraMath::IsLineToRight(const RangeLine * A, const RangeLine * B)
{
	return A->P2->GetX() - B->P1->GetX();
}*/

/*int CExtraMath::IsSmallerP(const int * A, const int * B)
{
	return *A - *B;
}

bool CExtraMath::IsIn(int SearchFor,int * InArray, int NoArrayMembers)
{
	int i;
	
	for(i = 0; i < NoArrayMembers; i++)
	{
		if(InArray[i] == SearchFor)
			return true;
	}
	
	return false;
}*/

int CExtraMath::CompareLineSegments(const LineSegment * A, const LineSegment * B)
{
	return B->y1 - A->y1;
}

double CExtraMath::FMin(double A, double B)
{
        if(A <= B)
                return A;
        else
                return B;
}

int CExtraMath::Max(int A, int B)
{
	if(A >= B)
		return A;
	else
		return B;
}

double CExtraMath::Max(double A, double B)
{
	if(A >= B)
		return A;
	else
		return B;
}

int CExtraMath::Max(int A, int B, int C)
{
	int Ret = A;

	if(B > Ret)
		Ret = B;
	if(C > Ret)
		Ret = C;

	return Ret;
}

int CExtraMath::Min(int A, int B)
{
	if(A <= B)
		return A;
	else
		return B;
}

long CExtraMath::Min(long A, long B)
{
	if(A <= B)
		return A;
	else
		return B;
}

/*uint16_t CExtraMath::Min(uint16_t A, uint16_t B)
{
	if(A <= B)
		return A;
	else
		return B;
}*/

unsigned CExtraMath::Min(unsigned A, unsigned B)
{
	if(A <= B)
		return A;
	else
		return B;
}

int CExtraMath::Min(int A, int B, int C)
{
	int Ret = A;

	if(B < Ret)
		Ret = B;
	if(C < Ret)
		Ret = C;

	return Ret;
}

/*
 * Converts radians to degrees
 */
double CExtraMath::RadiansToDegrees(double Radians)
{
	return 180.0 * Radians / M_PI;
}

/*
 * Traverses the specified line and returns a list containing each visited point.
 *
 * CC 29.09.2010
 */
list<Point> * CExtraMath::GetPointsOfLine(int x1, int y1, int x2, int y2)
{
	list<Point> * points = new list<Point>();
	Point p;

	//Vertical
	if (x1 == x2)
	{
		p.x = x1;
		for (p.y=min(y1,y2); p.y<=max(y1,y2); p.y++)
			points->push_back(p);
	}
	//Horizontal
	else if (y1 == y2)
	{
		p.y = y1;
		for (p.x=min(x1,x2); p.x<=max(x1,x2); p.x++)
			points->push_back(p);
	}
	//Diagonal
	else
	{
		if(abs(x2 - x1) >= abs(y2 - y1)) // Diagonal Line Closer To Horizontal
		{
			if(x2 < x1)
			{
				//Swap (to guarantee that x1 is always smaller than x2)
				int temp;
				temp = x1;
				x1 = x2;
				x2 = temp;

				temp = y1;
				y1 = y2;
				y2 = temp;
			}

			double gradient = double(y2 - y1) / double(x2 - x1);

			for(p.x = x1; p.x <= x2; p.x++)
			{
				p.y = int(double(p.x - x1) * gradient) + y1;
				points->push_back(p);
			}
		}
		else // Diagonal Line Closer To Vertical
		{
			if(y2 < y1)
			{
				//Swap (to guarantee that y1 is always smaller than y2)
				int temp;
				temp = x1;
				x1 = x2;
				x2 = temp;

				temp = y1;
				y1 = y2;
				y2 = temp;
			}

			double gradient = double(x2 - x1) / double(y2 - y1);

			for(p.y = y1; p.y <= y2; p.y++)
			{
				p.x = int(double(p.y - y1) * gradient) + x1;
				points->push_back(p);
			}
		}
	}

	return points;
}

/*
 * Calculates the intersection point of the linear functions defined by the 4 given points.
 *
 * 'ax1', 'ay1', 'ax2', 'ay2' (in) - Points that define linear function a.
 * 'bx1', 'by1', 'bx2', 'by2' (in) - Points that define linear function b.
 * 'intersectX', 'intersectX' (out) - Intersection point.
 * Returns true, if the functions have an intersection point, false otherwise.
 *
 * CC 10.02.2012
 * See: http://paulbourke.net/geometry/lineline2d/
 */
bool CExtraMath::GetInterceptionPointOfTwoLines(double ax1, double ay1, double ax2, double ay2, 
												double bx1, double by1, double bx2, double by2,
												double & intersectX, double & intersectY)
{
	double dividend = ((bx2-bx1)*(ay1-by1) - (by2-by1)*(ax1-bx1));
	double divisor = ((by2-by1)*(ax2-ax1) - (bx2-bx1)*(ay2-ay1));

	if (divisor == 0.0)
		return false; //Parallel lines

	double ua =  dividend / divisor;

	intersectX = ax1 + ua*(ax2-ax1);
	intersectY = ay1 + ua*(ay2-ay1);

	return true;
}


} //end namespace