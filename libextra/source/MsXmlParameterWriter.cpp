#include "MsXmlParameterWriter.h"

using namespace PRImA;

/*
 * Class CMsXmlParameterWriter
 *
 * MS XML implementation of CParameterWriter.
 *
 * CC 06.07.2010 - created
 */

/*
 * Constructor
 *
 * 'parentNode' - Parent XML node whereto to write parameters (for AppendParameters(params))
 *                If this parameter is NULL, only the WriteParameters(params, file) can be used.
 */
CMsXmlParameterWriter::CMsXmlParameterWriter(CMsXmlNode * parentNode /*= NULL*/)
{
	m_ParentNode = parentNode;
}

/*
 * Destructor
 */
CMsXmlParameterWriter::~CMsXmlParameterWriter(void)
{
}

/*
 * Creates a new XML file and writes the parameter into it.
 */
void CMsXmlParameterWriter::WriteParameters(CParameterMap * params, CUniString fileName)
{
	if (params == NULL)
		return;

	int i;
	CMsXmlNode * baseElement;

	CMsXmlWriter * xmlWriter = new CMsXmlWriter(fileName);

	IXMLDOMDocument2 * xmlDoc = xmlWriter->CreateXmlDoc();

	//Root node
	baseElement = xmlWriter->CreateRoot(CMsXmlParameterReader::ELEMENT_Parameters,
										NULL, NULL, NULL);

	//Map name
	if (!params->GetName().IsEmpty())
		baseElement->AddAttribute(CMsXmlParameterReader::ATTR_name, params->GetName());

	//Parameters
	for(i = 0; i < params->GetSize(); i++)
	{
		WriteParameterNode(params->Get(i), baseElement);
	}

	// Save XML File With Formatting
	xmlWriter->SaveDocument();
	delete xmlWriter;
}

/*
 * Appends the parameters to the parent node, the writer was created with.
 */
void CMsXmlParameterWriter::AppendParameters(CParameterMap * params)
{
	if (m_ParentNode == NULL)
		return;

	//Parameters
	for(int i = 0; i < params->GetSize(); i++)
	{
		WriteParameterNode(params->Get(i), m_ParentNode);
	}
}

/*
 * Writes an XML element for the given parameter
 * 'param' - Parameter object
 * 'paramsNode' - Parent XML node
 */
void CMsXmlParameterWriter::WriteParameterNode(CParameter * param, CMsXmlNode * paramsNode)
{
	CMsXmlNode * paramNode;
	paramNode = paramsNode->AddChildNode(CMsXmlParameterReader::ELEMENT_Parameter);

	//Type
	paramNode->AddAttribute(CMsXmlParameterReader::ATTR_type, param->GetType());

	//ID
	paramNode->AddAttribute(CMsXmlParameterReader::ATTR_id, param->GetId());

	//Sort Index
	paramNode->AddAttribute(CMsXmlParameterReader::ATTR_sortIndex, param->GetSortIndex());

	//Version
	paramNode->AddAttribute(CMsXmlParameterReader::ATTR_version, param->GetVersion());

	//Name
	paramNode->AddAttribute(CMsXmlParameterReader::ATTR_name, param->GetName());

	//Read Only
	paramNode->AddAttribute(CMsXmlParameterReader::ATTR_readOnly, param->IsReadOnly());

	//Is set
	paramNode->AddAttribute(CMsXmlParameterReader::ATTR_isSet, param->IsSet());

	//Visible
	paramNode->AddAttribute(CMsXmlParameterReader::ATTR_visible, param->IsVisible());

	//Value
	//if (param->IsSet())
	//{
		if (param->GetType() == CParameter::TYPE_BOOL)
			paramNode->AddAttribute(CMsXmlParameterReader::ATTR_value, ((CBoolParameter*)param)->GetValue(false));
		else if (param->GetType() == CParameter::TYPE_INT)
			paramNode->AddAttribute(CMsXmlParameterReader::ATTR_value, ((CIntParameter*)param)->GetValue(false));
		else if (param->GetType() == CParameter::TYPE_DOUBLE)
			paramNode->AddAttribute(CMsXmlParameterReader::ATTR_value, ((CDoubleParameter*)param)->GetValue(false));
		else if (param->GetType() == CParameter::TYPE_STRING)
			paramNode->AddAttribute(CMsXmlParameterReader::ATTR_value, ((CStringParameter*)param)->GetValue(false));
	//}

	//Min, Max, Step for Int Parameters
	paramNode->AddAttribute(CMsXmlParameterReader::ATTR_id, param->GetId());
	if (param->GetType() == CParameter::TYPE_INT)
	{
		CIntParameter * intParam = (CIntParameter*)param;
		//Min
		paramNode->AddAttribute(CMsXmlParameterReader::ATTR_min, intParam->GetMin());
		//Max
		paramNode->AddAttribute(CMsXmlParameterReader::ATTR_max, intParam->GetMax());
		//Step
		paramNode->AddAttribute(CMsXmlParameterReader::ATTR_step, intParam->GetStep());
	}

	//Min, Max, Step for Double Parameters
	if (param->GetType() == CParameter::TYPE_DOUBLE)
	{
		CDoubleParameter * dblParam = (CDoubleParameter*)param;
		//Min
		paramNode->AddAttribute(CMsXmlParameterReader::ATTR_min, dblParam->GetMin());
		//Max
		paramNode->AddAttribute(CMsXmlParameterReader::ATTR_max, dblParam->GetMax());
		//Step
		paramNode->AddAttribute(CMsXmlParameterReader::ATTR_step, dblParam->GetStep());
	}

	//Text Type
	if (param->GetType() == CParameter::TYPE_STRING)
	{
		CStringParameter * stringParam = (CStringParameter*)param;
		paramNode->AddAttribute(CMsXmlParameterReader::ATTR_textType, stringParam->GetTextType());
	}
	//Description
	paramNode->AddTextNode(CMsXmlParameterReader::ELEMENT_Description, param->GetDescription());

	//Valid Values
	if (param->GetType() == CParameter::TYPE_STRING)
	{
		WriteValidValues((CStringParameter*)param, paramNode);
	}
}

/*
 * Valid values for string parameters
 */
void CMsXmlParameterWriter::WriteValidValues(CStringParameter * param, CMsXmlNode * paramNode)
{
	set<CUniString> * validValues = param->GetValidValues();
	set<CUniString>::iterator it = validValues->begin();

	if (!validValues->empty())
	{
		CMsXmlNode * valuesNode = paramNode->AddChildNode(CMsXmlParameterReader::ELEMENT_ValidValues);

		while (it != validValues->end())
		{
			CMsXmlNode * valueNode = valuesNode->AddChildNode(CMsXmlParameterReader::ELEMENT_Value);
			char * buf = new char[512];

			//Type
			valueNode->AddAttribute(CMsXmlParameterReader::ATTR_value, (*it));

			it++;
		}
	}
}