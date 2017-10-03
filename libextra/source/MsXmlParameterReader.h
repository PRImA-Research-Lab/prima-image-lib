#pragma once
#include "parameter.h"
#include "MsXmlReader.h"
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

/*
 * Class CMsXmlParameterReader
 *
 * MS XML implementation of CParameterReader.
 *
 * CC 06.07.2010 - created
 */

class DllExport CMsXmlParameterReader :
	public CParameterReader
{
public:
	CMsXmlParameterReader(CMsXmlNode * parentNode = NULL);
	~CMsXmlParameterReader(void);

	CParameterMap * ReadParameters(CUniString fileName);
	CParameterMap * ReadParameters();

public:
	static const wchar_t* ELEMENT_Parameters;
	static const wchar_t* ELEMENT_Parameter;
	static const wchar_t* ELEMENT_Description;
	static const wchar_t* ELEMENT_ValidValues;
	static const wchar_t* ELEMENT_Value;

	static const wchar_t* ATTR_id;
	static const wchar_t* ATTR_type;
	static const wchar_t* ATTR_name;
	static const wchar_t* ATTR_sortIndex;
	static const wchar_t* ATTR_readOnly;
	static const wchar_t* ATTR_isSet;
	static const wchar_t* ATTR_version;
	static const wchar_t* ATTR_visible;
	static const wchar_t* ATTR_value;
	static const wchar_t* ATTR_min;
	static const wchar_t* ATTR_max;
	static const wchar_t* ATTR_step;
	static const wchar_t* ATTR_textType;

private:
	void		ParseParameter(CParameterMap * params, CMsXmlNode * parameterNode);
	void		ParseValidValues(CStringParameter * param, CMsXmlNode * valuesNode);
	CUniString	ConvertText(CUniString & text);


private:
	CMsXmlNode * m_ParentNode;
};

}