#pragma once
#include "stdafx.h"
#include <map>
#include "ExtraString.h"
#include "Validator.h"
#include "xmlvalidator.h"
#include <msxml6.h>
#include "msxmlreader.h"
#include "msxmlnode.h"

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
 * Class CMsXmlValidator
 *
 * Implementation for the CXmlValidator interface using MSXML.
 * Provides functions to validate an XML document that has been
 * loaded or created using MSXML.
 * 
 * CC 29.03.2010 - created
 */

class DllExport CMsXmlValidator : public CXmlValidator
{
private:
	static const wchar_t* ELEMENT_complexType;
	static const wchar_t* ELEMENT_simpleType;
	static const wchar_t* ELEMENT_attribute;
	static const wchar_t* ELEMENT_restriction;
	static const wchar_t* ELEMENT_enumeration;
	static const wchar_t* ELEMENT_sequence;
	static const wchar_t* ELEMENT_element;
	static const wchar_t* ELEMENT_annotation;
	static const wchar_t* ELEMENT_documentation;
	static const wchar_t* ELEMENT_complexContent;
	static const wchar_t* ELEMENT_extension;
	static const wchar_t* ATTR_name;
	static const wchar_t* ATTR_type;
	static const wchar_t* ATTR_value;

public:
	CMsXmlValidator(const wchar_t * schemaFile, const wchar_t * nameSpace, 
					map<CUniString,CUniString> * docMap = NULL, bool extractTypesAndDocumentation = true);
	~CMsXmlValidator(void);

	int validate(void * doc); //Validates the given XML document (IXMLDOMDocument2)

	CUniString	GetErrorMsg();

	virtual CUniString				GetType(CUniString parentElementType, CUniString objectName);
	virtual map<int, CUniString>  *	GetSimpleTypeValuesByIndex(CUniString typeName);
	virtual map<CUniString, int>  *	GetSimpleTypeValuesByName(CUniString typeName);
	virtual CUniString				GetDocumentation(CUniString & type, CUniString & attr);
	inline map<CUniString, CUniString> * GetDocumentationMap() { return m_Documentation; };

	virtual void		SetErrorMsg(CUniString msg);

private:
	void	FillTypeMap(IXMLDOMDocument2 * xsdDoc);
	HRESULT VariantFromString(PCWSTR wszValue, VARIANT &Variant);
	void ParseAttribute(CMsXmlNode * node, CUniString & typeName, CUniString & attrName);
	void ParseComplexType(CMsXmlNode * node, map<CUniString, CUniString> * nameTypeMap, 
						CUniString & typeName);
	void HandleSequenceAttributeAndComplexContent(CMsXmlNode * node, map<CUniString, CUniString> * nameTypeMap, 
						CUniString & typeName);
	void ParseSimpleType(CMsXmlNode * node, map<int, CUniString> * valueMapByIndex, 
						map<CUniString, int> * valueMapByName);

private:
	IXMLDOMSchemaCollection    * m_SchemaCollection;

	map<CUniString, map<CUniString, CUniString>*>	m_TypeMap;
	map<CUniString, map<int, CUniString>*>			m_SimpleTypesByIndex;
	map<CUniString, map<CUniString, int>*>			m_SimpleTypesByName;
	map<CUniString, CUniString>					*	m_Documentation;  //key: <type name>::<attribute name>

	CUniString		m_ErrorMsg;

	static CCriticalSection	CriticalSect;

};

}