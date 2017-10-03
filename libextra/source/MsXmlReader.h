#pragma once
#include "stdafx.h"
//#include <objbase.h>
#include <msxml6.h>
#include "ExtraString.h"
#include "Crypto.h"

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif

namespace PRImA 
{

/*
 * Class CMsXmlReader
 *
 * Helper class to open and parse XML files using MSXML.
 *
 * CC 29.03.2010 - created
 */

class DllExport CMsXmlReader
{
public:
	CMsXmlReader(CUniString fileName);
	CMsXmlReader(CUniString fileName, CCrypto * crypto);
	~CMsXmlReader(void);

	IXMLDOMDocument2	*	GetXmlDoc(bool passPointerResponsibility = false);
	CUniString				GetParseError();

private:
	HRESULT		VariantFromString(PCWSTR wszValue, VARIANT &Variant);
	HRESULT		CreateAndInitDOM(IXMLDOMDocument2 **ppDoc);

private:
	IXMLDOMDocument2	* m_XmlDoc;
    IXMLDOMParseError	* m_XmlErr;

    BSTR			m_BstrXML;
    BSTR			m_BstrErr;
    VARIANT_BOOL	m_VarStatus;
    VARIANT			m_VarFileName;
 
};


}