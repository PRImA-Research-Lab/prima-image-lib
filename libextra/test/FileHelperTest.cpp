#include "stdafx.h"
#include "CppUnitTest.h"
#include "ExtraFileHelper.h"
#include "UnitTestConstants.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace PRImA;

/*
 * Unit tests for CExtraFileHelper
 *
 * CC 09/05/2014
 */

namespace libextra
{
	TEST_CLASS(FileHelperTest)
	{

	public:
		
		TEST_METHOD(SplitPathTest)
		{
			CUniString filePath(L"c:\\temp\\EvalSet\\00000259.xml");
			CUniString pathOnly;
			CUniString filenameOnly;

			CExtraFileHelper::SplitPath(filePath, pathOnly, filenameOnly);

			Assert::AreEqual(L"c:\\temp\\EvalSet\\", pathOnly.GetBuffer(), "Path only");
			Assert::AreEqual(L"00000259.xml", filenameOnly.GetBuffer(), "Filename only");
		}

		TEST_METHOD(FileExistsTest)
		{
			CUniString testingFolder(CUnitTestConstants::TESTING_FOLDER);

			CUniString emptyFile = testingFolder;
			emptyFile.Append(L"empty_file.txt");
			Assert::IsTrue(CExtraFileHelper::FileExists(emptyFile), emptyFile.GetBuffer());

			CUniString phantomFile = testingFolder;
			phantomFile.Append(L"non_existing_file.txt");
			Assert::IsFalse(CExtraFileHelper::FileExists(phantomFile), phantomFile.GetBuffer());
		}

		TEST_METHOD(FolderExistsTest)
		{
			CUniString testingFolder(CUnitTestConstants::TESTING_FOLDER);

			Assert::IsTrue(CExtraFileHelper::FileExists(testingFolder), testingFolder.GetBuffer());

			CUniString phantomFolder("c:\\does_not_exist");
			Assert::IsFalse(CExtraFileHelper::FileExists(phantomFolder), phantomFolder.GetBuffer());
		}

		TEST_METHOD(CountFilesTest)
		{
			CUniString folder(CUnitTestConstants::TESTING_FOLDER);
			folder.Append(L"folder_with_2_files");

			Assert::IsTrue(CExtraFileHelper::FileExists(folder), folder.GetBuffer());

			Assert::IsTrue(CExtraFileHelper::CountFilesInFolder(folder) == 2, L"Two files in folder");
		}
	};


}