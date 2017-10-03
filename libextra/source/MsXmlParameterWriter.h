#pragma once
#include "parameter.h"
#include "MsXmlWriter.h"
#include "MsXmlNode.h"
#include "msxmlparameterreader.h"

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
 * Class CMsXmlParameterWriter
 *
 * MS XML implementation of CParameterWriter.
 *
 * CC 06.07.2010 - created
 */

class DllExport CMsXmlParameterWriter : public CParameterWriter
{
public:
	CMsXmlParameterWriter(CMsXmlNode * parentNode = NULL);
	~CMsXmlParameterWriter(void);

	void WriteParameters(CParameterMap * params, CUniString fileName);
	void AppendParameters(CParameterMap * params);

private:
	void WriteParameterNode(CParameter * param, CMsXmlNode * paramsNode);
	void WriteValidValues(CStringParameter * param, CMsXmlNode * paramNode);

private:
	CMsXmlNode * m_ParentNode;		//Parent node for Append
};

}