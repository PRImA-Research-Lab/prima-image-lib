#pragma once

#include <string>
#include "wchar.h"
#include "ExtraString.h"

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif

using namespace std;

namespace PRImA 
{

/*
 * Class CExtraFileHelper
 *
 * Provides static methods for file and directory handling.
 *
 * CC 17.12.2009 - created
 */

class DllExport CExtraFileHelper
{
public:
	CExtraFileHelper(void);
	~CExtraFileHelper(void);

	static void SplitPath(CString &  FullPath, CString & PathOnly, CString & FileNameOnly);
	static CUniString RemoveFileExtension(CUniString filename);
	static bool	FileExists(string* fileName);
	static bool	FileExists(const char * fileName);
	static bool	FileExists(CUniString & fileName);
	static int CountFilesInFolder(CUniString & folderPath, bool bCountHidden=false);
	static void CreateFolder(CUniString path);
	static bool WriteToFile(CUniString contentToWrite, CUniString fileName, bool addByteOrderMark = true);
	static bool ReadFromFile(CUniString fileName, CUniString & target, bool utf8 = false);
	static bool WriteToFile(CByteArray * contentToWrite, CUniString fileName);
	static bool ReadFromFile(CUniString fileName, CByteArray * target);
	static bool IsAccessPermitted(CUniString folder);

	static void CollectFiles(CUniString folderPath, CUniString fileExtension, vector<CUniString> & fileNames);
};

}