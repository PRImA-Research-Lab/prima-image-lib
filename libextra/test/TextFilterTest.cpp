#include "stdafx.h"
#include "CppUnitTest.h"
#include "TextFilter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace PRImA;

/*
 * Unit tests for CTextFilter and replacement rule classes
 *
 * CC 09/05/2014
 */

namespace libextra
{
	TEST_CLASS(TextFilterTest)
	{
	public:
		
		TEST_METHOD(TextfilterTests)
		{

			CStringParameter rule1(1, "", "", L"00AC:=002D"); //Not sign to hyphen
			CStringParameter rule2(1, "", "", L"_MULTSPACE_:=0020"); //Multiple spaces
			CStringParameter rule3(1, "", "", L"_STARTSPACE_:="); //Start with space
			CStringParameter rule4(1, "", "", L"_ENDSPACE_:="); //End with space
			CStringParameter rule5(1, "", "", L"0061:=0061,0061"); //a -> aa

			CParameterMap rules;
			rules.Add(&rule1);
			rules.Add(&rule2);
			rules.Add(&rule3);
			rules.Add(&rule4);
			rules.Add(&rule5);

			CUniString text(L" Bla bla bla¬\r\n\r\nbla  bla bla ");

			CTextFilter filter(rules.Clone());

			//Check text
			set<CUniString> res = filter.CheckText(text);

			//Apply to text
			filter.ApplyFilter(text);

			Assert::IsFalse(text.Find(L"¬") > 0, L"Not sign removed");
			Assert::IsTrue(text.Find(L"-") > 0, L"Hyphen added");

			Assert::IsFalse(text.Find(L"  ") > 0, L"Multiple spaces removed");
			Assert::IsTrue(text.Find(L" ") >= 0, L"Multiple spaces replaced with one space");

			Assert::IsTrue(text.Find(L" ") > 0, L"Removed space at start");

			Assert::IsFalse(text.EndsWith(L" "), L"Removed space at end");

			Assert::IsTrue(text.Find(L"aa") > 0, L"a -> aa");
		}

	};
}