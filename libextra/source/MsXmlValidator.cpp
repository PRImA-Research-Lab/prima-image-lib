#include "MsXmlValidator.h"

using namespace PRImA;

/*
 * Class CMsXmlValidator
 *
 * Implementation for the CXmlValidator interface using MSXML.
 * Provides functions to validate an XML document that has been
 * loaded or created using MSXML.
 * 
 * CC 29.03.2010 - created
 */

const wchar_t* CMsXmlValidator::ELEMENT_complexType		= _T("complexType");
const wchar_t* CMsXmlValidator::ELEMENT_simpleType		= _T("simpleType");
const wchar_t* CMsXmlValidator::ELEMENT_attribute		= _T("attribute");
const wchar_t* CMsXmlValidator::ELEMENT_restriction		= _T("restriction");
const wchar_t* CMsXmlValidator::ELEMENT_enumeration		= _T("enumeration");
const wchar_t* CMsXmlValidator::ELEMENT_sequence		= _T("sequence");
const wchar_t* CMsXmlValidator::ELEMENT_element			= _T("element");
const wchar_t* CMsXmlValidator::ELEMENT_annotation		= _T("annotation");
const wchar_t* CMsXmlValidator::ELEMENT_documentation	= _T("documentation");
const wchar_t* CMsXmlValidator::ELEMENT_complexContent	= _T("complexContent");
const wchar_t* CMsXmlValidator::ELEMENT_extension		= _T("extension");
const wchar_t* CMsXmlValidator::ATTR_name				= _T("name");
const wchar_t* CMsXmlValidator::ATTR_type				= _T("type");
const wchar_t* CMsXmlValidator::ATTR_value				= _T("value");

/*
 * Constructor
 *
 * Loads the schema and extract types and documentation (if extractTypesAndDocumentation true)
 *
 * In case of an error the m_ErrorMsg field is populated (use GetErrorMsg)
 */
CMsXmlValidator::CMsXmlValidator(const wchar_t * schemaFile, const wchar_t * nameSpace,
								 map<CUniString,CUniString> * docMap /* = NULL */, bool extractTypesAndDocumentation /*= true */)
{
	HRESULT hr = CoInitialize(NULL);

    // Create a schema cache
	if (CoCreateInstance(__uuidof(XMLSchemaCache60), NULL, CLSCTX_INPROC_SERVER, 
				IID_IXMLDOMSchemaCollection, (LPVOID*)&m_SchemaCollection) == S_OK)
    //if (pXS->CreateInstance(__uuidof(XMLSchemaCache60), NULL, CLSCTX_INPROC_SERVER) == S_OK)
	{
		VARIANT varSchemaFile;
		VariantFromString(schemaFile, varSchemaFile);
		HRESULT res = 0;
		if ((res = m_SchemaCollection->add((BSTR)nameSpace, varSchemaFile)) == S_OK)
		{
		}
		else
		{
			CUniString msg(L"Error adding XML schema ");
			msg.Append(schemaFile);
			
			IErrorInfo * iErrorInfo = NULL;
			if (GetErrorInfo(0, &iErrorInfo) == S_OK && iErrorInfo != NULL)
			{
				BSTR reason;
				if (iErrorInfo->GetDescription(&reason) == S_OK)
				{
					msg.Append(L" \n");
					msg.Append(reason);
				}
			}
			//IErrorInfo::GetDescription(&reason);

			m_ErrorMsg = msg;
		}
		VariantClear(&varSchemaFile);
	}

	//Read the xsd and extract types and documentation
	if (extractTypesAndDocumentation)
	{
		CMsXmlReader * msXmlReader = new CMsXmlReader(CUniString(schemaFile));
		IXMLDOMDocument2 * xsdDoc = msXmlReader->GetXmlDoc();
		if (xsdDoc != NULL)
		{
			m_Documentation = docMap;
			FillTypeMap(xsdDoc);
		}
		delete msXmlReader;
	}
}

/*
 * Destructor
 */
CMsXmlValidator::~CMsXmlValidator(void)
{
	if (m_SchemaCollection != NULL)
		m_SchemaCollection->Release();

	//Delete map content
	map<CUniString, map<CUniString, CUniString>*>::iterator it = m_TypeMap.begin();
	while (it != m_TypeMap.end())
	{
		delete (*it).second;
		it++;
	}

	map<CUniString, map<int, CUniString>*>::iterator it2 = m_SimpleTypesByIndex.begin();
	while (it2 != m_SimpleTypesByIndex.end())
	{
		delete (*it2).second;
		it2++;
	}

	map<CUniString, map<CUniString, int>*>::iterator it3 = m_SimpleTypesByName.begin();
	while (it3 != m_SimpleTypesByName.end())
	{
		delete (*it3).second;
		it3++;
	}

	CoUninitialize();
}

/*
 * Validates the given XML document (IXMLDOMDocument2).
 */
int CMsXmlValidator::validate(void * pDoc)
{
	if (pDoc == NULL)
		return -1;

	IXMLDOMDocument2 * xmlDoc = (IXMLDOMDocument2 *)pDoc;
	IXMLDOMParseError * err;

	CComVariant varSchemaCollection = m_SchemaCollection;
	xmlDoc->putref_schemas(varSchemaCollection);
		
	long res = xmlDoc->validate(&err);
	if (res == S_FALSE) //Not valid
	{
		CUniString msg;
		BSTR reason;
		err->get_reason(&reason);
		if (reason != NULL)
			msg.Append(CUniString(reason));

		BSTR srcText;
		err->get_srcText(&srcText);
		if (srcText != NULL)
		{
			msg.Append(_T("\n"));
			msg.Append(CUniString(srcText));
		}

		m_ErrorMsg = msg;
		return -1;
	}

	return 0;
}

/*
 * Returns the last error message
 */
CUniString CMsXmlValidator::GetErrorMsg()
{
	return m_ErrorMsg;
}

// Helper function to create a VT_BSTR variant from a null terminated string. 
HRESULT CMsXmlValidator::VariantFromString(PCWSTR wszValue, VARIANT &Variant)
{
    HRESULT hr = S_OK;
    BSTR bstr = SysAllocString(wszValue);

	if (!bstr) 
	{ 
		hr = E_OUTOFMEMORY; 
		return hr;
	}
    
    V_VT(&Variant)   = VT_BSTR;
    V_BSTR(&Variant) = bstr;

    return hr;
}


/*
 * Returns the schema type of the given object (attribut or element).
 */
CUniString CMsXmlValidator::GetType(CUniString parentElementType, CUniString objectName)
{
	//Find name type map
	map<CUniString, CUniString> * nameTypeMap = NULL;
	map<CUniString, map<CUniString, CUniString>*>::iterator it = m_TypeMap.find(parentElementType);
	if (it != m_TypeMap.end())
	{
		nameTypeMap = (*it).second;
	}

	if (nameTypeMap == NULL)
		return CUniString();

	//Find type
	CUniString ret;
	map<CUniString, CUniString>::iterator it2 = nameTypeMap->find(objectName);
	if (it2 != nameTypeMap->end())
	{
		ret = (*it2).second;
	}

	if (ret.IsEmpty()) //Nothing found
	{
		//Check for PlainText and Unicode
		if (objectName.Find(CUniString("PlainText"))==0 || objectName.Find(CUniString("Unicode"))==0)
			ret.SetString(CUniString("string"));
	}

	return ret;
}

/*
 * Parses the xsd document and fills the internal type and documentation maps.
 */
void CMsXmlValidator::FillTypeMap(IXMLDOMDocument2 * xsdDoc)
{
	CMsXmlNode baseElement;
	CUniString value;

	IXMLDOMElement * element;
	if (xsdDoc->get_documentElement(&element) != S_OK)
	{
		return;
	}
	baseElement.SetNodeObject(element, xsdDoc);

	if (m_Documentation != NULL)
		m_Documentation->clear();

	CMsXmlNode * tempNode = baseElement.GetFirstChild();
	while (tempNode != NULL)
	{
		//Complex type
		if(tempNode->GetName() == CUniString(ELEMENT_complexType))
		{
			//Name
			if(tempNode->HasAttribute(ATTR_name))
			{
				value = tempNode->GetAttribute(ATTR_name);

				//Find name-type map
				CUniString key = value;
				map<CUniString, CUniString> * nameTypeMap = NULL;
				map<CUniString, map<CUniString, CUniString>*>::iterator it = m_TypeMap.find(key);
				if (it != m_TypeMap.end())
				{
					nameTypeMap = (*it).second;
				}

				if (nameTypeMap == NULL) //Create new map
				{
					nameTypeMap = new map<CUniString, CUniString>();
					m_TypeMap.insert(pair<CUniString, map<CUniString, CUniString> *>(key, nameTypeMap));
				}
				
				ParseComplexType(tempNode, nameTypeMap, key);
			}
		}
		//Simple type
		else if(tempNode->GetName() == CUniString(ELEMENT_simpleType))
		{
			//Name
			if(tempNode->HasAttribute(ATTR_name))
			{
				value = tempNode->GetAttribute(ATTR_name);
				//Find name-type map
				CUniString key = value;
				map<int, CUniString> * valueMapByIndex = NULL;
				map<CUniString, int> * valueMapByName = NULL;
				map<CUniString, map<int, CUniString>*>::iterator it = m_SimpleTypesByIndex.find(key);
				if (it != m_SimpleTypesByIndex.end())
				{
					valueMapByIndex = (*it).second;
				}

				map<CUniString, map<CUniString, int>*>::iterator it2 = m_SimpleTypesByName.find(key);
				if (it2 != m_SimpleTypesByName.end())
				{
					valueMapByName = (*it2).second;
				}

				if (valueMapByIndex == NULL) //Create new map
				{
					valueMapByIndex = new map<int, CUniString>();
					m_SimpleTypesByIndex.insert(pair<CUniString, map<int, CUniString> *>(key, valueMapByIndex));
					valueMapByName = new map<CUniString, int>();
					m_SimpleTypesByName.insert(pair<CUniString, map<CUniString, int> *>(key, valueMapByName));
				}
				
				ParseSimpleType(tempNode, valueMapByIndex, valueMapByName);
			}
		}
		tempNode = tempNode->GetNextSibling();
	}
}

/*
 * Finds the possible values of the given simple type element and stores them in the map.
 */
void CMsXmlValidator::ParseSimpleType(CMsXmlNode * node, map<int, CUniString> * valueMapByIndex, 
									map<CUniString, int> * valueMapByName)
{
	CUniString tempString;

	CMsXmlNode * tempNode = node->GetFirstChild();
	while (tempNode != NULL)
	{
		//Restriction
		if(tempNode->GetName() == CUniString(ELEMENT_restriction))
		{
			tempNode = tempNode->GetFirstChild();
			while (tempNode != NULL)
			{
				//Enumeration
				if(tempNode->GetName() == CUniString(ELEMENT_enumeration))
				{
					//Value
					tempString = tempNode->GetAttribute(ATTR_value);

					valueMapByIndex->insert(pair<int, CUniString>((int)valueMapByIndex->size(), tempString));
					valueMapByName->insert(pair<CUniString, int>(tempString, valueMapByName->size()));
				}
				tempNode = tempNode->GetNextSibling();
			}
			break;
		}
		tempNode = tempNode->GetNextSibling();
	}
}

/*
 * Inserts the attrName-attrType pairs of the given complext type element into the nameTypeMap.
 */
void CMsXmlValidator::ParseComplexType(CMsXmlNode * node, map<CUniString, CUniString> * nameTypeMap, 
									 CUniString & typeName)
{
	HandleSequenceAttributeAndComplexContent(node, nameTypeMap, typeName);
}

/*
 * Parses the content of a 'Complecx Type'
 */
void CMsXmlValidator::HandleSequenceAttributeAndComplexContent(CMsXmlNode * node, map<CUniString, CUniString> * nameTypeMap, 
						CUniString & typeName)
{
	CUniString tempString;

	CMsXmlNode * tempNode = node->GetFirstChild();
	while (tempNode != NULL)
	{
		//Attribute
		if(tempNode->GetName() == CUniString(ELEMENT_attribute))
		{
			//Name
			CUniString name = tempNode->GetAttribute(ATTR_name);

			//Type
			if(tempNode->HasAttribute(ATTR_type))
			{
				CUniString type = tempNode->GetAttribute(ATTR_type);
				if (type.Find(CUniString("pc:")) == 0)
				{
					type = type.Right(type.GetLength()-3);
				}
				nameTypeMap->insert(pair<CUniString, CUniString>(name, type));
			}			

			//Documentation
			ParseAttribute(tempNode, typeName, name);
		}
		//Sequence
		else if(tempNode->GetName() == CUniString(ELEMENT_sequence))
		{
			CMsXmlNode * tempNode2 = tempNode->GetFirstChild();
			while (tempNode2 != NULL)
			{
				//Element
				if(tempNode->GetName() == CUniString(ELEMENT_element))
				{
					//Name
					CUniString name = tempNode2->GetAttribute(ATTR_name);

					//Type
					if(tempNode2->HasAttribute(ATTR_type))
					{
						CUniString type = tempNode2->GetAttribute(ATTR_type);
						if (type.Find(CUniString("pc:")) == 0)
						{
							type = type.Right(type.GetLength()-3);
						}
						nameTypeMap->insert(pair<CUniString, CUniString>(name, type));
					}			
				}
				tempNode2 = tempNode2->GetNextSibling();
			}
		}
		//ComplexContent (for inheritance)
		else if(tempNode->GetName() == CUniString(ELEMENT_complexContent))
		{
			//Look for extension
			CMsXmlNode * extNode = tempNode->GetFirstChild();
			while (extNode != NULL)
			{
				if(extNode->GetName() == CUniString(ELEMENT_extension))
					HandleSequenceAttributeAndComplexContent(extNode, nameTypeMap, typeName);
				extNode = extNode->GetNextSibling();
			}
		}
		tempNode = tempNode->GetNextSibling();
	}
}


/*
 * Parse attribute metadata (e.g. documentation)
 */
void CMsXmlValidator::ParseAttribute(CMsXmlNode * node, CUniString & typeName, CUniString & attrName)
{
	if (m_Documentation == NULL)
		return;

	//The key for the documentation map is: <type name>::<attribute name>
	CUniString key(typeName);
	key.Append(_T("::"));
	key.Append(attrName);

	CMsXmlNode * tempNode = node->GetFirstChild();
	while (tempNode != NULL)
	{
		//Annotation
		if(tempNode->GetName() == CUniString(ELEMENT_annotation))
		{
			CMsXmlNode * tempNode2 = tempNode->GetFirstChild();
			while (tempNode2 != NULL)
			{
				//Documentation
				if(tempNode->GetName() == CUniString(ELEMENT_documentation))
				{
					CUniString content = tempNode2->GetTextContent();
					m_Documentation->insert(pair<CUniString, CUniString>(key, content));
				}
				tempNode2 = tempNode2->GetNextSibling();
			}
		}
		tempNode = tempNode->GetNextSibling();
	}
}

/*
 * Returns the schema documentation of the specified attribute.
 * Returns an empty string if the attribute could not be found.
 */
CUniString CMsXmlValidator::GetDocumentation(CUniString & type, CUniString & attr)
{
	CUniString key(type);
	key.Append(_T("::"));
	key.Append(attr);
	map<CUniString, CUniString>::iterator it = m_Documentation->find(key);
	if (it != m_Documentation->end())
		return (*it).second;
	return CUniString();
}

/*
 * Returns a map <index, simple type value> (Simple type values: colour, chart type, ...)
 */
map<int, CUniString> *  CMsXmlValidator::GetSimpleTypeValuesByIndex(CUniString typeName)
{
	map<CUniString, map<int, CUniString> *>::iterator it = m_SimpleTypesByIndex.find(typeName);
	if (it == m_SimpleTypesByIndex.end())
		return NULL;
	return (*it).second;
}

/*
 * Returns a map <index, simple type value> (Simple type values: colour, chart type, ...)
 */
map<CUniString, int> *  CMsXmlValidator::GetSimpleTypeValuesByName(CUniString typeName)
{
	map<CUniString, map<CUniString, int> *>::iterator it = m_SimpleTypesByName.find(typeName);
	if (it == m_SimpleTypesByName.end())
		return NULL;
	return (*it).second;
}

/*
 * Sets the last error message to the given string
 */
void CMsXmlValidator::SetErrorMsg(CUniString msg)
{
	m_ErrorMsg.SetString(CUniString(msg));
}