#include "stdafx.h"
#include "CppUnitTest.h"
#include "PointList.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace PRImA;

/*
 * Unit tests for CPointList
 *
 * CC 12/05/2014
 */

namespace libextra
{
	TEST_CLASS(PointListTest)
	{
	public:
		
		TEST_METHOD(PointListBasicTest)
		{
			CPointList list;

			Assert::IsTrue(list.GetNoPoints() == 0, L"Empty new list");

			//Add
			CPolygonPoint * p1 = list.AddPoint();
			Assert::IsTrue(list.GetNoPoints() == 1, L"First point");
			CPolygonPoint * p2 = list.AddPoint(10,20);
			Assert::IsTrue(list.GetNoPoints() == 2, L"Second point");

			//Insert
			CPolygonPoint * p3 = new CPolygonPoint(10,5);
			list.InsertAfter(p1, p3);
			Assert::IsTrue(list.GetNoPoints() == 3, L"Inserted 3rd point");

			//Head and tail
			Assert::IsTrue(list.GetHeadPoint() == p1, L"p1 is head");
			Assert::IsTrue(list.GetTailPoint() == p2, L"p2 is tail");

			//Copy points to other list
			CPointList list2;
			list2.CopyPoints(list);
			Assert::IsTrue(list.GetNoPoints() == 3, L"Copied points");

			//Delete
			list2.DeletePoint(list2.GetHeadPoint()->GetNextPoint());
			Assert::IsTrue(list2.GetNoPoints() == 2, L"Deleted one point");
			list2.DeletePoint(0);
			Assert::IsTrue(list2.GetNoPoints() == 1, L"Deleted 2nd point");
			Assert::AreEqual(20, list2.GetHeadPoint()->GetY(), L"One point remaining");
			list2.DeletePoints();
			Assert::IsTrue(list2.GetNoPoints() == 0, L"All points deleted");
		}

		TEST_METHOD(PointListGeometricTests)
		{
			CPointList list1;
			list1.AddPoint(10,10);
			list1.AddPoint(100,10);
			list1.AddPoint(100,100);
			list1.AddPoint(10,100);

			CPointList list2;
			list2.AddPoint(50,50);
			list2.AddPoint(150,50);
			list2.AddPoint(150,150);
			list2.AddPoint(50,150);

			CPointList list3;
			list3.AddPoint(200,10);
			list3.AddPoint(250,10);
			list3.AddPoint(250,50);
			list3.AddPoint(200,50);

			//Bounding box overlap
			Assert::IsTrue(list1.DoBoundingBoxesOverlap(&list2), L"Bounding boxes of list 1 and list 2 do overlap");
			Assert::IsFalse(list1.DoBoundingBoxesOverlap(&list3), L"Bounding boxes of list 1 and list 3 do not overlap");
			Assert::IsTrue(list1.DoBoundingBoxesOverlap(20,20,50,50), L"Bounding box of list 1 overlaps specified box");
			
			//Line exists
			Assert::IsTrue(list1.DoesLineExist(list1.GetHeadPoint(), list1.GetHeadPoint()->GetNextPoint()), L"Line exits");
			Assert::IsFalse(list1.DoesLineExist(list1.GetHeadPoint()->GetNextPoint(), list1.GetTailPoint()), L"Line does not exits");

			//Area
			Assert::AreEqual(90*90, (int)list1.GetArea(), L"Area");

			//Bounding box
			Assert::AreEqual(10, (int)list1.GetBBX1(), L"BBX1");
			Assert::AreEqual(10, (int)list1.GetBBY1(), L"BBY1");
			Assert::AreEqual(100, (int)list1.GetBBX2(), L"BBX2");
			Assert::AreEqual(100, (int)list1.GetBBY2(), L"BBY2");

			//Indbound rect
			RECT r = list1.GetInboundRect();
			Assert::AreEqual((int)r.left, (int)list1.GetBBX1()+1, L"Inbound rect left");
			Assert::AreEqual((int)r.top, (int)list1.GetBBY1()+1, L"Inbound rect top");
			Assert::AreEqual((int)r.right, (int)list1.GetBBX2(), L"Inbound rect right");
			Assert::AreEqual((int)r.bottom, (int)list1.GetBBY2(), L"Inbound rect bottom");

			//Centroid (center of bounding box)
			Assert::AreEqual(55, (int)list1.GetCentroidX(), L"GetCentroidX");
			Assert::AreEqual(55, (int)list1.GetCentroidY(), L"GetCentroidY");

			//Centre (center of bounding box)
			Assert::AreEqual(55, (int)list1.GetCenterX(), L"GetCenterX");
			Assert::AreEqual(55, (int)list1.GetCenterY(), L"GetCenterY");

			//Centre of mass
			CPolygonPoint p = list1.GetCenterOfMass();
			Assert::AreEqual(55, (int)p.GetX(), L"GetCenterOfMassX");
			Assert::AreEqual(55, (int)p.GetY(), L"GetCenterOfMassY");

			//Height/width
			Assert::AreEqual(91, (int)list1.GetWidth(), L"width");
			Assert::AreEqual(91, (int)list1.GetHeight(), L"height");

			//Area inside
			Assert::IsTrue(list1.IsAreaInside(50,50,51,51), L"Area inside");
			Assert::IsFalse(list1.IsAreaInside(50,50,150,150), L"Area not inside");

			//Is point on line
			Assert::IsTrue(list1.IsPointOnLine(20, 10), L"Point on outline");
			Assert::IsFalse(list1.IsPointOnLine(20, 12), L"Point not on outline");

			//Simplify polygon
			CPointList list4;
			list4.AddPoint(10,10);
			list4.AddPoint(20,10);
			list4.AddPoint(100,10);
			list4.AddPoint(100,50);
			list4.AddPoint(100,100);
			list4.AddPoint(10,100);
			list4.SimplifyPolygon();
			Assert::IsTrue(list4.GetNoPoints() == 4, L"Simplified polygon");

			//Is rectangle, is isothetic
			Assert::IsTrue(list4.IsRectangle(), L"Rectangle");
			Assert::IsTrue(list4.IsIsothetic(), L"Rectangle");

			CPointList list5;
			list5.AddPoint(10,10);
			list5.AddPoint(20,10);
			list5.AddPoint(15,20);
			Assert::IsFalse(list5.IsRectangle(), L"Rectangle");
			Assert::IsFalse(list5.IsIsothetic(), L"Rectangle");
		}
	};
}