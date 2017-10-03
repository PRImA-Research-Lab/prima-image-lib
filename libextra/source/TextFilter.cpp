#include "TextFilter.h"
#include <vector>

namespace PRImA
{

/*
 * Class CTextFilter
 *
 * Replacement filter for Unicode text.
 *
 * CC 03.12.2010 - created
 * CC 25.03.2011 - added special rules and text level filter
 */

/*
 * Constructor
 */
CTextFilter::CTextFilter(CParameterMap * replacementRules)
{
	m_ReplacementRules = replacementRules;
	ParseFilterRules();
}

/*
 * Destructor
 */
CTextFilter::~CTextFilter(void)
{
	list<CReplacementRule*>::iterator it = m_ParesdRules.begin();
	while (it != m_ParesdRules.end())
	{
		delete (*it);
		it++;
	}
	delete m_ReplacementRules;
}

/*
 * Apply the replacement rules to the given text
 */
void CTextFilter::ApplyFilter(CUniString & text, int regionType /*=0*/)
{
	list<CReplacementRule*>::iterator it = m_ParesdRules.begin();
	while (it != m_ParesdRules.end())
	{
		(*it)->Apply(text, regionType);
		it++;
	}
}

/*
 * Checks if the given text contains characters that would be changed when
 * applying the filter.
 * Returns a set of strings in format "HHHH,HHHH,HHHH" or "_SPECIALRULE_".
 */
set<CUniString> CTextFilter::CheckText(CUniString text)
{
	set<CUniString> res;
	list<CReplacementRule*>::iterator it = m_ParesdRules.begin();
	while (it != m_ParesdRules.end())
	{
		if ((*it)->Apply(text))
			res.insert((*it)->GetInfoString());
		it++;
	}
	return res;
}

/*
 * Parses the rules from the parameter map and stores them as an internal list.
 */
void CTextFilter::ParseFilterRules()
{
	m_ReplacementRules->SetSorted(true);
	for (int i=0; i<m_ReplacementRules->GetSize(); i++)
	{
		//Parse rule
		CParameter * param = m_ReplacementRules->Get(i);
		if (param == NULL || param->GetType() != CParameter::TYPE_STRING)
			continue;
		CStringParameter * ruleParam = (CStringParameter*)param;
		CUniString rule = ruleParam->GetValue();

		CReplacementRule * ruleObject = NULL;
		if (rule.Find(_T("_")) >= 0) //Special rule
			ruleObject = new CSpecialReplacementRule();
		else
			ruleObject = new CSimpleReplacementRule();
		ruleObject->Init(rule);

		if (!ruleObject->IsValid())
		{
			delete ruleObject;
			continue;
		}

		//Put rule into list
		m_ParesdRules.push_back(ruleObject);
	}
}

/*
 * Converts a string in format "HHHH,HHHH,HHHH" to a normal Unicode string.
 * HHHH stands for a 4 digit hexadecimal number representing a Unicode character.
 */
CUniString CTextFilter::DecodeUnicodeCharacters(CUniString & chars)
{
	CUniString result;

	vector<CUniString> hexCodes;
	chars.Split(_T(","), hexCodes);
	for (unsigned int i=0; i<hexCodes.size(); i++)
	{
		wchar_t * wbuffer = new wchar_t[2];
		wbuffer[0] = _httoi(hexCodes[i].GetBuffer());
		wbuffer[1] = 0;
		CUniString character(wbuffer);
		delete [] wbuffer;
		result.Append(character);
	}
	return result;
}

/*
 * Converts an hexadecimal string (e.g. 0xB3CD or 3A41) to an integer.
 * See http://www.codeproject.com/KB/string/hexstrtoint.aspx
 */
int CTextFilter::_httoi(const TCHAR *value)
{
  struct CHexMap
  {
    TCHAR chr;
    int value;
  };
  const int HexMapL = 16;
  CHexMap HexMap[HexMapL] =
  {
    {'0', 0}, {'1', 1},
    {'2', 2}, {'3', 3},
    {'4', 4}, {'5', 5},
    {'6', 6}, {'7', 7},
    {'8', 8}, {'9', 9},
    {'A', 10}, {'B', 11},
    {'C', 12}, {'D', 13},
    {'E', 14}, {'F', 15}
  };
  TCHAR *mstr = _tcsupr(_tcsdup(value));
  TCHAR *s = mstr;
  int result = 0;
  if (*s == '0' && *(s + 1) == 'X') s += 2;
  bool firsttime = true;
  while (*s != '\0')
  {
    bool found = false;
    for (int i = 0; i < HexMapL; i++)
    {
      if (*s == HexMap[i].chr)
      {
        if (!firsttime) result <<= 4;
        result |= HexMap[i].value;
        found = true;
        break;
      }
    }
    if (!found) break;
    s++;
    firsttime = false;
  }
  free(mstr);
  return result;
}


/*
 * Class CReplacementRule
 *
 * Base class for rules
 */

void CReplacementRule::Init(CUniString & fullRule)
{
	//By default all text elemeny levels are targeted
	m_ApplyToRegions = true;
	m_ApplyToLines = true;
	m_ApplyToWords = true;
	m_ApplyToGlyphs = true;

	//Parse rule
	m_Valid = true;
	ParseRule(fullRule);
}

/*
 * Destructor
 */
CReplacementRule::~CReplacementRule()
{
}

/*
 * Parses a replacement rule from the given string representation
 */
void CReplacementRule::ParseRule(CUniString & rule)
{
	vector<CUniString> splitRule;
	rule.Split(_T(":="), splitRule);
	if (splitRule.size() != 2)	//Wrong rule format
	{
		m_Valid = false;
		return;
	}
	ParseLeftSide(splitRule[0]);
	if (m_Valid)
		ParseRightSide(splitRule[1]);
}

/*
 * Parses the right side of a replacment rule
 */
void CReplacementRule::ParseRightSide(CUniString & rightSideOfRule)
{
	CUniString encodedChars;
	if (rightSideOfRule.Find(_T("|")) >= 0) //contains text element level filter
	{
		vector<CUniString> splitRule;
		rightSideOfRule.Split(_T("|"), splitRule);
		if (splitRule.size() != 2) //Wrong rule format
		{
			m_Valid = false;
			return;
		}
		encodedChars = splitRule[0];
		ParseTextLevelFilter(splitRule[1]); //Filter for region,line,word,glyph
	}
	else
		encodedChars = rightSideOfRule;

	m_ToString = CTextFilter::DecodeUnicodeCharacters(encodedChars);
}

/*
 * Parses the text level (region/line/word/glyph) from the given string representation
 */
void CReplacementRule::ParseTextLevelFilter(CUniString & filter)
{
	m_ApplyToRegions	= filter.Find(_T("R")) >= 0 || filter.Find(_T("r")) >= 0;
	m_ApplyToLines		= filter.Find(_T("L")) >= 0 || filter.Find(_T("l")) >= 0;
	m_ApplyToWords		= filter.Find(_T("W")) >= 0 || filter.Find(_T("w")) >= 0;
	m_ApplyToGlyphs		= filter.Find(_T("G")) >= 0 || filter.Find(_T("g")) >= 0;
}


/*
 * Class CSimpleReplacementRule
 *
 * Replace character (sequence) with character (sequence)
 */

/*
 * Destructor
 */
CSimpleReplacementRule::~CSimpleReplacementRule()
{
}

/*
 * Applies this rule to the given text.
 *
 * Returns true, if the text has been changed.
 */
bool CSimpleReplacementRule::Apply(CUniString & text, int regionType /*=0*/)
{
	if (regionType == 0 
		|| regionType == CTextFilter::TYPE_LAYOUT_REGION && m_ApplyToRegions
		|| regionType == CTextFilter::TYPE_TEXT_LINE && m_ApplyToLines
		|| regionType == CTextFilter::TYPE_WORD && m_ApplyToWords
		|| regionType == CTextFilter::TYPE_GLYPH && m_ApplyToGlyphs)
	{
		return CExtraString::repl(text, m_FromString, m_ToString) > 0;
	}
	return false;
}

/*
 * Parses the left side of the string representation of a rule
 */
void CSimpleReplacementRule::ParseLeftSide(CUniString & leftSideOfRule)
{
	m_InfoString = leftSideOfRule;
	m_FromString = CTextFilter::DecodeUnicodeCharacters(leftSideOfRule);
}


/*
 * Class CSpecialReplacementRule
 *
 * Replace predefined character entities with character (sequence)
 */

/*
 * Destructor
 */
CSpecialReplacementRule::~CSpecialReplacementRule()
{
}

/*
 * Applies this rule to the given text.
 *
 * Returns true, if the text has been changed.
 */
bool CSpecialReplacementRule::Apply(CUniString & text, int regionType /*=0*/)
{
	if (regionType == 0 
		|| regionType == CTextFilter::TYPE_LAYOUT_REGION && m_ApplyToRegions
		|| regionType == CTextFilter::TYPE_TEXT_LINE && m_ApplyToLines
		|| regionType == CTextFilter::TYPE_WORD && m_ApplyToWords
		|| regionType == CTextFilter::TYPE_GLYPH && m_ApplyToGlyphs)
	{
		if (m_RuleType == SPECIAL_RULE_MULTSPACE)
		{
			//First replace all occurances of three consecutive spaces with two spaces (repeat until no more change)
			while (CExtraString::repl(text, _T("   "), _T("  ")) != 0)
				;
			//Now replace all occurances of two spaces with the replacement string
			return CExtraString::repl(text, _T("  "), m_ToString) > 0;
		}
		else if (m_RuleType == SPECIAL_RULE_MULTBREAK)
		{
			//First replace all occurances of three consecutive breaks with two breaks (repeat until no more change)
			while (CExtraString::repl(text, _T("\r\n\r\n\r\n"), _T("\r\n\r\n")) != 0)
				;
			//Now replace all occurances of two breaks with the replacement string
			return CExtraString::repl(text, _T("\r\n\r\n"), m_ToString) > 0;
		}
		else if (m_RuleType == SPECIAL_RULE_STARTSPACE)
		{
			if (text.Find(_T(" ")) == 0) //Space at beginning
			{
				text.Delete(0, 1);
				text.Insert(0, m_ToString);
				return true;
			}
		}
		else if (m_RuleType == SPECIAL_RULE_STARTBREAK)
		{
			if (text.Find(_T("\r\n")) == 0) //Break at beginning
			{
				text.Delete(0, 2);
				text.Insert(0, m_ToString);
				return true;
			}
		}
		else if (m_RuleType == SPECIAL_RULE_ENDSPACE)
		{
			if (text.EndsWith(_T(" "))) //Space at end
			{
				text.Truncate(text.GetLength()-1);
				text.Append(m_ToString);
				return true;
			}
		}
		else if (m_RuleType == SPECIAL_RULE_ENDBREAK)
		{
			if (text.EndsWith(_T("\r\n"))) //Break at end
			{
				text.Truncate(text.GetLength()-2);
				text.Append(m_ToString);
				return true;
			}
		}
	}
	return false;
}

/*
 * Parses the left side of the string representation of a special rule
 */
void CSpecialReplacementRule::ParseLeftSide(CUniString & leftSideOfRule)
{
	m_InfoString = leftSideOfRule;
	if (leftSideOfRule == _T("_MULTSPACE_"))
		m_RuleType = SPECIAL_RULE_MULTSPACE;
	else if (leftSideOfRule == _T("_STARTSPACE_"))
		m_RuleType = SPECIAL_RULE_STARTSPACE;
	else if (leftSideOfRule == _T("_ENDSPACE_"))
		m_RuleType = SPECIAL_RULE_ENDSPACE;
	else if (leftSideOfRule == _T("_MULTBREAK_"))
		m_RuleType = SPECIAL_RULE_MULTBREAK;
	else if (leftSideOfRule == _T("_STARTBREAK_"))
		m_RuleType = SPECIAL_RULE_STARTBREAK;
	else if (leftSideOfRule == _T("_ENDBREAK_"))
		m_RuleType = SPECIAL_RULE_ENDBREAK;
}


} //end namespace