#include "stdafx.h"
#include "CppUnitTest.h"
#include "ini.h"
#include "UnitTestConstants.h"
#include "extrastring.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace PRImA;

/*
 * Unit tests for CIniReader
 *
 * CC 09/05/2014
 */
namespace libextra
{
	TEST_CLASS(IniReaderTest)
	{
	public:
		
		TEST_METHOD(ReadIniFileTest)
		{
			CUniString filePath(CUnitTestConstants::TESTING_FOLDER);
			filePath.Append(L"test.ini");

			CIniReader reader(filePath);

			//Section exists
			Assert::IsTrue(reader.sectionExists(L"Section1"), L"Section 1 exists");
			Assert::IsTrue(reader.sectionExists(L"Section2"), L"Section 2 exists");
			Assert::IsFalse(reader.sectionExists(L"Section3"), L"Section 3 doesn't exist");

			//Values
			Assert::IsTrue(reader.getKeyValue(L"key1", L"Section1") == CUniString(L"value1"), L"Value 1");
			Assert::IsTrue(reader.getKeyValue(L"key2", L"Section1") == CUniString(L""), L"Value 2");
			Assert::IsTrue(reader.getKeyValue(L"key3", L"Section2") == CUniString(L"value3"), L"Value 3");
			Assert::IsTrue(reader.getKeyValue(L"key4", L"Section2") == CUniString(L""), L"Invalid key");

		}

	};
}