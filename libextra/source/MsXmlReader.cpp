#include <afxcoll.h>
#include "MsXmlReader.h"
#include "ExtraFileHelper.h"

using namespace PRImA;

/*
 * Class CMsXmlReader
 *
 * Helper class to open and parse XML files using MSXML.
 *
 * CC 29.03.2010 - created
 */


/*
 * Constructor for loading from file
 */
CMsXmlReader::CMsXmlReader(CUniString fileName)
{
	m_XmlDoc = NULL;
	m_XmlErr = NULL;
	m_BstrXML = NULL;
	m_BstrErr = NULL;

	long res = CoInitialize(NULL);

    if(SUCCEEDED(res))
	{
		res = S_OK;

		VariantInit(&m_VarFileName);
    
		if (CreateAndInitDOM(&m_XmlDoc) == S_OK)
		{
			//Allow DTD (required for HOCR format)
			VARIANT val;
			VariantInit(&val);
			val.boolVal = false;
			HRESULT r = m_XmlDoc->setProperty((BSTR)L"ProhibitDTD", val);
			if (r != S_OK)
				return;

			if (VariantFromString(fileName.GetBuffer(), m_VarFileName) == S_OK)
			{
				if (m_XmlDoc->load(m_VarFileName, &m_VarStatus) == S_OK)
				{
					//nothing to do
				}
				else //Error (done in GetParseError())
				{
				}
			}
		}
	}
}

/*
 * Cosntructor for loading from file with decryption
 * 'crypto' - Optional for decryption
 */
CMsXmlReader::CMsXmlReader(CUniString filename, CCrypto * crypto)
{
	m_XmlDoc = NULL;
	m_XmlErr = NULL;
	m_BstrXML = NULL;
	m_BstrErr = NULL;

	long res = CoInitialize(NULL);

	if (SUCCEEDED(res))
	{
		res = S_OK;

		//Load file
		CUniString xmlContent;

		if (crypto != NULL)
		{
			CByteArray arBytes;
			bool decrypted = false;
			if (CExtraFileHelper::ReadFromFile(filename, &arBytes))
				decrypted = crypto->Decrypt(arBytes, xmlContent);
		}
		else //No crypto
		{
			CExtraFileHelper::ReadFromFile(filename, xmlContent);
		}
		
		VariantInit(&m_VarFileName);

		if (CreateAndInitDOM(&m_XmlDoc) == S_OK)
		{
			//Allow DTD (required for HOCR format)
			VARIANT val;
			VariantInit(&val);
			val.boolVal = false;
			HRESULT r = m_XmlDoc->setProperty((BSTR)L"ProhibitDTD", val);
			if (r == S_OK)
			{
				//Read
				BSTR bstrXmlContent = SysAllocString(xmlContent.GetBuffer());

				if (m_XmlDoc->loadXML(bstrXmlContent, &m_VarStatus) == S_OK)
				{
					//nothing to do
				}
				else //Error (done in GetParseError())
				{
				}

				SysFreeString(bstrXmlContent);
			}
		}
	}
}

/*
 * Destructor
 */
CMsXmlReader::~CMsXmlReader(void)
{
    if (m_XmlDoc != NULL)
		m_XmlDoc->Release();
	m_XmlDoc = NULL;
    if (m_XmlErr != NULL)
		m_XmlErr->Release();
	m_XmlErr = NULL;
    SysFreeString(m_BstrXML);
    SysFreeString(m_BstrErr);
    VariantClear(&m_VarFileName);

	CoUninitialize();
}

/*
 * Returns the XML document object.
 *
 * 'passPointerResponsibility' - Sets the internal variable holding the document to NULL.
 *                               The responsibility to free the document falls to the caller.
 */
IXMLDOMDocument2 * CMsXmlReader::GetXmlDoc(bool passPointerResponsibility /*= false*/)
{
	IXMLDOMDocument2 * ret = m_XmlDoc;
	if (passPointerResponsibility)
		m_XmlDoc = NULL;
	if (m_VarStatus == VARIANT_TRUE)
		return ret;
	return NULL;
}

/*
 * Composes an error message from the last XML parsing error (including reason and line number)
 */
CUniString CMsXmlReader::GetParseError()
{
	CUniString msg;
	if (m_XmlDoc != NULL)
	{
		long result = m_XmlDoc->get_parseError(&m_XmlErr);
		if (result == S_OK)
		{
			result = m_XmlErr->get_reason(&m_BstrErr);
			if (result == S_OK)
				msg.Append(m_BstrErr);
			long lineNumber = 0;
			result = m_XmlErr->get_line(&lineNumber);
			if (result == S_OK)
			{
				msg.Append(_T("\nLine: "));
				msg.Append((int)lineNumber);
			}
		}
		m_XmlDoc->get_parseError(&m_XmlErr); //CC: What is that for?
	}
	return msg;
}

// Helper function to create a VT_BSTR variant from a null terminated string. 
HRESULT CMsXmlReader::VariantFromString(PCWSTR wszValue, VARIANT &Variant)
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

// Helper function to create a DOM instance. 
HRESULT CMsXmlReader::CreateAndInitDOM(IXMLDOMDocument2 **ppDoc)
{
    HRESULT hr = CoCreateInstance(__uuidof(DOMDocument60), NULL, 
								CLSCTX_INPROC_SERVER, IID_PPV_ARGS(ppDoc));
    if (SUCCEEDED(hr))
    {
        // these methods should not fail so don't inspect result
        (*ppDoc)->put_async(VARIANT_FALSE);  
        (*ppDoc)->put_validateOnParse(VARIANT_FALSE);
        (*ppDoc)->put_resolveExternals(VARIANT_FALSE);
    }
    return hr;
}

