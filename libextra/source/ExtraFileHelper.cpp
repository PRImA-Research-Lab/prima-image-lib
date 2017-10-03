#include "StdAfx.h"
#include "ExtraFileHelper.h"
#include <sys/stat.h>
#include <strsafe.h>
#include <wchar.h>
#include <windows.h>
//#include <svrapi.h>
#include <stdio.h>

namespace PRImA
{

/*
 * Class CExtraFileHelper
 *
 * Provides static methods for file and directory handling.
 *
 * CC 17.12.2009 - created
 */

/*
 * Constructor
 */
CExtraFileHelper::CExtraFileHelper(void)
{
}

/*
 * Destructor
 */
CExtraFileHelper::~CExtraFileHelper(void)
{
}

/*
 * Splits the given file path in path and filename.
 *
 * 'FullPath' (in)
 * 'PathOnly' (out)
 * 'FileNameOnly' (out)
 */
void CExtraFileHelper::SplitPath(CString &  FullPath, CString & PathOnly, CString & FileNameOnly)
{
	int DirSlash = FullPath.ReverseFind(_T('\\'));

	if(DirSlash == -1)
	{
		PathOnly = _T("");
		FileNameOnly = FullPath;
	}
	else
	{
		PathOnly = FullPath.Left(DirSlash + 1);
		FileNameOnly = FullPath.Right(FullPath.GetLength() - DirSlash - 1);
	}
}

/*
 * Removes the file extension from the given filename and returns the result
 */
CUniString CExtraFileHelper::RemoveFileExtension(CUniString filename)
{
	int pos = filename.FindLast(CUniString(L"."));
	if (pos < 0)
		return filename;
	return filename.Left(pos);
}

/*
 * Checks if file or directory exists.
 */
bool CExtraFileHelper::FileExists(CUniString & fileName)
{
	//Remove trailing backslash
	if (fileName.EndsWith(L"\\"))
	{
		CUniString folder = fileName.Left(fileName.GetLength() - 1);
		//return PathIsDirectory(folder.GetBuffer()) == TRUE;
		return FileExists(folder);
	}

	//CC 25/09/2015: Now not referred to the "char *" method to support Unicode paths
	struct _stat64i32 stFileInfo;
	bool blnReturn;
	int intStat;

	// Attempt to get the file attributes
	intStat = _wstat(fileName,&stFileInfo);
	if(intStat == 0) 
	{
		// We were able to get the file attributes
		// so the file obviously exists.
		blnReturn = true;
	} 
	else 
	{
		// We were not able to get the file attributes.
		// This may mean that we don't have permission to
		// access the folder which contains this file. If you
		// need to do that level of checking, lookup the
		// return values of stat which will give you
		// more details on why stat failed.
		blnReturn = false;
	}
	return(blnReturn);
}

/*
 * Checks if file or directory exists.
 */
bool CExtraFileHelper::FileExists(string * fileName)
{
	return CExtraFileHelper::FileExists(fileName->c_str());
}

/*
 * Checks if file or directory exists.
 *
 * Note: If called for a directory and the path ends with a backslash, false is returned!
 *       Remove the ending \ before checking the existence or use the method for CUniString instead.
 *
 * See: http://www.techbytes.ca/techbyte103.html
 */
bool CExtraFileHelper::FileExists(const char * fileName)
{
	struct stat stFileInfo;
	bool blnReturn;
	int intStat;

	// Attempt to get the file attributes
	intStat = stat(fileName,&stFileInfo);
	if(intStat == 0) 
	{
		// We were able to get the file attributes
		// so the file obviously exists.
		blnReturn = true;
	} 
	else 
	{
		// We were not able to get the file attributes.
		// This may mean that we don't have permission to
		// access the folder which contains this file. If you
		// need to do that level of checking, lookup the
		// return values of stat which will give you
		// more details on why stat failed.
		blnReturn = false;
	}
	return(blnReturn);
}


/*
 * CountFilesInDir takes a CUniString as the folder path and a boolean of whether to count hidden files
 * If the folder doesn't exist -1 is returned
 * AJF - 06/07/2011
 */
int CExtraFileHelper::CountFilesInFolder(CUniString & folderPath, bool bCountHidden /* = false */)
{
	LPTSTR directory = (LPTSTR)folderPath.GetBuffer();
	TCHAR szDir[MAX_PATH];
	WIN32_FIND_DATA fd;
	DWORD dwAttr = FILE_ATTRIBUTE_DIRECTORY;
	
	if( !bCountHidden)
		dwAttr |= FILE_ATTRIBUTE_HIDDEN;
	
	StringCchCopy(szDir, MAX_PATH, directory);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	HANDLE hFind = FindFirstFile( szDir, &fd);

	if(hFind != INVALID_HANDLE_VALUE)
	{
		int count = 0;
		do
		{
			if( !(fd.dwFileAttributes & dwAttr))
				count++;
		}while( FindNextFile( hFind, &fd));
		FindClose( hFind);
		return count;
	}
	return -1;
}

/*
 * Creates the folder specified in the given path.
 */
void CExtraFileHelper::CreateFolder(CUniString path)
{
	CreateDirectory(path, NULL);
}

/*
 * Writes the given string to a text file of the specified name
 */
bool CExtraFileHelper::WriteToFile(CUniString contentToWrite, CUniString fileName, bool addByteOrderMark /*= true*/)
{
	TCHAR * sfile = fileName.GetBuffer();
	DWORD dwBytesWritten;
	SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);
	HANDLE hFile = CreateFile(sfile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	TCHAR * swork = contentToWrite.GetBuffer();

	size_t llen = _tcslen(swork);
	if (addByteOrderMark)
	{
		unsigned char smarker[3];

		smarker[0] = 0xEF;
		smarker[1] = 0xBB;
		smarker[2] = 0xBF;

		WriteFile(hFile, smarker, 3, &dwBytesWritten, NULL);
	}

	char *m_CharBuffer = NULL;
	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, swork, -1, m_CharBuffer, 0, NULL, NULL);
	m_CharBuffer = new char[bufferSize+1];
	WideCharToMultiByte(CP_UTF8, 0, swork, -1, m_CharBuffer, bufferSize, NULL, NULL);

	WriteFile(hFile, m_CharBuffer, (DWORD)strlen(m_CharBuffer), &dwBytesWritten, NULL);

	free(m_CharBuffer);
	SetEndOfFile(hFile);
	CloseHandle(hFile);

	return true;
}

/*
 * Loads a text file and fills the specified target string variable with the content
 * 'utf8' - If set to true, a special mode for UTF-8 is used. Otherwise the default CStdioFile read function is used.
 */
bool CExtraFileHelper::ReadFromFile(CUniString fileName, CUniString & target, bool utf8 /*=false*/)
{
	target.Clear(); 
	CStdioFile * file = NULL;
	TRY 
	{ 
		if (utf8)
		{
			FILE * fStream;
			errno_t errCode = _tfopen_s(&fStream, fileName, _T("r, ccs=UTF-8"));
			if (0 != errCode)
			{
				return false;
			}
			file = new CStdioFile(fStream);
		}
		else
		{
			file= new CStdioFile(fileName.GetBuffer(), CFile::modeRead);
		}

		CUniString line;
		while(file->ReadString(line)) 
		{
			if (!target.IsEmpty())
				target.Append(L"\n");
			target.Append(line);
		}
		file->Close();
		delete file;
		return true;
	} 
	CATCH_ALL(e) 
	{ 
		e->ReportError(); // shows what's going wrong 
	} 
	END_CATCH_ALL 
	
	return false;
}

/*
 * Writes the given byte array to a file of the specified name
 */
bool CExtraFileHelper::WriteToFile(CByteArray * contentToWrite, CUniString fileName)
{
	TCHAR * sfile = fileName.GetBuffer();
	SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);
	HANDLE hFile = CreateFile(sfile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	CFile myFile(hFile);

	myFile.Write(contentToWrite->GetData(), contentToWrite->GetSize());

	myFile.Close();

	return true;
}

/*
 * Loads a file and fills the specified target byte array with the content
 */
bool CExtraFileHelper::ReadFromFile(CUniString fileName, CByteArray * target)
{
	CFile myFile;
	if(myFile.Open(fileName, CFile::modeRead | CFile::typeBinary | CFile::modeNoTruncate))
	{
		CByteArray buffer;
		buffer.SetSize(1024); // ensure that buffer is allocated and the size we want it
		UINT bytesRead = 0;
		
		while ((bytesRead = myFile.Read(buffer.GetData(), buffer.GetSize())) > 0)
		{
			if (bytesRead < 1024)
				buffer.SetSize(bytesRead);
			target->Append(buffer);
		}
			
		return true;
	}
	return false;
}

/*
 * Checks if the current user has write permission in the specified folder
 */
bool CExtraFileHelper::IsAccessPermitted(CUniString folder) 
{
	//See http://www.experts-exchange.com/Programming/System/Q_23779515.html

    const DWORD ACCESS_READ  = 1; //CC: Is that correct?
	const DWORD ACCESS_WRITE = 2;

    HANDLE hToken;
	// AccessCheck() variables
	DWORD           dwAccessDesired;
	PRIVILEGE_SET   PrivilegeSet;
	DWORD           dwPrivSetSize;
	DWORD           dwAccessGranted;
	BOOL            fAccessGranted = FALSE;
	GENERIC_MAPPING GenericMapping;
 
    SECURITY_INFORMATION si = (SECURITY_INFORMATION)( OWNER_SECURITY_INFORMATION
            | GROUP_SECURITY_INFORMATION
            | DACL_SECURITY_INFORMATION);
 
 
	PSECURITY_DESCRIPTOR psdSD = NULL;
	DWORD dwNeeded;
 
	GetFileSecurity(folder.GetBuffer(), si, NULL, 0, &dwNeeded);
 
 
	psdSD = (PSECURITY_DESCRIPTOR) new BYTE[dwNeeded];
 
	if (!GetFileSecurity(folder.GetBuffer(), si, psdSD, dwNeeded, &dwNeeded)) 
	{
          //printf("GetFileSecurity\n");
    }
 
    // AccessCheck() requires an impersonation token.
    ImpersonateSelf(SecurityImpersonation);
 
    OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, TRUE, &hToken);
 
    // Using AccessCheck(), there are two different things we could do:
    // 
    // 1. See if we have Read/Write access to the object.
    // 
    dwAccessDesired = ACCESS_WRITE;
 
    // Initialize generic mapping structure to map all.
    memset(&GenericMapping, 0xff, sizeof(GENERIC_MAPPING));
    GenericMapping.GenericRead = ACCESS_READ;
    GenericMapping.GenericWrite = ACCESS_WRITE;
    GenericMapping.GenericExecute = 0;
    GenericMapping.GenericAll = ACCESS_READ | ACCESS_WRITE;
 
    // This only does something if we want to use generic access
    // rights, like GENERIC_ALL, in our call to AccessCheck().
    MapGenericMask(&dwAccessDesired, &GenericMapping);
 
    dwPrivSetSize = sizeof(PRIVILEGE_SET);
 
    //printf("calling 'AccessCheck()'...\n");
 
    // Make the AccessCheck() call.
    if (!AccessCheck(psdSD, hToken, dwAccessDesired, &GenericMapping,
            &PrivilegeSet, &dwPrivSetSize, &dwAccessGranted, 
            &fAccessGranted))
	{
         //printf("AccessCheck: %d\n",GetLastError());
    }
 
    //printf("... 'AccessCheck()' succeeded\n");
    delete [] psdSD;
 
	return fAccessGranted == TRUE;
}

/*
* Retunrs a list of all files in the specified folder with the given file extension.
* 'fileNames' (out) - Target list for file paths
*/
void CExtraFileHelper::CollectFiles(CUniString folderPath, CUniString fileExtension, vector<CUniString> & fileNames)
{
	if (!folderPath.IsEmpty())
	{
		CUniString path(folderPath);
		path.Append(_T("\\*."));
		path.Append(fileExtension);

		WIN32_FIND_DATA findFileData;
		HANDLE handle;

		handle = FindFirstFile(path.GetBuffer(), &findFileData);

		if (handle != INVALID_HANDLE_VALUE) //found a file
		{
			BOOL foundFile = FALSE;
			int count = 0;
			do
			{
				fileNames.push_back(CUniString(findFileData.cFileName));

				foundFile = FindNextFile(handle, &findFileData);
			} while (foundFile);
			FindClose(handle);
		}
	}
}


} //end namespace