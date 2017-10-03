#include "ExtraString.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

namespace PRImA
{

// We need in following line to check if we're using MFC. In absence of better solution, check instead for MSVC
char * CExtraString::CStringToCharArray(CString & Str)
{
	#ifdef UNICODE
		char * Ret = new char[Str.GetLength() + 2];
		size_t Converted = 0;
		wcstombs_s(&Converted, Ret, Str.GetLength()+1, Str, _TRUNCATE);
		return Ret;
	#else
		return NULL;
	#endif
}

/*
 * Checks if the given character is a letter of the Latin alphabet
 */
bool CExtraString::IsAlpha(const char Char)
{
	if(Char >= 'A' && Char <= 'Z')
		return true;
	else if(Char >= 'a' && Char <= 'z')
		return true;
	return false;
}

/*
 * Checks if the given character is a letter of the Latin alphabet or a digit
 */
bool CExtraString::IsAlphaNumeric(const char Char)
{
	return (IsAlpha(Char) || IsNumeric(Char));
}

/*
 * Checks if the given character is a digit (0..9)
 */
bool CExtraString::IsNumeric(const char Char)
{
	if(Char >= '0' && Char <= '9')
		return true;
	return false;
}

/*
 * Checks if the given character is a white space (space or tab)
 */
bool CExtraString::IsWSpace(const char Char)
{
	if(Char == ' ' || Char == '\t')
		return true;
	else
		return false;
}

/*
 * Returns the length of the string a conversion from the given double value would produce (6 digits after decimal point)
 */
int CExtraString::strlen(double Value)
{
	if(Value == 0.0)
		return 1 + 1 + 6;
	else if(Value > 0)
		return int(log10(Value)) + 1 + 1 + 6;
	else
		return int(log10((double) abs(int(Value)))) + 1 + 1 + 1 + 6;
}

/*
 * Returns the length of the string a conversion from the given integer value would produce
 */
int CExtraString::strlen(int Value)
{
	if(Value == 0)
		return 1;
	else if(Value > 0)
		return int(log10(double(Value))) + 1;
	else
		return int(log10(double(abs(Value)))) + 2;

}

/*char * CExtraString::strrstr(char * str1, const char * str2 )
{
	int i;
	unsigned int j;
	bool Matched;

	for(i = (int)std::strlen(str1) - (int)std::strlen(str2); i >= 0; i--)
	{
		Matched = true;

		for(j = 0; j < (int)std::strlen(str2) && Matched; j++)
		{
			if(str1[i + j] != str2[j])
				Matched = false;
		}

		if(Matched)
			return &(str1[i]);
	}

	return NULL;
}*/

/*
 * Replaces all occurances of 'from' with 'to' in the given string.
 */
int CExtraString::repl(CUniString& s, const CUniString& from, const CUniString& to)
{
	int cnt = 0;

	if(from != to && !from.IsEmpty())
	{
		cnt += s.Replace(from, to);
	}
	return cnt;
}

/*
 * Replaces all '&', ''', '"', '<' and '>' in the given string with '&...;'
 * CC 22.02.2010 - commeted out the encoding because libxml does the encoding now
 */
void CExtraString::XMLEncode(CUniString & value)
{
	//repl(value, CUniString("&"), CUniString("&amp;"));
	//repl(value, CUniString("'"), CUniString("&apos;"));
	//repl(value, CUniString("\""), CUniString("&quot;"));
	//repl(value, CUniString("<"), CUniString("&lt;"));
	//repl(value, CUniString(">"), CUniString("&gt;"));
} 

/*
 * Replaces all '&...;' in the given string back to '&', ''', '"', '<' and '>'
 */
void CExtraString::XMLDecode(CUniString & value)
{
	repl(value, CUniString("&amp;"), CUniString("&"));
	repl(value, CUniString("&apos;"), CUniString("'"));
	repl(value, CUniString("&quot;"), CUniString("\""));
	repl(value, CUniString("&lt;"), CUniString("<"));
	repl(value, CUniString("&gt;"), CUniString(">"));
}

/*
 * Convert all line breaks to windows style (line break and carriage return).
 */
void CExtraString::ConvertLineEndings(CUniString & value)
{
	unsigned char * buffer = new unsigned char[2];
	buffer[1] = 0;

	buffer[0] = 10;
	CUniString lineBreak(buffer);
	buffer[0] = 13;
	CUniString carriageReturn(buffer);
	CUniString carriageReturnAndLineBreak = carriageReturn;
	carriageReturnAndLineBreak.Append(lineBreak);
	delete [] buffer;

	//Also handle the unicode line separator (U+2028)
	wchar_t * wbuffer = new wchar_t[2];
	wbuffer[0] = 0x2028;
	wbuffer[1] = 0;
	CUniString lineSep(wbuffer);
	delete [] wbuffer;

	repl(value, carriageReturnAndLineBreak, lineBreak); //to avoid double carriage returns
	repl(value, lineBreak, carriageReturnAndLineBreak);
	repl(value, lineSep, carriageReturnAndLineBreak);
	//repl(value, lineSep, lineBreak);
}

/*
 * Calculates the line and column number of the specified position in the given text
 *
 */
void CExtraString::CalculateLineAndColumn(CUniString & text, int pos, int & line, int & column)
{
	column = 0;
	const wchar_t * buffer = text.GetBuffer();

	//Count new lines
	int newLineCount = 0;
	for (int i=pos; i>=0; i--)
	{
		if (buffer[i] == '\n')
		{
			if (newLineCount==0) //Still in the start line -> determine column number
				column = pos-i;
			newLineCount++;
		}
	}
	if (newLineCount==0) //Only one line
		column = pos+1;
	line = newLineCount + 1;
}

/*
 * Converts an hexadecimal string (e.g. 0xB3CD or 3A41) to an integer.
 * See http://www.codeproject.com/KB/string/hexstrtoint.aspx
 */
int CExtraString::_httoi(const TCHAR *value)
{
  struct CHexMap
  {
    TCHAR chr;
    int value;
  };
  const int HexMapL = 16;
  CHexMap HexMap[HexMapL] =
  {
    {'0', 0}, {'1', 1},
    {'2', 2}, {'3', 3},
    {'4', 4}, {'5', 5},
    {'6', 6}, {'7', 7},
    {'8', 8}, {'9', 9},
    {'A', 10}, {'B', 11},
    {'C', 12}, {'D', 13},
    {'E', 14}, {'F', 15}
  };
  TCHAR *mstr = _tcsupr(_tcsdup(value));
  TCHAR *s = mstr;
  int result = 0;
  if (*s == '0' && *(s + 1) == 'X') s += 2;
  bool firsttime = true;
  while (*s != '\0')
  {
    bool found = false;
    for (int i = 0; i < HexMapL; i++)
    {
      if (*s == HexMap[i].chr)
      {
        if (!firsttime) result <<= 4;
        result |= HexMap[i].value;
        found = true;
        break;
      }
    }
    if (!found) break;
    s++;
    firsttime = false;
  }
  free(mstr);
  return result;
}

/*
 * Creates a string from the given Unicode character code
 */
CUniString CExtraString::CharCodeToString(int charCode)
{
	wchar_t * wbuffer = NULL;

	if (charCode <= 0xFFFF)
	{
		wbuffer = new wchar_t[2];
		wbuffer[0] = charCode;
		wbuffer[1] = 0;
	}
	else //Use surrogates (see https://en.wikipedia.org/wiki/UTF-16#Code_points_U.2B10000..U.2B10FFFF)
	{
		charCode -= 0x010000;
		wbuffer = new wchar_t[3];
		int highSurrogate = (charCode >> 10) + 0xD800;
		int lowSurrogate = (charCode & 0x3FF) + 0xDC00;
		wbuffer[0] = highSurrogate;
		wbuffer[1] = lowSurrogate;
		wbuffer[2] = 0;
	}
	CUniString character(wbuffer);
	delete[] wbuffer;
	return character;
}


/*
 * Class CUniString
 *
 * Unicode based string class.
 * Works only if UNICODE is defined!
 *
 * CC 17.02.2010 - created
 */

CUniString::CUniString() : CString()
{
	Init();
}

/*
 * Copy Constructor
 */
CUniString::CUniString(const CUniString & str) : CString(str)
{
	Init();
}

CUniString::CUniString(CString & str) : CString(str)
{
	Init();
}

CUniString::CUniString(string stlString) : CString(stlString.c_str())
{
	Init();
}

CUniString::CUniString(unsigned char * charStr) 
				: CString((unsigned char *)charStr)
{
	Init();
}

CUniString::CUniString(const char * charArray) : CString(charArray)
{
	Init();
}

CUniString::CUniString(unsigned char * charStr, bool decodeUtf8) 
	: CString(decodeUtf8 ? CUniString::DecodeUtf8(charStr) : CString((char*)charStr))
{
	Init();
}

CUniString::CUniString(wchar_t * wcharArray) : CString(wcharArray)
{
	Init();
}

CUniString::CUniString(const wchar_t * wcharArray) : CString(wcharArray)
{
	Init();
}

CUniString::CUniString(CUniString * str) : CString(str->GetBuffer())
{
	Init();
}

/*
 * Assignment operator
 */
CUniString & CUniString::operator=(const CUniString & other)
{
	this->SetString(other);
	return *this;
}

/*
 * Destructor
 */
CUniString::~CUniString()
{
	delete [] m_CharBuffer;
	delete m_CriticalSection;
}

/*
 * Initialises all fields
 */
void CUniString::Init()
{
	m_CharBuffer = NULL;
	m_CriticalSection = NULL;
}

/*
 * Returns a const char array of this string in UTF-8 encoding.
 * Also converts the line endings to linux style!
 */
const char * CUniString::ToUtf8C_Str()
{
	CSingleLock lock(GetCriticalSection(), TRUE);
	#ifdef UNICODE
		CExtraString::ConvertLineEndings(*this);
		LPCWSTR unicode = this->GetBuffer();
		delete [] m_CharBuffer;
		m_CharBuffer = NULL;
		int bufferSize = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, m_CharBuffer, 0, NULL, NULL);
		m_CharBuffer = new char[bufferSize+1];
		WideCharToMultiByte(CP_UTF8, 0, unicode, -1, m_CharBuffer, bufferSize, NULL, NULL);
		const char * ret = (const char *)m_CharBuffer;
		return ret;
	#else
		return NULL;
	#endif
}

/*
 * Decodes the given character array to unicode using UTF-8.
 */
CUniString CUniString::DecodeUtf8(unsigned char * charArray)
{
	wchar_t * unicode = NULL;
	int bufferSize = MultiByteToWideChar(CP_UTF8, 0, (char *)charArray, -1, unicode, 0);
	unicode = new wchar_t[bufferSize+1];
	MultiByteToWideChar(CP_UTF8, 0, (char *)charArray, -1, unicode, bufferSize);
	CUniString ret(unicode);
	delete [] unicode;
	CExtraString::XMLDecode(ret);
	return ret;
}

/*
 * Decodes the content of this CUniString using UTF-8 and replaces the content with the decoded string.
 */
void CUniString::DecodeUtf8()
{
	unsigned char * charArray = (unsigned char *)this->ToC_Str();
	this->SetString(DecodeUtf8(charArray));
}

/*
 * Converts the content of this CUniString to a standard character array
 */
const char * CUniString::ToC_Str()
{
	CSingleLock lock(GetCriticalSection(), TRUE);
	#ifdef UNICODE
		USES_CONVERSION;
		const char * pszNonUnicode = W2A( this->LockBuffer( ) );
		this->UnlockBuffer( );
		delete [] m_CharBuffer;
		m_CharBuffer = new char[this->GetLength()+1];
		strcpy_s(m_CharBuffer, this->GetLength()+1, pszNonUnicode);
		const char * ret = (const char *)m_CharBuffer;
		return ret;
	#else
		return NULL;
	#endif
}

/*
 * Parses the content of this CUniString to a double floating point number
 */
double CUniString::ToDouble()
{
	return _tstof(this->GetBuffer());
}

/*
 * Parses the content of this CUniString to an integer number
 */
int CUniString::ToInt()
{
	return _tstoi(this->GetBuffer());
}

/*
 * Appends the given string
 */
void CUniString::Append(CUniString & str)
{
	CString::Append(str);
}

void CUniString::Append(int number)
{
	CString str;
	str.Format(_T("%d"), number);
	CString::Append(str);
}

void CUniString::Append(__int64 number)
{
	CString str;
	str.Format(_T("%d"), number);
	CString::Append(str);
}

void CUniString::Append(const char * charArray)
{
	CString str(charArray);
	CString::Append(str);
}

void CUniString::Append(const wchar_t * wcharArray)
{
	#ifdef UNICODE
		CString::Append(wcharArray);
	#endif
}

/*
 * Appends double value (formatted).
 * 'decimalDigits' - Number of decimal digits (default is 1)
 */
void CUniString::Append(double number, int decimalDigits /* = 1 */)
{
	if (decimalDigits<0)
		decimalDigits = 0;

	CUniString formatString(_T("%."));
	formatString.Append(decimalDigits);
	formatString.Append(_T("f"));

	CString temp;
	temp.Format(formatString.GetBuffer(), number);
	CString::Append(temp);
}

void CUniString::Append(wchar_t c) {
	CString::AppendChar(c);
}

/*
 * Clears the content, making this an empty string
 */
void CUniString::Clear()
{
	CString::SetString(_T(""));
}

/*
 * Returns the first position of the given string within this CUniString.
 * Returns -1 if not found.
 */
int CUniString::Find(const char * charArray)
{
	return CString::Find(CUniString(charArray));
}

/*
 * Returns the first position of the given string within this CUniString.
 * Returns -1 if not found.
 */
int CUniString::Find(const wchar_t * wcharArray)
{
	#ifdef UNICODE
		return CString::Find(wcharArray);
	#else
		return -1;
	#endif
}

/*
 * Returns the first position of the given string within this CUniString.
 * Returns -1 if not found.
 */
int CUniString::Find(CUniString & str)
{
	return CString::Find(str);
}

/*
 * Retunrs the position of the last occurance of the specified string or -1;
 */
int CUniString::FindLast(CUniString & str)
{
	CSingleLock lock(GetCriticalSection(), TRUE);
	CUniString temp = CUniString(this->GetBuffer());
	int ret = 0;
	int pos;
	int lastPos = -1;
	bool isFirst = true;
	while ((pos = temp.Find(str)) >= 0)
	{
		lastPos = pos;
		ret += pos;
		if (isFirst)
			isFirst = false;
		else
			ret++; //Corrects the +1 of the next line
		temp = temp.Right(temp.GetLength()-(pos+1));
	}
	if (lastPos < 0)
		ret = -1;
	return ret;
}

/*
 * Normal CString::GetBuffer() enriched with thread synchronization
 */
ATL::CSimpleStringT<TCHAR,false>::PXSTR CUniString::GetBuffer()
{
	CSingleLock lock(GetCriticalSection(), TRUE);
	return CString::GetBuffer();
}

/*
 * Returns a critical section object for thread synchronization
 */
CCriticalSection * CUniString::GetCriticalSection()
{
	if (m_CriticalSection == NULL)
		m_CriticalSection = new CCriticalSection();
	return m_CriticalSection;
}

/*
 * Checks if this CUniString ends with the given string
 */
bool CUniString::EndsWith(CUniString str)
{
	if (GetLength() < str.GetLength())
		return false;
	return FindLast(str) == GetLength()-str.GetLength();
}

/*
 * Splits this glyph using the given split token.
 * 'splitBy' - Split token (e.g. a space or line break)
 * 'result' (out) - The vector where the split segments are stored.
 */
void CUniString::Split(CUniString splitBy, vector<CUniString> & result)
{
	CUniString str = CUniString(this);
	int pos = 0;
	while ((pos = str.Find(splitBy)) >=0)
	{
		CUniString segment = str.Left(pos);
		result.push_back(segment);
		str = str.Right(str.GetLength()-splitBy.GetLength()-segment.GetLength());
	}
	//Put the last segment
	result.push_back(str);
}


} //end namespace