#ifndef EXTRASTRING_H
#define EXTRASTRING_H

#ifdef _MSC_VER
#include "stdafx.h"
//#include <atlstr.h>
#include <afxmt.h>
#include <string>
#include <vector>

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif


using namespace std;

#endif

namespace PRImA 
{

class CUniString;

/*
 * Class CExtraString
 *
 * String helper functions.
 * 
 */
class DllExport CExtraString
{
	// METHODS
public:
	static char * CStringToCharArray(CString & Str);
	static bool IsAlpha(const char Char);
	static bool IsAlphaNumeric(const char Char);
	static bool IsNumeric(const char Char);
	static bool IsWSpace(const char Char);
	static int  strlen(double Value);
	static int  strlen(int    Value);
	static char * strrstr(char * str1, const char * str2 );
	static int repl(CUniString& s, const CUniString& from, const CUniString& to);
	static void XMLEncode(CUniString & value);
	static void XMLDecode(CUniString & Value);
	static void ConvertLineEndings(CUniString & value);
	static void CalculateLineAndColumn(CUniString & text, int pos, int & line, int & column);
	static int _httoi(const TCHAR *value);
	static CUniString CharCodeToString(int charCode);
};


/*
 * Class CUniString
 *
 * Extended string class, able to handle all different string types
 * of c++ (char*, wchar*, stl::string, CString). It also includes
 * UTF-8 encoding and decoding.
 *
 * CC 17.02.2010 - created
 */
class DllExport CUniString : public CString
{
public:
	CUniString();
	CUniString(const CUniString & str); //Copy constructor
	CUniString(CString & str);
	CUniString(string stlString);
	CUniString(const char * charArray);
	CUniString(unsigned char * charArray);
	CUniString(unsigned char * charArray, bool decodeUtf8);
	CUniString(wchar_t * wcharArray);
	CUniString(const wchar_t * wcharArray);
	CUniString(CUniString * str);
	~CUniString();

	CUniString & operator=(const CUniString & other);

	const char * ToUtf8C_Str();
	const char * ToC_Str();
	double ToDouble();
	int ToInt();
	//const unsigned char * ToUC_Str();

	void Append(CUniString & str);
	void Append(int number);
	void Append(__int64 number);
	void Append(const char * charArray);
	void Append(const wchar_t * wcharArray);
	void Append(double number, int decimalDigits = 1);
	void Append(wchar_t c);
	void Clear();

	int Find(const char * charArray);
	int Find(const wchar_t * wcharArray);
	int Find(CUniString & str);
	int FindLast(CUniString & str);

	bool EndsWith(CUniString str);

	void DecodeUtf8();

	void Split(CUniString splitBy, vector<CUniString> & result);

	virtual ATL::CSimpleStringT<TCHAR,false>::PXSTR GetBuffer();

private:
	CCriticalSection * GetCriticalSection();

private:
	void Init();
	char * m_CharBuffer;
	static CUniString DecodeUtf8(unsigned char * charArray);
	CCriticalSection * m_CriticalSection;
};

}

#else

namespace PRImA 
{
class CExtraString;
}
#endif
