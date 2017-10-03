#include "stdafx.h"
#include "CppUnitTest.h"
#include "ExtraMath.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace PRImA;

/*
 * Unit tests for CExtraMath
 *
 * CC 09/05/2014
 */

namespace libextra
{
	TEST_CLASS(ExtraMathTest)
	{
	public:
		
		TEST_METHOD(DistancePointToLineTest)
		{
			//Horizontal line
			int x1 = 10;
			int x2 = 20;
			int y1 = 10;
			int y2 = 10;

			//Point below
			int xp = 15;
			int yp = 15;
			Assert::AreEqual(CExtraMath::DistancePointLine(xp, yp, x1, y1, x2, y2), 5.0, 0.1, L"Point below horizontal line");

			//Point above
			xp = 15;
			yp = 5;
			Assert::AreEqual(CExtraMath::DistancePointLine(xp, yp, x1, y1, x2, y2), 5.0, 0.1, L"Point above horizontal line");

			//Point on line
			xp = 15;
			yp = 10;
			Assert::AreEqual(CExtraMath::DistancePointLine(xp, yp, x1, y1, x2, y2), 0.0, 0.1, L"Point on horizontal line");

			//Vertical line
			x1 = 10;
			x2 = 10;
			y1 = 10;
			y2 = 20;

			//Point right
			xp = 15;
			yp = 15;
			Assert::AreEqual(CExtraMath::DistancePointLine(xp, yp, x1, y1, x2, y2), 5.0, 0.1, L"Point right of vertical line");

			//Point left
			xp = 5;
			yp = 15;
			Assert::AreEqual(CExtraMath::DistancePointLine(xp, yp, x1, y1, x2, y2), 5.0, 0.1, L"Point left of vertical line");

			//Point on line
			xp = 10;
			yp = 15;
			Assert::AreEqual(CExtraMath::DistancePointLine(xp, yp, x1, y1, x2, y2), 0.0, 0.1, L"Point on vertical line");
		}

		TEST_METHOD(DistancePointToPointTest)
		{
			Assert::AreEqual(CExtraMath::DistanceTwoPoints(1, 1, 2, 1), 1.0, 0.01, L"Point-to-point distance 1");
		}

		TEST_METHOD(LineIntersectionTestTest)
		{
			//Do they intercept?
			Assert::IsTrue(CExtraMath::DoLineSegmentsIntersect(0, 1, 10, 1, 5, 0, 5, 10), L"Line segments do intersect");
			Assert::IsFalse(CExtraMath::DoLineSegmentsIntersect(0, 1, 10, 1, 15, 0, 15, 10), L"Line segments do not intersect");

			//Where is the interception point?
			double interceptX = 0.0;
			double interceptY = 0.0;
			CExtraMath::GetInterceptionPointOfTwoLines(0, 1, 10, 1, 5, 0, 5, 10, interceptX, interceptY);
			Assert::AreEqual(interceptX, 5.0, 0.01, L"Interception point x");
			Assert::AreEqual(interceptY, 1.0, 0.01, L"Interception point y");
		}
	};
}