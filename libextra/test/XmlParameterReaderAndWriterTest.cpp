#include "stdafx.h"
#include "CppUnitTest.h"
#include "ExtraFileHelper.h"
#include "msxmlparameterreader.h"
#include "msxmlparameterwriter.h"
#include "UnitTestConstants.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace PRImA;

/*
 * Unit tests for CMsXmlParameterReader and CMsXmlParameterWriter
 *
 * CC 09/05/2014
 */

namespace libextra
{
	TEST_CLASS(XmlParameterReaderAndWriterTest)
	{
	public:
		
		TEST_METHOD(ParameterReaderAndWriterTest)
		{
			CBoolParameter p1(1,"p1", "", true);
			p1.SetSortIndex(4);
			p1.SetValue(true);
			CIntParameter p2(2,"p2", "", 5, 1, 10, 1);
			p2.SetSortIndex(3);
			CDoubleParameter p3(3,"p3", "", 5.0, 1.0, 10.0, 1.0);
			p3.SetSortIndex(2);
			CStringParameter p4(4,"p4", "", CUniString(L"A"));
			p4.SetSortIndex(1);

			CParameterMap m;
			m.Add(&p1);
			m.Add(&p2);
			m.Add(&p3);
			m.Add(&p4);

			//Delete old file
			CUniString filePath(CUnitTestConstants::TESTING_FOLDER);
			filePath.Append(L"params.xml");
			if (CExtraFileHelper::FileExists(filePath))
				CFile::Remove(filePath.GetBuffer());
			Assert::IsFalse(CExtraFileHelper::FileExists(filePath), L"Xml file does not exist");

			//Save
			CMsXmlParameterWriter writer;
			writer.WriteParameters(&m, filePath.ToC_Str());

			//Load
			CMsXmlParameterReader reader;
			CParameterMap * loaded = reader.ReadParameters(filePath.ToC_Str());

			Assert::IsTrue(loaded != NULL, L"Map loaded");
			Assert::IsTrue(loaded->GetSize() == 4, L"Not empty");

			CParameter * l1 = loaded->Get(CUniString(L"p1"));
			CParameter * l2 = loaded->Get(CUniString(L"p2"));
			CParameter * l3 = loaded->Get(CUniString(L"p3"));
			CParameter * l4 = loaded->Get(CUniString(L"p4"));
			Assert::IsTrue(l1->GetType() == CParameter::TYPE_BOOL, L"Loaded bool parameter");
			Assert::IsTrue(l2->GetType() == CParameter::TYPE_INT, L"Loaded int parameter");
			Assert::IsTrue(l3->GetType() == CParameter::TYPE_DOUBLE, L"Loaded double parameter");
			Assert::IsTrue(l4->GetType() == CParameter::TYPE_STRING, L"Loaded string parameter");

			Assert::IsTrue(((CBoolParameter*)l1)->IsSet(), L"Correct bool is set state");
			Assert::IsTrue(((CBoolParameter*)l1)->GetValue(), L"Correct bool value");
			Assert::IsTrue(((CBoolParameter*)l1)->GetSortIndex() == 4, L"Correct bool sort index");
		}

	};
}