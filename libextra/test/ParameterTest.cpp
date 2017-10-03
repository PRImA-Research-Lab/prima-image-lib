#include "stdafx.h"
#include "CppUnitTest.h"
#include "Parameter.h"
#include "UnitTestConstants.h"
#include "ExtraFileHelper.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace PRImA;

/*
 * Unit tests for CParameter and child classes, as well as CParameterMap
 *
 * CC 09/05/2014
 */

namespace libextra
{
	TEST_CLASS(ParameterTest)
	{
	public:
		
		TEST_METHOD(BooleanParameterTest)
		{
			CBoolParameter p(1,"", "", true);

			//Value
			Assert::IsTrue(p.GetValue(), L"Initial value");
			Assert::IsFalse(p.IsSet(), L"Not set");
			p.SetValue(false);
			Assert::IsFalse(p.GetValue(), L"Changed value");
			Assert::IsTrue(p.IsSet(), L"Set");

			//Copy
			CBoolParameter p2(2,"", "", true);
			p.CopyTo(&p2);
			Assert::IsFalse(p2.GetValue(), L"Copied value");
		}

		TEST_METHOD(IntegerParameterTest)
		{
			CIntParameter p(1,"", "", 5, 1, 10, 1);

			//Value
			Assert::IsTrue(p.GetValue() == 5, L"Initial value");
			Assert::IsFalse(p.IsSet(), L"Not set");
			p.SetValue(6);
			Assert::IsTrue(p.GetValue() == 6, L"Changed value");
			Assert::IsTrue(p.IsSet(), L"Set");
			
			//Min, max, step
			Assert::IsTrue(p.GetMin() == 1, L"Min value");
			Assert::IsTrue(p.GetMax() == 10, L"Max value");
			Assert::IsTrue(p.GetStep() == 1, L"Step value");
			p.SetMin(2);
			Assert::IsTrue(p.GetMin() == 2, L"Changed min value");
			p.SetMax(11);
			Assert::IsTrue(p.GetMax() == 11, L"Changed max value");
			p.SetStep(2);
			Assert::IsTrue(p.GetStep() == 2, L"Changed step value");

			//Copy
			CIntParameter p2(2,"", "", 2);
			p.CopyTo(&p2);
			Assert::IsTrue(p2.GetValue() == 6, L"Copied value");

			//Check boundaries
			p.SetValue(0);
			Assert::IsTrue(p.GetValue() == 2, L"Min boundary");
			p.SetValue(15);
			Assert::IsTrue(p.GetValue() == 11, L"Max boundary");
		}

		TEST_METHOD(DoubleParameterTest)
		{
			CDoubleParameter p(1,"", "", 5.0, 1.0, 10.0, 1.0);

			//Value
			Assert::IsTrue(p.GetValue() == 5.0, L"Initial value");
			Assert::IsFalse(p.IsSet(), L"Not set");
			p.SetValue(6.0);
			Assert::IsTrue(p.GetValue() == 6.0, L"Changed value");
			Assert::IsTrue(p.IsSet(), L"Set");
			
			//Min, max, step
			Assert::IsTrue(p.GetMin() == 1.0, L"Min value");
			Assert::IsTrue(p.GetMax() == 10.0, L"Max value");
			Assert::IsTrue(p.GetStep() == 1.0, L"Step value");
			p.SetMin(2.0);
			Assert::IsTrue(p.GetMin() == 2.0, L"Changed min value");
			p.SetMax(11.0);
			Assert::IsTrue(p.GetMax() == 11.0, L"Changed max value");
			p.SetStep(2.0);
			Assert::IsTrue(p.GetStep() == 2.0, L"Changed step value");

			//Copy
			CDoubleParameter p2(2,"", "", 2.0);
			p.CopyTo(&p2);
			Assert::IsTrue(p2.GetValue() == 6.0, L"Copied value");

			//Check boundaries
			p.SetValue(0.0);
			Assert::IsTrue(p.GetValue() == 2.0, L"Min boundary");
			p.SetValue(15.0);
			Assert::IsTrue(p.GetValue() == 11.0, L"Max boundary");
		}

		TEST_METHOD(StringParameterTest)
		{
			CStringParameter p(1,"", "", CUniString(L"A"));

			//Value
			Assert::IsTrue(p.GetValue() == CUniString(L"A"), L"Initial value");
			Assert::IsFalse(p.IsSet(), L"Not set");
			p.SetValue(L"B");
			Assert::IsTrue(p.GetValue() == CUniString(L"B"), L"Changed value");
			Assert::IsTrue(p.IsSet(), L"Set");

			//Copy
			CStringParameter p2(2,"", "", CUniString(L"C"));
			p.CopyTo(&p2);
			Assert::IsTrue(p2.GetValue() == CUniString(L"B"), L"Copied value");

			//Valid values //CC: At the moment the value is not checked against valid values
			//set<CUniString> * validValues = p.GetValidValues();
			//validValues->insert(CUniString(L"bla"));
			//validValues->insert(CUniString(L"bli"));

			//p.SetValue(L"blu");
			//Assert::IsFalse(p.GetValue() == CUniString(L"blu"), L"Invalid value");
			//p.SetValue(L"bla");
			//Assert::IsFalse(p.GetValue() == CUniString(L"bla"), L"Valid value");
		}

		TEST_METHOD(ParameterMapTest)
		{
			CBoolParameter p1(1,"p1", "", true);
			p1.SetSortIndex(4);
			CIntParameter p2(2,"p2", "", 5, 1, 10, 1);
			p2.SetSortIndex(3);
			CDoubleParameter p3(3,"p3", "", 5.0, 1.0, 10.0, 1.0);
			p3.SetSortIndex(2);
			CStringParameter p4(4,"p4", "", CUniString(L"A"));
			p4.SetSortIndex(1);

			CParameterMap m;

			Assert::IsTrue(m.GetSize() == 0, L"New empty map");

			//Fill
			m.Add(&p1);
			m.Add(&p2);
			m.Add(&p3);
			m.Add(&p4);

			Assert::IsTrue(m.GetSize() == 4, L"Filled map");

			//Get
			Assert::IsTrue(m.Get(0) == &p1, L"Get by index 0");
			Assert::IsTrue(m.Get(2) == &p3, L"Get by index 2");
			Assert::IsTrue(m.Get(L"p2") == &p2, L"Get by name p2");
			Assert::IsTrue(m.Get(L"p4") == &p4, L"Get by name p4");
			Assert::IsTrue(m.GetParamForId(1) == &p1, L"Get by id 1");
			Assert::IsTrue(m.GetParamForId(2) == &p2, L"Get by id 2");

			//Sort
			m.SetSorted(true);
			Assert::IsTrue(m.Get(0) == &p4, L"Sorted get index 0");
			Assert::IsTrue(m.Get(1) == &p3, L"Sorted get index 1");
			Assert::IsTrue(m.Get(2) == &p2, L"Sorted get index 2");
			Assert::IsTrue(m.Get(3) == &p1, L"Sorted get index 3");

			//Clone
			CParameterMap * copy = m.Clone();
			Assert::IsTrue(copy->GetSize() == 4, L"Clone");

			//Copy values
			p1.SetValue(false);
			copy->CopyValues(&m);
			Assert::IsFalse(((CBoolParameter*)copy->GetParamForId(1))->GetValue(), L"Value copied");
			Assert::IsFalse(copy->GetParamForId(2)->IsSet(), L"Value copied");

			//Merge with
			CBoolParameter p5(5,"p5", "", true);
			m.Add(&p5);
			p2.SetValue(7);
			copy->MergeWith(&m);
			Assert::IsTrue(copy->GetSize() == 5, L"Merge - added parameter");
			Assert::AreEqual(((CIntParameter*)copy->GetParamForId(2))->GetValue(), 5, L"Merge - parameter version the same - value not copied");
			p2.SetVersion(p2.GetVersion()+1);
			copy->MergeWith(&m);
			Assert::AreEqual(((CIntParameter*)copy->GetParamForId(2))->GetValue(), 7, L"Merge - parameter version newer - value copied");

			//Save to ini
			p3.SetSet(true);
			CUniString filePath(CUnitTestConstants::TESTING_FOLDER);
			filePath.Append(L"params.ini");
			if (CExtraFileHelper::FileExists(filePath))
				CFile::Remove(filePath.GetBuffer());
			Assert::IsFalse(CExtraFileHelper::FileExists(filePath), L"Ini file does not exist");
			m.SaveToIni(filePath, L"Section1");

			//Remove
			m.Remove(&p5, false);
			Assert::IsTrue(m.GetSize() == 4, L"Removed one parameter");
			m.Remove(0, false);
			Assert::IsTrue(m.GetSize() == 3, L"Removed 2nd parameter");
			m.RemoveParamWithId(3, false);
			Assert::IsTrue(m.GetSize() == 2, L"Removed 3rd parameter");
			m.Clear();
			Assert::IsTrue(m.GetSize() == 0, L"Cleared");

			copy->DeleteAll();
			Assert::IsTrue(copy->GetSize() == 0, L"Deleted all");

			delete copy;
			copy = NULL;

			//Load from ini
			m.Add(&p1);
			p1.SetValue(true);
			p2.SetValue(3);
			m.Add(&p2);
			m.Add(&p3);
			m.Add(&p4);
			m.LoadFromIni(filePath, L"Section1");
			Assert::IsFalse(p1.GetValue(), L"Loaded from ini p1");
			Assert::IsTrue(p2.GetValue() == 7, L"Loaded from ini p2");
			Assert::IsTrue(p3.GetValue() == 5.0, L"Loaded from ini p3");

			
		}
	};
}