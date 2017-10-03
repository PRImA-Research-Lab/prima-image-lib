#include "stdafx.h"
#include "CppUnitTest.h"
#include "ExtraString.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace PRImA;

/*
 * Unit tests for CUniString and CExtraString
 *
 * CC 09/05/2014
 */

namespace libextra
{
	TEST_CLASS(ExtraStringTest)
	{
	public:
		
		TEST_METHOD(TestExtraStringMethods)
		{
			CUniString str(L"This is a test string");
			CUniString find(L" a ");
			CUniString replace(L" an extended ");
			CUniString extended(L"This is an extended test string");

			CExtraString::repl(str, find, replace);

			Assert::IsTrue(str == extended, L"Substring replaced");
		}

		TEST_METHOD(CUniStringTests)
		{
			//Operator =
			CUniString str(L"old");
			str = CUniString(L"new");
			Assert::IsTrue(str == CUniString(L"new"), L"Operator =");

			//ToUTF8
			//str = L"Ā";
			//const char * utf8 = str.ToUtf8C_Str();
			//CUniString out;
			//out.Append(utf8);
			//Assert::IsTrue(utf8[0] == 0xC4 && utf8[1] == 0x80, out.GetBuffer());

			//To double
			str = L"1.25";
			Assert::AreEqual(str.ToDouble(), 1.25, 0.01, L"String to double");

			//To int
			str = L"125";
			Assert::AreEqual(str.ToInt(), 125, L"String to integer");

			//Append
			str = L"1";
			str.Append(CUniString("2"));
			str.Append(3);
			str.Append("4");
			str.Append(L"5");
			str.Append(6.7);

			Assert::IsTrue(str == CUniString(L"123456.7"), L"Append");

			//Clear
			str.Clear();
			Assert::IsTrue(str == CUniString(L""), L"Clear");

			//IsEmpty
			Assert::IsTrue(str.IsEmpty(), L"IsEmpty true");
			str = L"bla";
			Assert::IsFalse(str.IsEmpty(), L"IsEmpty false");

			//Ends with
			Assert::IsTrue(str.EndsWith(L"la"), L"IsEmpty true");

			//Find
			Assert::AreEqual(str.Find(L"a"), 2, L"Find in string");

			//Split
			vector<CUniString> splitResult;
			str.Split(L"l", splitResult);
			Assert::AreEqual((int)splitResult.size(), 2, L"Split string");
			Assert::IsTrue(splitResult[0] == CUniString(L"b"), L"Split part 1");
			Assert::IsTrue(splitResult[1] == CUniString(L"a"), L"Split part 2");
		}
	};
}