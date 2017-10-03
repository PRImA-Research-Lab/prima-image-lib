#include "MsXmlParameterReader.h"

using namespace PRImA;

/*
 * Class CMsXmlParameterReader
 *
 * MS XML implementation of CParameterReader.
 *
 * CC 06.07.2010 - created
 */

const wchar_t * CMsXmlParameterReader::ELEMENT_Parameters	= _T("Parameters");
const wchar_t * CMsXmlParameterReader::ELEMENT_Parameter	= _T("Parameter");
const wchar_t * CMsXmlParameterReader::ELEMENT_Description	= _T("Description");
const wchar_t * CMsXmlParameterReader::ELEMENT_ValidValues	= _T("ValidValues");
const wchar_t * CMsXmlParameterReader::ELEMENT_Value		= _T("Value");

const wchar_t * CMsXmlParameterReader::ATTR_id			= _T("id");
const wchar_t * CMsXmlParameterReader::ATTR_type		= _T("type");
const wchar_t * CMsXmlParameterReader::ATTR_name		= _T("name");
const wchar_t * CMsXmlParameterReader::ATTR_sortIndex	= _T("sortIndex");
const wchar_t * CMsXmlParameterReader::ATTR_readOnly	= _T("readOnly");
const wchar_t * CMsXmlParameterReader::ATTR_isSet		= _T("isSet");
const wchar_t * CMsXmlParameterReader::ATTR_version		= _T("version");
const wchar_t * CMsXmlParameterReader::ATTR_visible		= _T("visible");
const wchar_t * CMsXmlParameterReader::ATTR_value		= _T("value");
const wchar_t * CMsXmlParameterReader::ATTR_min			= _T("min");
const wchar_t * CMsXmlParameterReader::ATTR_max			= _T("max");
const wchar_t * CMsXmlParameterReader::ATTR_step		= _T("step");
const wchar_t * CMsXmlParameterReader::ATTR_textType	= _T("textType");


/*
 * Constructor
 *
 * 'parentNode' - Parent XML node wherefrom to read parameters (for ReadParameters())
 *                If this parameter is NULL, only the ReadParameters(file) can be used.
 */
CMsXmlParameterReader::CMsXmlParameterReader(CMsXmlNode * parentNode /*= NULL*/)
{
	m_ParentNode = parentNode;
}

/*
 * Destructor
 */
CMsXmlParameterReader::~CMsXmlParameterReader(void)
{
}

/*
 * Reads the parameters from the specified parameter XML file.
 */
CParameterMap * CMsXmlParameterReader::ReadParameters(CUniString fileName)
{
	CMsXmlReader * msXmlReader = new CMsXmlReader(fileName);

	IXMLDOMDocument2 * xmlDoc = msXmlReader->GetXmlDoc();
	if (xmlDoc == NULL)
	{
		delete msXmlReader;
		return NULL;
	}

	//Read the content
	CMsXmlNode * baseElement = new CMsXmlNode();
	CUniString value;

	IXMLDOMElement * element;
	if (xmlDoc->get_documentElement(&element) != S_OK)
	{
		delete msXmlReader;
		return NULL;
	}
	baseElement->SetNodeObject(element, xmlDoc);

	if (baseElement->GetName() != CUniString(ELEMENT_Parameters)) //Not a parameter XML file
	{
		delete msXmlReader;
		return NULL;
	}

	CParameterMap * params = new CParameterMap();

	//Map name
	if(baseElement->HasAttribute(ATTR_name))
	{
		params->SetName(baseElement->GetAttribute(ATTR_name));
	}


	//Traverse children of base element
	CMsXmlNode * tempNode = baseElement->GetFirstChild();
	while (tempNode != NULL)
	{
		if(tempNode->GetName() == CUniString(ELEMENT_Parameter))
			ParseParameter(params, tempNode);
		tempNode = tempNode->GetNextSibling();
	}
	
	delete baseElement;
	delete msXmlReader;
	return params;
}

/*
 * Read the parameters from the specified parent node (see constructor).
 */
CParameterMap * CMsXmlParameterReader::ReadParameters()
{
	if (m_ParentNode == NULL)
		return NULL;

	CParameterMap * params = new CParameterMap();

	//Traverse children of base element
	CMsXmlNode * tempNode = m_ParentNode->GetFirstChild();
	while (tempNode != NULL)
	{
		if(tempNode->GetName() == CUniString(ELEMENT_Parameter))
			ParseParameter(params, tempNode);
		tempNode = tempNode->GetNextSibling();
	}

	return params;
}

/*
 * Parses a Parameter XML element and inserts the parameter into the given map.
 */
void CMsXmlParameterReader::ParseParameter(CParameterMap * params, CMsXmlNode * parameterNode)
{
	CMsXmlNode * tempNode, * textNode = NULL;

	CParameter * param = NULL;

	//Type
	if (parameterNode->HasAttribute(ATTR_type))
	{
		int type = parameterNode->GetIntAttribute(ATTR_type);
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
	if(parameterNode->HasAttribute(ATTR_id))
	{
		param->SetId(parameterNode->GetIntAttribute(ATTR_id));
	}
	//Version
	if(parameterNode->HasAttribute(ATTR_version))
	{
		param->SetVersion(parameterNode->GetIntAttribute(ATTR_version));
		//TODO merging
	}
	//Sort Index
	if(parameterNode->HasAttribute(ATTR_sortIndex))
	{
		param->SetSortIndex(parameterNode->GetIntAttribute(ATTR_sortIndex));
	}
	//Name
	if(parameterNode->HasAttribute(ATTR_name))
	{
		param->SetName(parameterNode->GetAttribute(ATTR_name));
	}
	//ReadOnly
	if(parameterNode->HasAttribute(ATTR_readOnly))
	{
		param->SetReadOnly(parameterNode->GetBoolAttribute(ATTR_readOnly));
	}
	//Visible
	if(parameterNode->HasAttribute(ATTR_visible))
	{
		param->SetVisible(parameterNode->GetBoolAttribute(ATTR_visible));
	}

	//Min, Max, Step for Int Params
	if (param->GetType() == CParameter::TYPE_INT)
	{
		CIntParameter * intParam = (CIntParameter*)param;
		//Min
		if(parameterNode->HasAttribute(ATTR_min))
		{
			intParam->SetMin(parameterNode->GetIntAttribute(ATTR_min));
		}
		//Max
		if(parameterNode->HasAttribute(ATTR_max))
		{
			intParam->SetMax(parameterNode->GetIntAttribute(ATTR_max));
		}
		//Step
		if(parameterNode->HasAttribute(ATTR_step))
		{
			intParam->SetStep(parameterNode->GetIntAttribute(ATTR_step));
		}
	}

	//Min, Max, Step for Double Params
	if (param->GetType() == CParameter::TYPE_DOUBLE)
	{
		CDoubleParameter * dblParam = (CDoubleParameter*)param;
		//Min
		if(parameterNode->HasAttribute(ATTR_min))
		{
			dblParam->SetMin(parameterNode->GetDoubleAttribute(ATTR_min));
		}
		//Max
		if(parameterNode->HasAttribute(ATTR_max))
		{
			dblParam->SetMax(parameterNode->GetDoubleAttribute(ATTR_max));
		}
		//Step
		if(parameterNode->HasAttribute(ATTR_step))
		{
			dblParam->SetStep(parameterNode->GetDoubleAttribute(ATTR_step));
		}
	}

	//Text Type
	if (param->GetType() == CParameter::TYPE_STRING)
	{
		CStringParameter * stringParam = (CStringParameter*)param;
		if(parameterNode->HasAttribute(ATTR_textType))
		{
			stringParam->SetTextType(parameterNode->GetIntAttribute(ATTR_textType));
		}
	}

	//Value
	if(parameterNode->HasAttribute(ATTR_value))
	{
		if (param->GetType() == CParameter::TYPE_BOOL)
			((CBoolParameter*)param)->SetValue(parameterNode->GetBoolAttribute(ATTR_value));
		else if (param->GetType() == CParameter::TYPE_INT)
			((CIntParameter*)param)->SetValue(parameterNode->GetIntAttribute(ATTR_value));
		else if (param->GetType() == CParameter::TYPE_DOUBLE)
			((CDoubleParameter*)param)->SetValue(parameterNode->GetDoubleAttribute(ATTR_value));
		else if (param->GetType() == CParameter::TYPE_STRING)
			((CStringParameter*)param)->SetValue(parameterNode->GetAttribute(ATTR_value));
	}

	//Is Set
	if(parameterNode->HasAttribute(ATTR_isSet))
	{
		param->SetSet(parameterNode->GetBoolAttribute(ATTR_isSet));
	}

	//Description
	tempNode = parameterNode->GetFirstChild();
	while (tempNode != NULL)
	{
		if(tempNode->GetName() == CUniString(ELEMENT_Description))
		{
			if(tempNode->HasChildren())
				param->SetDescription(ConvertText(tempNode->GetTextContent()));
		}
		tempNode = tempNode->GetNextSibling();
	}

	//Valid Values
	if (param->GetType() == CParameter::TYPE_STRING)
	{
		tempNode = parameterNode->GetFirstChild();
		while (tempNode != NULL)
		{
			if(tempNode->GetName() == CUniString(ELEMENT_ValidValues))
				ParseValidValues((CStringParameter*)param, tempNode);
			tempNode = tempNode->GetNextSibling();
		}
	}

	params->Add(param);
}

/*
 * Parses a valid values node (used for string parameters).
 */
void CMsXmlParameterReader::ParseValidValues(CStringParameter * param, CMsXmlNode * valuesNode)
{
	CMsXmlNode * tempNode;
	set<CUniString> * validValues = param->GetValidValues();

	tempNode = valuesNode->GetFirstChild();
	while (tempNode != NULL)
	{
		if(tempNode->GetName() == CUniString(ELEMENT_Value))
		{
			if(tempNode->HasAttribute(ATTR_value))
				validValues->insert(tempNode->GetAttribute(ATTR_value));
		}
		tempNode = tempNode->GetNextSibling();
	}
}

/*
 * Decodes UTF-8 text and converts the line breaks to Windows style
 */
CUniString CMsXmlParameterReader::ConvertText(CUniString & text)
{
	text.DecodeUtf8();
	CExtraString::ConvertLineEndings(text);
	return text;
}