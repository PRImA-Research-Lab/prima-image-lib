#include <afxcoll.h>
#include "MsXmlWriter.h"

using namespace PRImA;

/*
 * Class CMsXmlWriter
 *
 * Helper class to create and save XML files using MSXML.
 *
 * CC 29.03.2010 - created
 */


/*
 * Constructor
 */
CMsXmlWriter::CMsXmlWriter(CUniString & fileName)
{
	m_XmlDoc = NULL;
	m_Root = NULL;

	HRESULT hr = CoInitialize(NULL);
    if(SUCCEEDED(hr))
	{
		m_FileName = fileName;
	}
}

/*
 * Destructor
 */
CMsXmlWriter::~CMsXmlWriter(void)
{
    if (m_XmlDoc != NULL)
		m_XmlDoc->Release();
	delete m_Root;
	CoUninitialize();
}

/*
 * Creates a new XML document.
 * Sets XML version to 1.0 and encoding to UTF-8.
 */
IXMLDOMDocument2 * CMsXmlWriter::CreateXmlDoc()
{
	if (m_XmlDoc == NULL)
	{

		if (CreateAndInitDOM(&m_XmlDoc) == S_OK)
		{
			if (CreateAndAddPINode(m_XmlDoc, _T("xml"), _T("version=\"1.0\" encoding=\"UTF-8\"")) == S_OK)
			{
				m_XmlDoc->put_resolveExternals(VARIANT_FALSE);
			}
		}
		//TODO Error Handling
	}
	return m_XmlDoc;
}

/*
 * Creates the root element for the XML document (no namespace).
 */
CMsXmlNode * CMsXmlWriter::CreateRoot(const wchar_t * nodeName)
{
	return CreateRoot(nodeName, NULL, NULL, NULL);
}

/*
 * Creates the root element for the XML document and sets the
 * given name space properties.
 */
CMsXmlNode * CMsXmlWriter::CreateRoot(const wchar_t * nodeName, const wchar_t * nameSpace,
									const wchar_t * xmlnsXsi, const wchar_t * schemaLocation,
									const wchar_t * xlinkNameSpace /*= NULL*/)
{
	if (m_XmlDoc == NULL)
		return NULL;

	if (m_Root == NULL)
	{
		IXMLDOMElement * pRoot = NULL;
		if (nameSpace != NULL)
		{
			if (CreateElement(m_XmlDoc, nodeName, nameSpace, &pRoot) == S_OK)
			{
				// Add NEWLINE for identation before </root>.
				//if (CreateAndAddTextNode(pXMLDom, _T("\n"), pRoot) == S_OK);
				// add <root> to document
				if (AppendChildToParent(pRoot, m_XmlDoc) >= 0)
				{
					m_Root = new CMsXmlNode(pRoot, m_XmlDoc);
					m_Root->SetNameSpace(const_cast<wchar_t*>(nameSpace));
					
					VARIANT varXmlnsXsi;
					VariantFromString(xmlnsXsi, varXmlnsXsi);

					// Add the schema attributes to the Root Element
					// First declare the xsi namespace
					pRoot->setAttribute(_T("xmlns:xsi"), varXmlnsXsi);
					VariantClear(&varXmlnsXsi);

					if (xlinkNameSpace != NULL)
					{
						VARIANT varXmlnsXlink;
						VariantFromString(xlinkNameSpace, varXmlnsXlink);
						pRoot->setAttribute(_T("xmlns:xlink"), varXmlnsXlink);
						VariantClear(&varXmlnsXlink);
					}					

					// Next set the schema location
					// MSXML requires that namespace qualified Attributes
					// be created as Nodes, then set
					CComVariant varType = NODE_ATTRIBUTE;
					IXMLDOMNode * spSchemaLocationAttribute = NULL;
					m_XmlDoc->createNode(varType, _T("xsi:schemaLocation"), _T("http://www.w3.org/2001/XMLSchema-instance"),
										&spSchemaLocationAttribute);
					IXMLDOMAttribute * attr1 = NULL;
					IXMLDOMAttribute * attr2 = NULL;

					spSchemaLocationAttribute->QueryInterface(IID_IXMLDOMAttribute, (void**)&attr1);

					pRoot->setAttributeNode(attr1, &attr2);
					
					// We can finally set it now
					VARIANT varSchemaLocation;
					VariantFromString(schemaLocation, varSchemaLocation);
					pRoot->setAttribute(_T("xsi:schemaLocation"), varSchemaLocation);

					if (spSchemaLocationAttribute != NULL)
						spSchemaLocationAttribute->Release();
					if (attr1 != NULL)
						attr1->Release();
					if (attr2 != NULL)
						attr2->Release();
					VariantClear(&varSchemaLocation);
				}
			}
		}
		else //No namespace specified
		{
			if (CreateElement(m_XmlDoc, nodeName, &pRoot) == S_OK)
			{
				// Add NEWLINE for identation before </root>.
				//if (CreateAndAddTextNode(pXMLDom, _T("\n"), pRoot) == S_OK);
				// add <root> to document
				if (AppendChildToParent(pRoot, m_XmlDoc) == S_OK)
				{
					m_Root = new CMsXmlNode(pRoot, m_XmlDoc);
				}
			}
		}
	}
	return m_Root;
}

/*
 * Saves the document to the specified file (see constructor).
 */
bool CMsXmlWriter::SaveDocument()
{
    VARIANT varFileName;
	bool success = false;
	if (VariantFromString(m_FileName.GetBuffer(), varFileName) >= 0)
	{
		if (m_XmlDoc->save(varFileName) >= 0)
			success = true;
	}

    VariantClear(&varFileName);

	return success;
}

// Helper function to create a DOM instance. 
HRESULT CMsXmlWriter::CreateAndInitDOM(IXMLDOMDocument2 **ppDoc)
{
    HRESULT hr = CoCreateInstance(__uuidof(DOMDocument60), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(ppDoc));
    if (SUCCEEDED(hr))
    {
        // these methods should not fail so don't inspect result
        (*ppDoc)->put_async(VARIANT_FALSE);  
        (*ppDoc)->put_validateOnParse(VARIANT_FALSE);
        (*ppDoc)->put_resolveExternals(VARIANT_FALSE);
    }
    return hr;
}

// Helper function to create and add a processing instruction to a document node.
HRESULT CMsXmlWriter::CreateAndAddPINode(IXMLDOMDocument2 *pDom, PCWSTR wszTarget, PCWSTR wszData)
{
    HRESULT hr = S_OK;
    IXMLDOMProcessingInstruction *pPI = NULL;

    BSTR bstrTarget = SysAllocString(wszTarget);
    BSTR bstrData = SysAllocString(wszData);
    if(bstrTarget && bstrData)
	{
		if (pDom->createProcessingInstruction(bstrTarget, bstrData, &pPI) >= 0)
			AppendChildToParent(pPI, pDom);
	}

	//CleanUp:
    if(pPI != NULL)
		pPI->Release();
    SysFreeString(bstrTarget);
    SysFreeString(bstrData);
    return hr;
}

// Helper function to create a VT_BSTR variant from a null terminated string. 
HRESULT CMsXmlWriter::VariantFromString(PCWSTR wszValue, VARIANT &Variant)
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

// Helper function to append a child to a parent node.
HRESULT CMsXmlWriter::AppendChildToParent(IXMLDOMNode *pChild, IXMLDOMNode *pParent)
{
    HRESULT hr = S_OK;
    IXMLDOMNode *pChildOut = NULL;
    pParent->appendChild(pChild, &pChildOut);

	//CleanUp:
    if (pChildOut != NULL)
		pChildOut->Release();
    return hr;
}

// Helper that allocates the BSTR param for the caller.
HRESULT CMsXmlWriter::CreateElement(IXMLDOMDocument2 *pXMLDom, PCWSTR wszName, 
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
	//CC - We have to use createNode to be able to set the namespace
    pXMLDom->createElement(bstrName, ppElement);

	//CleanUp:
    SysFreeString(bstrName);
    return hr;
}

// Helper that allocates the BSTR param for the caller.
HRESULT CMsXmlWriter::CreateElement(IXMLDOMDocument2 *pXMLDom, PCWSTR wszName, PCWSTR wszNs, 
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