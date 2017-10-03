#include "stdafx.h"
#include "CppUnitTest.h"
#include "algorithm.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace PRImA;

/*
 * Unit tests for CAlgorithm
 *
 * CC 09/05/2014
 */

namespace libextra
{
	//Algorithm child class for testing
	class CAnyAlg : public CAlgorithm
	{
		public:
			CAnyAlg();
			void DoRun();
			inline bool IsStoppable() { return true; };
	};

	CAnyAlg::CAnyAlg()
	{
		AddParameter(new CIntParameter("","",1));
	}

	void CAnyAlg::DoRun()
	{
		SetProgress(0);
		for (int i=0; i<10; i++)
		{
			Sleep(100);
			if (HasStopSignal())
				return;
			SetProgress((i+1)*10);
		}
		m_Success = true;
	}



	//Tests
	TEST_CLASS(AlgorithmTest)
	{
	public:
		
		TEST_METHOD(AlgorithmParametersTest)
		{
			CAnyAlg alg;
			Assert::IsTrue(alg.GetParameterCount() == 1, L"Algorithm has one parameter");
			Assert::IsTrue(alg.GetParameter(0)->GetType() == CParameter::TYPE_INT, L"Algorithm parameter is of type integer");
		}

		TEST_METHOD(RunAlgorithmInThreadTest)
		{
			CAnyAlg alg;
			Assert::IsFalse(alg.IsRunning(), L"Algorithm not started yet");

			alg.RunAsync();

			Sleep(500);
			Assert::IsTrue(alg.IsRunning(), L"Algorithm running");

			Sleep(1000);
			Assert::IsFalse(alg.IsRunning(), L"Algorithm finished");
			Assert::IsTrue(alg.IsSuccess(), L"Algorithm successful");
		}

		TEST_METHOD(RunAlgorithmSynchronousTest)
		{
			CAnyAlg alg;
			Assert::IsFalse(alg.IsRunning(), L"Algorithm not started yet");

			alg.Run();

			Assert::IsFalse(alg.IsRunning(), L"Algorithm finished");
			Assert::IsTrue(alg.IsSuccess(), L"Algorithm successful");
		}

		TEST_METHOD(StopRunningAlgorithmTest)
		{
			CAnyAlg alg;
			Assert::IsFalse(alg.IsRunning(), L"Algorithm not started yet");

			alg.RunAsync();

			Sleep(500);
			
			alg.SendStopSignal();

			Sleep(200);
			Assert::IsFalse(alg.IsRunning(), L"Algorithm stopped");
			Assert::IsFalse(alg.IsSuccess(), L"Algorithm could not finish");
		}

		TEST_METHOD(AlgorithmProgressTest)
		{
			CAnyAlg alg;
			Assert::IsTrue(alg.GetProgress() == 0, L"Algorithm progress 0");

			alg.Run();

			Assert::IsTrue(alg.GetProgress() == 100, L"Algorithm progress 100");
		}
	};
}