#include "stdafx.h"
#include <msxml6.h>
#include "ExtraString.h"
#include <vector>

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif


namespace PRImA 
{

#ifndef MSXMLNODE_H
#define MSXMLNODE_H

/*
 * Class CMsXmlNode
 *
 * Wrapper for IXMLDOMElement of MSXML.
 *
 * CC 24.03.2010
 */

class DllExport CMsXmlNode
{
public:
	CMsXmlNode();
	CMsXmlNode(IXMLDOMElement * nodeObject, IXMLDOMDocument2 * xmlDoc);
	~CMsXmlNode();

	void			SetNodeObject(IXMLDOMElement * nodeObject, IXMLDOMDocument2 * xmlDoc);

	CUniString		GetName();
	bool			HasAttribute(const wchar_t * attrName);
	CUniString		GetAttribute(const wchar_t * attrName);
	int				GetIntAttribute(const wchar_t * attrName);
	double			GetDoubleAttribute(const wchar_t * attrName);
	bool			GetBoolAttribute(const wchar_t * attrName);

	void			AddAttribute(const wchar_t * attrName, CUniString value);
	void			AddAttribute(const wchar_t * attrName, int value);
	void			AddAttribute(const wchar_t * attrName, double value);
	void			AddAttribute(const wchar_t * attrName, bool value);
	CMsXmlNode   *	AddChildNode(const wchar_t * nodeName);
	void			AddTextNode(const wchar_t * nodeName, CUniString value);

	bool			HasChildren();
	CMsXmlNode	*	GetFirstChild();
	CMsXmlNode	*	GetNextSibling();

	CUniString		GetTextContent();

	void			DeleteChildNode(CMsXmlNode * child);

	inline void		SetNameSpace(wchar_t * ns) { m_NameSpace = ns; };

	inline CUniString GetNameSpace() { return m_NameSpace != NULL ?  CUniString(m_NameSpace) : CUniString(); };

private:
	HRESULT CreateAndAddAttributeNode(IXMLDOMDocument2 *pDom, PCWSTR wszName, PCWSTR wszValue, 
											  IXMLDOMElement *pParent);
	HRESULT VariantFromString(PCWSTR wszValue, VARIANT &Variant);
	HRESULT CreateAndAddElementNode(IXMLDOMDocument2 *pDom, PCWSTR wszName, 
									PCWSTR wszNs, PCWSTR wszNewline, 
									IXMLDOMNode *pParent, IXMLDOMElement **ppElement = NULL);
	HRESULT CreateElement(IXMLDOMDocument2 *pXMLDom, PCWSTR wszName, 
							IXMLDOMElement **ppElement);
	HRESULT CreateElement(IXMLDOMDocument2 *pXMLDom, PCWSTR wszName, PCWSTR wszNs, 
							IXMLDOMElement **ppElement);
	HRESULT AppendChildToParent(IXMLDOMNode *pChild, IXMLDOMNode *pParent);
	HRESULT RemoveChildFromParent(IXMLDOMNode *pChild, IXMLDOMNode *pParent);
	HRESULT CreateAndAddTextNode(IXMLDOMDocument2 *pDom, PCWSTR wszText, IXMLDOMNode *pParent);

private:
	IXMLDOMElement	*	m_NodeObject;
	CMsXmlNode		*	m_CurrChild;
	CMsXmlNode		*	m_CurrSibling;
	CUniString		*	m_Name;
	IXMLDOMDocument2 *	m_XmlDoc;
	wchar_t			*	m_NameSpace;
	vector<CMsXmlNode*> * m_Children;

};



#else
class CMsXmlNode;
#endif

}