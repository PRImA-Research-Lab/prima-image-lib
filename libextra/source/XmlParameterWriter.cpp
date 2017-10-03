#include "XmlParameterWriter.h"

using namespace PRImA;

/*
 * Class CParameterReader
 *
 * XML implementation of CParameterWriter.
 *
 * CC 04.03.2010 - created
 */

CXmlParameterWriter::CXmlParameterWriter(void)
{
}

CXmlParameterWriter::~CXmlParameterWriter(void)
{
}

void CXmlParameterWriter::WriteParameters(CParameterMap * params, const char * fileName)
{
	if (params == NULL)
		return;

	int i;
	xmlNode * baseElement;

	xmlDocPtr xmlDoc = xmlNewDoc((xmlChar *) "1.0");
	xmlDoc->encoding = (xmlChar *)"UTF-8";
	
	//Root node
	baseElement = xmlNewNode(NULL, (xmlChar *) CXmlParameterReader::ELEMENT_Parameters);

	xmlAddChild((xmlNode *) xmlDoc, (xmlNode *) baseElement);

	//Parameters
	for(i = 0; i < params->GetSize(); i++)
	{
		WriteParameterNode(params->Get(i), baseElement);
	}

	// Save XML File With Formatting
	xmlSaveFormatFileEnc(fileName, xmlDoc, "UTF-8", 1);
	xmlFree(xmlDoc);
}

void CXmlParameterWriter::WriteParameterNode(CParameter * param, xmlNode * paramsNode)
{
	xmlNode * paramNode = xmlNewNode(NULL, (xmlChar *) CXmlParameterReader::ELEMENT_Parameter);
	char * buf = new char[512];

	//Type
	sprintf(buf, "%d", param->GetType());
	xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_type, (xmlChar *) buf);

	//ID
	sprintf(buf, "%d", param->GetId());
	xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_id, (xmlChar *) buf);

	//Sort Index
	sprintf(buf, "%d", param->GetSortIndex());
	xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_sortIndex, (xmlChar *) buf);

	//Version
	sprintf(buf, "%d", param->GetVersion());
	xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_version, (xmlChar *) buf);

	//Name
	xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_name, 
				(const xmlChar*)param->GetName().ToUtf8C_Str());

	//Read Only
	xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_readOnly, 
					(const xmlChar*)(param->IsReadOnly() ? "true" : "false"));

	//Is set
	xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_isSet, 
					(const xmlChar*)(param->IsSet() ? "true" : "false"));

	//Visible
	xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_visible, 
					(const xmlChar*)(param->IsVisible() ? "true" : "false"));

	//Value
	if (param->IsSet())
	{
		if (param->GetType() == CParameter::TYPE_BOOL)
		{
			xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_value, 
						(const xmlChar*)(((CBoolParameter*)param)->GetValue() ? "true" : "false"));
		}
		else if (param->GetType() == CParameter::TYPE_INT)
		{
			sprintf(buf, "%d", ((CIntParameter*)param)->GetValue());
			xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_value, (xmlChar *) buf);
		}
		else if (param->GetType() == CParameter::TYPE_DOUBLE)
		{
			sprintf(buf, "%.5f", ((CDoubleParameter*)param)->GetValue());
			xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_value, (xmlChar *) buf);
		}
		else if (param->GetType() == CParameter::TYPE_STRING)
		{
			xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_value, 
				(xmlChar *) ((CStringParameter*)param)->GetValue().ToUtf8C_Str());
		}
	}

	//Min, Max, Step for Int Parameters
	if (param->GetType() == CParameter::TYPE_INT)
	{
		CIntParameter * intParam = (CIntParameter*)param;
		//Min
		sprintf(buf, "%d", intParam->GetMin());
		xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_min, (xmlChar *) buf);
		//Max
		sprintf(buf, "%d", intParam->GetMax());
		xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_max, (xmlChar *) buf);
		//Step
		sprintf(buf, "%d", intParam->GetStep());
		xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_step, (xmlChar *) buf);
	}

	//Min, Max, Step for Double Parameters
	if (param->GetType() == CParameter::TYPE_DOUBLE)
	{
		CDoubleParameter * dblParam = (CDoubleParameter*)param;
		//Min
		sprintf(buf, "%.5f", dblParam->GetMin());
		xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_min, (xmlChar *) buf);
		//Max
		sprintf(buf, "%.5f", dblParam->GetMax());
		xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_max, (xmlChar *) buf);
		//Step
		sprintf(buf, "%.5f", dblParam->GetStep());
		xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_step, (xmlChar *) buf);
	}

	//Text Type
	if (param->GetType() == CParameter::TYPE_STRING)
	{
		CStringParameter * stringParam = (CStringParameter*)param;
		sprintf(buf, "%d", stringParam->GetTextType());
		xmlNewProp(paramNode, (xmlChar *) CXmlParameterReader::ATTR_textType, (xmlChar *) buf);
	}
	//Description
	xmlNode * textNode = xmlNewTextChild(	paramNode, NULL, 
											(xmlChar *) CXmlParameterReader::ELEMENT_Description, 
											(xmlChar *) param->GetDescription().ToUtf8C_Str());

	//Valid Values
	if (param->GetType() == CParameter::TYPE_STRING)
	{
		WriteValidValues((CStringParameter*)param, paramNode);
	}

	delete [] buf;

	xmlAddChild(paramsNode, paramNode);
}

void CXmlParameterWriter::WriteValidValues(CStringParameter * param, xmlNode * paramNode)
{
	set<CUniString> * validValues = param->GetValidValues();
	set<CUniString>::iterator it = validValues->begin();

	if (!validValues->empty())
	{
		xmlNode * valuesNode = xmlNewNode(NULL, (xmlChar *) CXmlParameterReader::ELEMENT_ValidValues);

		while (it != validValues->end())
		{
			xmlNode * valueNode = xmlNewNode(NULL, (xmlChar *) CXmlParameterReader::ELEMENT_Value);
			char * buf = new char[512];

			//Type
			xmlNewProp(valueNode, (xmlChar *) CXmlParameterReader::ATTR_value, 
						(const xmlChar*)(*it).ToUtf8C_Str());
			xmlAddChild(valuesNode, valueNode);

			it++;
		}
		xmlAddChild(paramNode, valuesNode);
	}
}