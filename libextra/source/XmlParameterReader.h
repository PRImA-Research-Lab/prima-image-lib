#pragma once
#include "parameter.h"

namespace PRImA 
{

/*
 * Class CXmlParameterReader
 *
 * XML implementation of CParameterReader.
 *
 * CC 04.03.2010 - created
 */

class CXmlParameterReader :	public CParameterReader
{
public:
	CXmlParameterReader(void);
	~CXmlParameterReader(void);

	CParameterMap * ReadParameters(const char* fileName);
	inline CParameterMap * ReadParameters() { return NULL; /* Not implemented */ };

public:
	static const char* ELEMENT_Parameters;
	static const char* ELEMENT_Parameter;
	static const char* ELEMENT_Description;
	static const char* ELEMENT_ValidValues;
	static const char* ELEMENT_Value;

	static const char* ATTR_id;
	static const char* ATTR_type;
	static const char* ATTR_name;
	static const char* ATTR_sortIndex;
	static const char* ATTR_readOnly;
	static const char* ATTR_isSet;
	static const char* ATTR_version;
	static const char* ATTR_visible;
	static const char* ATTR_value;
	static const char* ATTR_min;
	static const char* ATTR_max;
	static const char* ATTR_step;
	static const char* ATTR_textType;

private:
	void		ParseParameter(CParameterMap * params, xmlNode* parameterNode);
	void		ParseValidValues(CStringParameter * param, xmlNode* valuesNode);
	CUniString	ConvertText(xmlChar * text);

};

}