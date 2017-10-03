#pragma once

#include "parameter.h"
#include "extrastring.h"
#include <list>

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif

namespace PRImA 
{

class CReplacementRule;

/*
 * Class CTextFilter
 *
 * Replacement filter for unicode text.
 *
 * CC 03.12.2010
 */

class DllExport CTextFilter
{
public:
	static const int			TYPE_LAYOUT_REGION	= 1;
	static const int			TYPE_TEXT_LINE		= 2;
	static const int			TYPE_WORD			= 3;
	static const int			TYPE_GLYPH			= 4;
public:
	CTextFilter(CParameterMap * replacementRules);
	~CTextFilter(void);

	void ApplyFilter(CUniString & text, int regionType = 0);
	set<CUniString> CheckText(CUniString text);

	static CUniString DecodeUnicodeCharacters(CUniString & chars);

private:
	void ParseFilterRules();
	static int _httoi(const TCHAR *value);

private:
	CParameterMap * m_ReplacementRules;
	list<CReplacementRule*> m_ParesdRules;
};


/*
 * Class CReplacementRule
 *
 * Base class for rules
 */

class DllExport CReplacementRule
{
public:
	virtual ~CReplacementRule();
	void Init(CUniString & fullRule);

	virtual bool Apply(CUniString & text, int regionType = 0) = 0;

	inline bool IsValid() { return m_Valid; };
	inline CUniString GetInfoString() { return m_InfoString; };

protected:
	void ParseRule(CUniString & rule);
	void ParseRightSide(CUniString & rightSideOfRule);
	virtual void ParseLeftSide(CUniString & leftSideOfRule) = 0;
	void ParseTextLevelFilter(CUniString & filter);

protected:
	CUniString m_ToString;		//The string which is used as replacement
	CUniString m_InfoString;	//String describing what is beeing replaced HHHH,HHHH or _SPECIALRULE_

	bool m_ApplyToRegions;
	bool m_ApplyToLines;
	bool m_ApplyToWords;
	bool m_ApplyToGlyphs;

	bool m_Valid;
};


/*
 * Class CSimpleReplacementRule
 *
 * Replace character (sequence) with character (sequence)
 */

class DllExport CSimpleReplacementRule : public CReplacementRule
{
public:
	~CSimpleReplacementRule();

	bool Apply(CUniString & text, int regionType = 0);

protected:
	virtual void ParseLeftSide(CUniString & leftSideOfRule);

protected:
	CUniString m_FromString;	//The search string
};


/*
 * Class CSpecialReplacementRule
 *
 * Replace predefined character entities with character (sequence)
 */

class DllExport CSpecialReplacementRule : public CReplacementRule
{
public:
	static const int SPECIAL_RULE_MULTSPACE		= 1;
	static const int SPECIAL_RULE_STARTSPACE	= 2;
	static const int SPECIAL_RULE_ENDSPACE		= 3;
	static const int SPECIAL_RULE_MULTBREAK		= 4;
	static const int SPECIAL_RULE_STARTBREAK	= 5;
	static const int SPECIAL_RULE_ENDBREAK		= 6;

public:
	~CSpecialReplacementRule();

	bool Apply(CUniString & text, int regionType = 0);

protected:
	virtual void ParseLeftSide(CUniString & leftSideOfRule);

private:
	int m_RuleType;
};

}