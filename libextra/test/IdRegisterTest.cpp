#include "stdafx.h"
#include "CppUnitTest.h"
#include "IdRegister.h"
#include "Algorithm.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace PRImA;

/*
 * Unit tests for CIdRegister
 *
 * CC 09/05/2014
 */

namespace libextra
{
	TEST_CLASS(IdRegisterTest)
	{
	public:
		
		TEST_METHOD(TestIdRegister)
		{
			CIdRegister<CUniString*> idReg;

			//Create ID
			CUniString id = idReg.CreateId((CUniString*)NULL, L'i');
			Assert::IsTrue(id.Find(L"i") == 0, L"Create ID");

			//Register ID / contains ID
			idReg.RegisterId(CUniString(L"m1"), (CUniString*)NULL);
			Assert::IsTrue(idReg.HasId(CUniString(L"m1")), L"Regsiter ID");

			//Unregister ID
			idReg.UnregisterId(CUniString(L"m1"));
			Assert::IsFalse(idReg.HasId(CUniString(L"m1")), L"Unregsiter ID");

			//ID valid?
			Assert::IsTrue(idReg.IsIdValid(CUniString(L"m1")), L"Valid ID");
			Assert::IsFalse(idReg.IsIdValid(CUniString(L"1m")), L"Invalid ID");

			//Get valid ID
			id = idReg.GetValidId(L'p');
			Assert::IsTrue(id.Find(L"p") == 0, L"Get valid ID");

			//Collisions
			Assert::IsFalse(idReg.HasCollisions(), L"No collisions");
			idReg.RegisterId(CUniString(L"m1"), (CUniString*)NULL);
			idReg.RegisterId(CUniString(L"m1"), (CUniString*)NULL);
			Assert::IsTrue(idReg.HasCollisions(), L"Has collisions");
		}

	};
}