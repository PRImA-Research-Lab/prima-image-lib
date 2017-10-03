#pragma once

#include "stdafx.h"
#include <msxml6.h>
#include "ExtraString.h"
#include "MsXmlNode.h"

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif

namespace PRImA 
{

#ifndef MSXMLWRITER_H
#define MSXMLWRITER_H

/*
 * Class CMsXmlWriter
 *
 * Helper class to create and save XML files using MSXML.
 *
 * CC 29.03.2010 - created
 */

class DllExport CMsXmlWriter
{
public:
	CMsXmlWriter(CUniString & fileName);
	~CMsXmlWriter(void);

	IXMLDOMDocument2	*	CreateXmlDoc();
	CMsXmlNode			*	CreateRoot(const wchar_t * nodeName);
	CMsXmlNode			*	CreateRoot(const wchar_t * nodeName, const wchar_t * xmlns,
										const wchar_t * xmlnsXsi, const wchar_t * schemaLocation,
										const wchar_t * xlinkNameSpace = NULL);

	bool					SaveDocument();

private:
	HRESULT CreateAndInitDOM(IXMLDOMDocument2 **ppDoc);
	HRESULT CreateAndAddPINode(IXMLDOMDocument2 *pDom, PCWSTR wszTarget, PCWSTR wszData);
	HRESULT VariantFromString(PCWSTR wszValue, VARIANT &Variant);
	HRESULT AppendChildToParent(IXMLDOMNode *pChild, IXMLDOMNode *pParent);
	HRESULT CreateElement(IXMLDOMDocument2 *pXMLDom, PCWSTR wszName, 
							IXMLDOMElement **ppElement);
	HRESULT CreateElement(IXMLDOMDocument2 *pXMLDom, PCWSTR wszName, PCWSTR wszNs, 
							IXMLDOMElement **ppElement);

private:
	CUniString			m_FileName;
	IXMLDOMDocument2	  * m_XmlDoc;
	CMsXmlNode		  * m_Root;
};

#else
class CMsXmlWriter;
#endif

}