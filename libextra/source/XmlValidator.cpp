#include "XmlValidator.h"

using namespace PRImA;

/*
 * Class CXmlValidator
 *
 * Interface for xml validators. Extends the CValidator interface.
 * 
 * CC 29.03.2010 - created
 */

CXmlValidator::~CXmlValidator()
{
}


/*
 * Class CXmlParseException
 *
 */

CXmlParseException::CXmlParseException(const char* msg)
{
	m_Msg = CUniString(msg);
}

CXmlParseException::CXmlParseException(CUniString msg)
{
	m_Msg = msg;
}

CXmlParseException::~CXmlParseException()
{
}



CXmlValidatorProvider::~CXmlValidatorProvider()
{
}