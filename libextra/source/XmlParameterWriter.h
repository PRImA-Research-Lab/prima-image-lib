#pragma once
#include "parameter.h"
#include "XmlParameterReader.h"


namespace PRImA 
{

/*
 * Class CXMLParameterWriter
 *
 * XML implementation of CParameterWriter.
 *
 * CC 04.03.2010 - created
 */

class CXmlParameterWriter :	public CParameterWriter
{
public:
	CXmlParameterWriter(void);
	~CXmlParameterWriter(void);

	void WriteParameters(CParameterMap * params, const char * fileName);
	inline void AppendParameters(CParameterMap * params) { /* not implemented */ };

private:
	void WriteParameterNode(CParameter * param, xmlNode * paramsNode);
	void WriteValidValues(CStringParameter * param, xmlNode * paramNode);
};

}