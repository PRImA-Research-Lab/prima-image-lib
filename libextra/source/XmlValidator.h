#pragma once
#include "stdafx.h"
#include "validator.h"
#include <map>
#include "ExtraString.h"
#include <afxmt.h>

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
 * Class CXmlValidator
 *
 * Interface for xml validators. Extends the CValidator interface.
 * 
 * CC 29.03.2010 - created
 */

class DllExport CXmlValidator : public CValidator
{
public:
	virtual ~CXmlValidator();

	virtual int validate(void * doc) = 0;

	virtual CUniString				GetType(CUniString parentElementType, CUniString objectName) = 0;
	virtual map<int, CUniString>  *	GetSimpleTypeValuesByIndex(CUniString typeName) = 0;
	virtual map<CUniString, int>  *	GetSimpleTypeValuesByName(CUniString typeName) = 0;
	virtual CUniString				GetDocumentation(CUniString & type, CUniString & attr) = 0;
	virtual map<CUniString, CUniString> * GetDocumentationMap() = 0;

	virtual CUniString	GetErrorMsg() = 0;
	virtual void		SetErrorMsg(CUniString msg) = 0;

};


/*
 * Class CXmlParseException
 *
 * CC
 */
class DllExport CXmlParseException 
{
public:
	CXmlParseException(const char* msg);
	CXmlParseException(CUniString msg);
	~CXmlParseException(void);
	inline CUniString GetMsg() { return m_Msg; };

private:
	CUniString m_Msg;
};


/*
 * Interface CXmlValidatorProvider
 *
 * Provides validators for different schema versions.
 *
 * CC 16.03.2010 - created
 */
class DllExport CXmlValidatorProvider
{
public:
	virtual ~CXmlValidatorProvider();
	virtual CValidator	*	GetValidator(CUniString schemaVersion) = 0;

	virtual CValidator	*	GetValidator(CUniString schemaVersion, CUniString & errMsg) = 0;

	virtual CValidator	*	GetLatestValidator() = 0;

	virtual int				GetVersionNumber(CUniString versionString) = 0;

	virtual CUniString		GetVersionString(int versionNumber) = 0;

	virtual CUniString		GetLatestVersion() = 0;

	inline CUniString		GetLastErrorMessage() { return m_ErrorMessage; };

protected:
	CUniString				m_ErrorMessage;
};

}