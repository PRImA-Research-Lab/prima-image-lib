#pragma once

#include <map>
#include <set>
#include "afxwin.h"
#include <afxmt.h>
#include "extrastring.h"
#include <gdiplus.h>

using namespace std;
using Gdiplus::PrivateFontCollection;

namespace PRImA
{

/*
 * Class CFontCache
 *
 * Static cache for fonts used in Aletheia
 *
 * CC 05.10.2011 - created
 */

class CFontCache
{
public:
	static const wchar_t * FONT_ALETHEIA_SANS;
	static const wchar_t * FONT_ARIAL;
	static const wchar_t * FONT_MS_SHELL_DLG;

private:
	CFontCache(void);
	~CFontCache(void);

	static void InitPrivateFontCollection();

public:
	static void AddPrivateGdiPlusFont(CUniString fontName, CUniString filepath);
	static void DeleteFonts();
	static CFont * GetFont(CUniString id, int height, bool bold = false);
	static Gdiplus::Font * GetGdiPlusFont(CUniString id, double size, bool unitIsPoint = true);

private:
	static map<CUniString, map<int, CFont *> * > s_NormalFonts;
	static map<CUniString, map<int, CFont *> * > s_BoldFonts;
	static map<CUniString, map<int, CFont *> * > s_Fonts;
	static map<CUniString, map<double, Gdiplus::Font *> * > s_GdiPlusFonts;

	static CCriticalSection	s_CriticalSect;

	static PrivateFontCollection * s_GdiPlusFontCollection;
	static set<CUniString> s_PrivateGdiPlusFonts;
};


} //end namespace