#include <afxcoll.h>
#include "MsXmlNode.h"

using namespace PRImA;

/*
 * Class CMsXmlNode
 *
 * Wrapper for IXMLDOMElement of MSXML.
 *
 * CC 24.03.2010
 */

/*
 * Constructor
 */
CMsXmlNode::CMsXmlNode()
{
	m_CurrChild = NULL;
	m_CurrSibling = NULL;
	m_Name = NULL;
	m_XmlDoc = NULL;
	m_NameSpace = NULL;
	m_Children = NULL;
}

/*
 * Constructor
 */
CMsXmlNode::CMsXmlNode(IXMLDOMElement * nodeObject, IXMLDOMDocument2 * xmlDoc)
{
	m_NodeObject = nodeObject;
	m_CurrChild = NULL;
	m_CurrSibling = NULL;
	m_Name = NULL;
	m_XmlDoc = xmlDoc;
	m_NameSpace = NULL;
	m_Children = NULL;
}

/*
 * Destructor
 */
CMsXmlNode::~CMsXmlNode()
{
	if (m_Children != NULL)
	{
		for (unsigned int i = 0; i < m_Children->size(); i++)
			delete m_Children->at(i);
		delete m_Children;
	}

	//Not recursive anymore (caused stack overflow)
	CMsXmlNode * sibling = m_CurrSibling;
	vector<CMsXmlNode *> siblings;
	while (sibling != NULL)
	{
		siblings.push_back(sibling);
		sibling = sibling->m_CurrSibling;
	}
	for (int i=(int)siblings.size()-1; i>=0; i--)
	{
		delete siblings[i];
		if (i>0)
			siblings[i-1]->m_CurrSibling = NULL;
	}
	//delete m_CurrSibling;

	delete m_CurrChild;
	if (m_NodeObject != NULL)
		m_NodeObject->Release();
	delete m_Name;
}

/*
 * Sets the internal MSXML element node object.
 */
void CMsXmlNode::SetNodeObject(IXMLDOMElement * nodeObject, IXMLDOMDocument2 * xmlDoc)
{
	m_NodeObject = nodeObject;
	m_XmlDoc = xmlDoc;
}

/*
 * Returns the name of the XML element.
 */
CUniString CMsXmlNode::GetName()
{
	if (m_Name != NULL)
		return CUniString(m_Name);

	m_Name = new CUniString();
	if (m_NodeObject == NULL)
		return CUniString(m_Name);

	BSTR tempString = NULL;
	if (m_NodeObject->get_nodeName(&tempString) == S_OK)
	{
		m_Name->Append((wchar_t*)tempString);
		SysFreeString(tempString);
	}

	return CUniString(m_Name);
}

/*
 * Checks if this XML element has an attribute with the given name.
 */
bool CMsXmlNode::HasAttribute(const wchar_t * attrName)
{
	VARIANT variant;
	if (m_NodeObject->getAttribute((BSTR)attrName, &variant) == S_OK)
	{
		bool res = variant.vt != NULL;
		VariantClear(&variant);
		return res;
	}
	return false;
}

/*
 * Returns the value of the attribute with the given name.
 */
CUniString CMsXmlNode::GetAttribute(const wchar_t * attrName)
{
	CUniString value;
	VARIANT variant;
	if (m_NodeObject->getAttribute((BSTR)attrName, &variant) == S_OK)
	{
		if(variant.vt != NULL)
			value.Append(variant.bstrVal);
		VariantClear(&variant);
	}
	return value;
}

/*
 * Returns the value of the attribute with the given name.
 */
int CMsXmlNode::GetIntAttribute(const wchar_t * attrName)
{
	CUniString value = GetAttribute(attrName);
	int ret = 0;
	if (!value.IsEmpty())
		ret = _wtoi(value.GetBuffer());
	return ret;
}

/*
 * Returns the value of the attribute with the given name.
 */
double CMsXmlNode::GetDoubleAttribute(const wchar_t * attrName)
{
	CUniString value = GetAttribute(attrName);
	double ret = 0;
	if (!value.IsEmpty())
		ret = _wtof(value.GetBuffer());
	return ret;
}

/*
 * Returns the value of the attribute with the given name.
 */
bool CMsXmlNode::GetBoolAttribute(const wchar_t * attrName)
{
	CUniString value = GetAttribute(attrName);
	bool ret = false;
	if (!value.IsEmpty())
		ret = (value == CUniString(_T("true")));
	return ret;
}

/*
 * Checks if this XML element has child nodes.
 */
bool CMsXmlNode::HasChildren()
{
	VARIANT_BOOL varBool;
	if(m_NodeObject->hasChildNodes(&varBool) == S_OK && varBool == VARIANT_TRUE)
		return true;
	return false;
}

/*
 * Returns the first child of this node or NULL, if it has no children.
 * NOTE: Deletes the object of the previous GetFirstChild call!
 */
CMsXmlNode * CMsXmlNode::GetFirstChild()
{
	delete m_CurrChild;
	m_CurrChild = NULL;
	IXMLDOMNode * tempNode = NULL;
	IXMLDOMElement * tempElement = NULL;
	if (m_NodeObject->get_firstChild(&tempNode) == S_OK && tempNode != NULL)
	{
		DOMNodeType nodeType;
		if (tempNode->get_nodeType(&nodeType) == S_OK)
		{
			if (nodeType == NODE_ELEMENT)
			{
				tempNode->QueryInterface(IID_IXMLDOMElement, (void**)&tempElement);
				m_CurrChild = new CMsXmlNode(tempElement, m_XmlDoc);
			}
			else
			{
				if (tempElement != NULL)
					tempElement->Release();
			}
		}
		else
		{
			if (tempElement != NULL)
				tempElement->Release();
		}
	}
	if (tempNode != NULL)
		tempNode->Release();
	return m_CurrChild;
}

/*
 * Returns the next child of parents child list or NULL, if there is no next child.
 * NOTE: Deletes the object of the previous GetNextSibling call!
 */
CMsXmlNode * CMsXmlNode::GetNextSibling()
{
	delete m_CurrSibling;
	m_CurrSibling = NULL;
	if (m_NodeObject == NULL)
		return NULL;

	IXMLDOMNode * tempNode = NULL;
	IXMLDOMElement * tempElement = NULL;
	if (m_NodeObject->get_nextSibling(&tempNode) == S_OK && tempNode != NULL)
	{
		DOMNodeType nodeType;
		if (tempNode->get_nodeType(&nodeType) == S_OK)
		{
			if (nodeType == NODE_ELEMENT)
			{
				tempNode->QueryInterface(IID_IXMLDOMElement, (void**)&tempElement);
				m_CurrSibling = new CMsXmlNode(tempElement, m_XmlDoc);
			}
			else
			{
				if (tempElement != NULL)
					tempElement->Release();
			}
		}
		else
		{
			if (tempElement != NULL)
				tempElement->Release();
		}
	}
	if (tempNode != NULL)
		tempNode->Release();
	return m_CurrSibling;
}

/*
 * Retunrs the text content of this XML element.
 * Note: Converts the line endings of the text. See CExtraString::ConvertLineEndings.
 */
CUniString CMsXmlNode::GetTextContent()
{
	CUniString text;
	
	if (!HasChildren())
		return text;

	IXMLDOMNode * tempNode = NULL;
	if (m_NodeObject->get_firstChild(&tempNode) == S_OK)
	{
		VARIANT variant;
		if (tempNode->get_nodeValue(&variant) == S_OK)
			text.Append(variant.bstrVal);
		VariantClear(&variant);
	}
	if (tempNode != NULL)
		tempNode->Release();

	//Convert line ending
	CExtraString::ConvertLineEndings(text);

	return text;
}

/*
 * Add an attribute with the given name and value to this XML element.
 */
void CMsXmlNode::AddAttribute(const wchar_t * attrName, CUniString value)
{
	CreateAndAddAttributeNode(m_XmlDoc, attrName, value.GetBuffer(), (IXMLDOMElement*)m_NodeObject);
}

/*
 * Add an attribute with the given name and value to this XML element.
 */
void CMsXmlNode::AddAttribute(const wchar_t * attrName, int value)
{
	CUniString str;
	str.Append(value);
	AddAttribute(attrName, str);
}

/*
 * Add an attribute with the given name and value to this XML element.
 */
void CMsXmlNode::AddAttribute(const wchar_t * attrName, double value)
{
	CUniString str;
	str.Format(_T("%.5f"), value);
	AddAttribute(attrName, str);
}

/*
 * Add an attribute with the given name and value to this XML element.
 */
void CMsXmlNode::AddAttribute(const wchar_t * attrName, bool value)
{
	CUniString str(value ? _T("true") : _T("false"));
	AddAttribute(attrName, str);
}

/*
 * Adds a child element with the specified name to this XML element.
 * Note: The returned object will be deleted automatically when
 *       this XML element is destroyed. 
 */
CMsXmlNode * CMsXmlNode::AddChildNode(const wchar_t * nodeName)
{
    IXMLDOMElement *pNode = NULL;
	if (CreateAndAddElementNode(m_XmlDoc, nodeName, this->m_NameSpace, _T("\n\t"), m_NodeObject, &pNode) == S_OK
		&& pNode != NULL)
	{	
		CMsXmlNode * ret = new CMsXmlNode(pNode, m_XmlDoc);
		ret->SetNameSpace(this->m_NameSpace);
		if (m_Children == NULL)
			m_Children = new vector<CMsXmlNode*>();
		m_Children->push_back(ret);
		return ret;
	}
	if (pNode != NULL)
		pNode->Release();
	return NULL;
}

/*
 * Removes the given child node.
 */
void CMsXmlNode::DeleteChildNode(CMsXmlNode * child)
{
	if (m_Children != NULL)
	{
		for (unsigned int i=0; i<m_Children->size(); i++)
			if (child == m_Children->at(i))
			{
				m_Children->erase(m_Children->begin()+i);
				break;
			}
	}
	RemoveChildFromParent(child->m_NodeObject, m_NodeObject);
	delete child;
}

// Helper function to create and append an element node to a parent node, and pass the newly created
// element node to caller if it wants.
HRESULT CMsXmlNode::CreateAndAddElementNode(IXMLDOMDocument2 *pDom, PCWSTR wszName, 
								PCWSTR wszNs, PCWSTR wszNewline, 
								IXMLDOMNode *pParent, IXMLDOMElement **ppElement)
{
    HRESULT hr = S_OK;
    IXMLDOMElement* pElement = NULL;

	HRESULT res = S_OK;
	if (wszNs != NULL)
		res = CreateElement(pDom, wszName, wszNs, &pElement);
	else
		res = CreateElement(pDom, wszName, &pElement);
    if (res == S_OK)
	{
		// Add NEWLINE+TAB for identation before this element.
		if (CreateAndAddTextNode(pDom, wszNewline, pParent) == S_OK)
		{
			// Append this element to parent.
			if (AppendChildToParent(pElement, pParent) == S_OK)
			{
			}
		}
	}

	//CleanUp:
    if (ppElement)
        *ppElement = pElement;  // Caller is repsonsible to release this element.
    else
	{
        if (pElement != NULL) // Caller is not interested on this element, so release it.
			pElement->Release();
	}

    return hr;
}

// Helper function to append a child to a parent node.
HRESULT CMsXmlNode::AppendChildToParent(IXMLDOMNode *pChild, IXMLDOMNode *pParent)
{
    HRESULT hr = S_OK;
    IXMLDOMNode *pChildOut = NULL;
    hr = pParent->appendChild(pChild, &pChildOut);

	//CleanUp:
    if (pChildOut != NULL)
		pChildOut->Release();
    return hr;
}

//Helper function to remove child node from parent
HRESULT CMsXmlNode::RemoveChildFromParent(IXMLDOMNode *pChild, IXMLDOMNode *pParent)
{
    HRESULT hr = S_OK;
	IXMLDOMNode *pOldChild = NULL;
	hr = pParent->removeChild(pChild, &pOldChild);

	//CleanUp:
    if (pOldChild != NULL)
		pOldChild->Release();
    return hr;
}

// Helper function to create and append a text node to a parent node.
HRESULT CMsXmlNode::CreateAndAddTextNode(IXMLDOMDocument2 *pDom, PCWSTR wszText, IXMLDOMNode *pParent)
{
    HRESULT hr = S_OK;    
    IXMLDOMText *pText = NULL;

    BSTR bstrText = SysAllocString(wszText);
    if(bstrText)
	{
		if (pDom->createTextNode(bstrText, &pText) == S_OK)
			AppendChildToParent(pText, pParent);
	}

	//CleanUp:
    if (pText != NULL)
		pText->Release();
    SysFreeString(bstrText);
    return hr;
}

/*
 * Creates a child node with text content
 */
void CMsXmlNode::AddTextNode(const wchar_t * nodeName, CUniString value)
{
	CMsXmlNode * newNode = AddChildNode(nodeName);

	CreateAndAddTextNode(m_XmlDoc, value.GetBuffer(), newNode->m_NodeObject);
}


// Helper that allocates the BSTR param for the caller.
HRESULT CMsXmlNode::CreateElement(IXMLDOMDocument2 *pXMLDom, PCWSTR wszName, 
								  IXMLDOMElement **ppElement)
{
    HRESULT hr = S_OK;
    *ppElement = NULL;

    BSTR bstrName = SysAllocString(wszName);
    if (!bstrName)
	{
		SysFreeString(bstrName);
		return E_INVALIDARG;
	}
    pXMLDom->createElement(bstrName, ppElement);

	//CleanUp:
    SysFreeString(bstrName);
    return hr;
}

// Helper that allocates the BSTR param for the caller.
HRESULT CMsXmlNode::CreateElement(IXMLDOMDocument2 *pXMLDom, PCWSTR wszName, PCWSTR wszNs, 
								  IXMLDOMElement **ppElement)
{
    HRESULT hr = S_OK;
    *ppElement = NULL;

    BSTR bstrName = SysAllocString(wszName);
    BSTR bstrNs = SysAllocString(wszNs);
    if (!bstrName || !bstrNs)
	{
		SysFreeString(bstrName);
		SysFreeString(bstrNs);
		return hr;
	}
	//CC - We have to use createNode to be able to set the namespace
    //pXMLDom->createElement(bstrName, ppElement);
	IXMLDOMNode * node = NULL;
	CComVariant type;
	type = NODE_ELEMENT;

	if (pXMLDom->createNode(type, bstrName, bstrNs, &node) == S_OK)
	{
		node->QueryInterface(IID_IXMLDOMElement, (void **)ppElement);
	}

	//CleanUp:
	if (node != NULL)
		node->Release();
    SysFreeString(bstrName);
    SysFreeString(bstrNs);
    return hr;
}

// Helper function to create and add an attribute to a parent node.
HRESULT CMsXmlNode::CreateAndAddAttributeNode(IXMLDOMDocument2 *pDom, PCWSTR wszName, PCWSTR wszValue, 
											  IXMLDOMElement *pParent)
{
    HRESULT hr = S_OK;
    IXMLDOMAttribute *pAttribute = NULL;
    IXMLDOMAttribute *pAttributeOut = NULL; // Out param that is not used

    BSTR bstrName = NULL;
    VARIANT varValue;
    VariantInit(&varValue);

    bstrName = SysAllocString(wszName);
    if (bstrName)
	{
		if (VariantFromString(wszValue, varValue) == S_OK)
		{
			if (pDom->createAttribute(bstrName, &pAttribute) == S_OK)
			{
				if (pAttribute->put_value(varValue) == S_OK)
					pParent->setAttributeNode(pAttribute, &pAttributeOut);
			}
		}
	}

	//CleanUp:
    if(pAttribute != NULL)
		pAttribute->Release();
    if(pAttributeOut != NULL)
		pAttributeOut->Release();
    SysFreeString(bstrName);
    VariantClear(&varValue);
    return hr;
}

// Helper function to create a VT_BSTR variant from a null terminated string. 
HRESULT CMsXmlNode::VariantFromString(PCWSTR wszValue, VARIANT &Variant)
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

