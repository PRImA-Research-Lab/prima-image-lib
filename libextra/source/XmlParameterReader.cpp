#include "XmlParameterReader.h"

using namespace PRImA;

/*
 * Class CParameterReader
 *
 * XML implementation of CParameterReader.
 *
 * CC 04.03.2010 - created
 */

const char * CXmlParameterReader::ELEMENT_Parameters	= "Parameters";
const char * CXmlParameterReader::ELEMENT_Parameter		= "Parameter";
const char * CXmlParameterReader::ELEMENT_Description	= "Description";
const char * CXmlParameterReader::ELEMENT_ValidValues	= "ValidValues";
const char * CXmlParameterReader::ELEMENT_Value			= "Value";

const char * CXmlParameterReader::ATTR_id			= "id";
const char * CXmlParameterReader::ATTR_type			= "type";
const char * CXmlParameterReader::ATTR_name			= "name";
const char * CXmlParameterReader::ATTR_sortIndex	= "sortIndex";
const char * CXmlParameterReader::ATTR_readOnly		= "readOnly";
const char * CXmlParameterReader::ATTR_isSet		= "isSet";
const char * CXmlParameterReader::ATTR_version		= "version";
const char * CXmlParameterReader::ATTR_visible		= "visible";
const char * CXmlParameterReader::ATTR_value		= "value";
const char * CXmlParameterReader::ATTR_min			= "min";
const char * CXmlParameterReader::ATTR_max			= "max";
const char * CXmlParameterReader::ATTR_step			= "step";
const char * CXmlParameterReader::ATTR_textType		= "textType";


CXmlParameterReader::CXmlParameterReader(void)
{
}

CXmlParameterReader::~CXmlParameterReader(void)
{
}

/*
 * Reads the parameters from the specified XML file and returns a parameter map.
 */
CParameterMap * CXmlParameterReader::ReadParameters(const char* fileName)
{
	xmlDocPtr doc = xmlReadFile(fileName, "UTF-8", XML_PARSE_RECOVER);
	xmlNode * baseElement;
	xmlNode * tempNode;

	if(doc == NULL)
	{
		fprintf(stderr, "Failed To Load XML File\n");
		return NULL;
	}

	baseElement = xmlDocGetRootElement(doc);

	if(baseElement == NULL)
	{
		xmlFreeDoc(doc);
		return NULL;
	}

	CParameterMap * params = new CParameterMap();

	//Process nodes
	for(tempNode = baseElement->children; tempNode != NULL; tempNode = tempNode->next)
	{
		if(tempNode->type != XML_ELEMENT_NODE)
			continue;

		if(strcmp((char *) tempNode->name, ELEMENT_Parameter) == 0)
			ParseParameter(params, tempNode);
	}

    xmlFreeDoc(doc);
	return params;
}

/*
 * Parses a Parameter XML element and inserts the parameter into the given map.
 */
void CXmlParameterReader::ParseParameter(CParameterMap * params, xmlNode* parameterNode)
{
	char * tempString;
	xmlNode * tempNode, * textNode = NULL;

	CParameter * param = NULL;

	//Type
	if((tempString = (char *) xmlGetProp(parameterNode, (xmlChar *) ATTR_type)) != NULL)
	{
		int type = atoi(tempString);
		xmlFree(tempString);
		if (type == CParameter::TYPE_BOOL)
			param = new CBoolParameter();
		else if (type == CParameter::TYPE_DOUBLE)
			param = new CDoubleParameter();
		else if (type == CParameter::TYPE_INT)
			param = new CIntParameter();
		else if (type == CParameter::TYPE_STRING)
			param = new CStringParameter();
		else
			return; //Unknown or not supported type
	}
	else
		return; //Type is mandatory

	//ID
	if((tempString = (char *) xmlGetProp(parameterNode, (xmlChar *) ATTR_id)) != NULL)
	{
		param->SetId(atoi(tempString));
		xmlFree(tempString);
	}
	//Version
	if((tempString = (char *) xmlGetProp(parameterNode, (xmlChar *) ATTR_version)) != NULL)
	{
		param->SetVersion(atoi(tempString));
		//TODO merging
		xmlFree(tempString);
	}
	//Sort Index
	if((tempString = (char *) xmlGetProp(parameterNode, (xmlChar *) ATTR_sortIndex)) != NULL)
	{
		param->SetSortIndex(atoi(tempString));
		xmlFree(tempString);
	}
	//Name
	if((tempString = (char *) xmlGetProp(parameterNode, (xmlChar *) ATTR_name)) != NULL)
	{
		param->SetName(CUniString(tempString));
		xmlFree(tempString);
	}
	//ReadOnly
	if((tempString = (char *) xmlGetProp(parameterNode, (xmlChar *) ATTR_readOnly)) != NULL)
	{
		param->SetReadOnly(strcmp((char *) tempString, "true") == 0);
		xmlFree(tempString);
	}
	//Visible
	if((tempString = (char *) xmlGetProp(parameterNode, (xmlChar *) ATTR_visible)) != NULL)
	{
		param->SetVisible(strcmp((char *) tempString, "true") == 0);
		xmlFree(tempString);
	}

	//Min, Max, Step for Int Params
	if (param->GetType() == CParameter::TYPE_INT)
	{
		CIntParameter * intParam = (CIntParameter*)param;
		//Min
		if((tempString = (char *) xmlGetProp(parameterNode, (xmlChar *) ATTR_min)) != NULL)
		{
			intParam->SetMin(atoi(tempString));
			xmlFree(tempString);
		}
		//Max
		if((tempString = (char *) xmlGetProp(parameterNode, (xmlChar *) ATTR_max)) != NULL)
		{
			intParam->SetMax(atoi(tempString));
			xmlFree(tempString);
		}
		//Step
		if((tempString = (char *) xmlGetProp(parameterNode, (xmlChar *) ATTR_step)) != NULL)
		{
			intParam->SetStep(atoi(tempString));
			xmlFree(tempString);
		}
	}

	//Min, Max, Step for Double Params
	if (param->GetType() == CParameter::TYPE_DOUBLE)
	{
		CDoubleParameter * dblParam = (CDoubleParameter*)param;
		//Min
		if((tempString = (char *) xmlGetProp(parameterNode, (xmlChar *) ATTR_min)) != NULL)
		{
			dblParam->SetMin(atof(tempString));
			xmlFree(tempString);
		}
		//Max
		if((tempString = (char *) xmlGetProp(parameterNode, (xmlChar *) ATTR_max)) != NULL)
		{
			dblParam->SetMax(atof(tempString));
			xmlFree(tempString);
		}
		//Step
		if((tempString = (char *) xmlGetProp(parameterNode, (xmlChar *) ATTR_step)) != NULL)
		{
			dblParam->SetStep(atof(tempString));
			xmlFree(tempString);
		}
	}

	//Text Type
	if (param->GetType() == CParameter::TYPE_STRING)
	{
		CStringParameter * stringParam = (CStringParameter*)param;
		if((tempString = (char *) xmlGetProp(parameterNode, (xmlChar *) ATTR_textType)) != NULL)
		{
			stringParam->SetTextType(atoi(tempString));
			xmlFree(tempString);
		}
	}

	//Value
	if((tempString = (char *) xmlGetProp(parameterNode, (xmlChar *) ATTR_value)) != NULL)
	{
		if (param->GetType() == CParameter::TYPE_BOOL)
			((CBoolParameter*)param)->SetValue(strcmp((char *) tempString, "true") == 0);
		else if (param->GetType() == CParameter::TYPE_INT)
			((CIntParameter*)param)->SetValue(atoi(tempString));
		else if (param->GetType() == CParameter::TYPE_DOUBLE)
			((CDoubleParameter*)param)->SetValue(atof(tempString));
		else if (param->GetType() == CParameter::TYPE_STRING)
			((CStringParameter*)param)->SetValue(CUniString(tempString));
		xmlFree(tempString);
	}

	//Is Set
	if((tempString = (char *) xmlGetProp(parameterNode, (xmlChar *) ATTR_isSet)) != NULL)
	{
		param->SetSet(strcmp((char *) tempString, "true") == 0);
		xmlFree(tempString);
	}

	//Description
	for(tempNode = parameterNode->children; tempNode != NULL; tempNode = tempNode->next)
    {
        if(tempNode->type == XML_ELEMENT_NODE && strcmp((char *) tempNode->name, ELEMENT_Description) == 0)
		{
			textNode = tempNode;
			if(tempNode->children != NULL)
				param->SetDescription(ConvertText(tempNode->children->content)); 
			break;
		}
	}

	//Valid Values
	if (param->GetType() == CParameter::TYPE_STRING)
	{
		for(tempNode = parameterNode->children; tempNode != NULL; tempNode = tempNode->next)
		{
			if(tempNode->type == XML_ELEMENT_NODE && strcmp((char *) tempNode->name, ELEMENT_ValidValues) == 0)
			{
				ParseValidValues((CStringParameter*)param, tempNode);
			}
		}
	}

	params->Add(param);
}

/*
 * Parses a valid values node (used for string parameters).
 */
void CXmlParameterReader::ParseValidValues(CStringParameter * param, xmlNode* valuesNode)
{
	char * tempString;
	xmlNode * tempNode;
	set<CUniString> * validValues = param->GetValidValues();

	for(tempNode = valuesNode->children; tempNode != NULL; tempNode = tempNode->next)
	{
		if(tempNode->type == XML_ELEMENT_NODE && strcmp((char *) tempNode->name, ELEMENT_Value) == 0)
		{
			if((tempString = (char *) xmlGetProp(tempNode, (xmlChar *) ATTR_value)) != NULL)
			{
				validValues->insert(CUniString(tempString));
				xmlFree(tempString);
			}
		}
	}
}

CUniString CXmlParameterReader::ConvertText(xmlChar * text)
{
	CUniString ret = CUniString(text, true);
	CExtraString::ConvertLineEndings(ret);
	return ret;
}