#include "FontCache.h"

using Gdiplus::FontStyleRegular;
using Gdiplus::UnitPixel;
using Gdiplus::UnitPoint;
using Gdiplus::FontCollection;

namespace PRImA
{

/*
 * Class CFontCache
 *
 * Static cache for fonts used in Aletheia
 *
 * CC 05.10.2011 - created
 */

const wchar_t * CFontCache::FONT_ALETHEIA_SANS	= _T("Aletheia Sans");
const wchar_t * CFontCache::FONT_ARIAL			= _T("Arial");
const wchar_t * CFontCache::FONT_MS_SHELL_DLG	= L"MS Shell Dlg";

map<CUniString, map<int, CFont *> * > CFontCache::s_NormalFonts;
map<CUniString, map<int, CFont *> * > CFontCache::s_BoldFonts;
map<CUniString, map<double, Gdiplus::Font *> * > CFontCache::s_GdiPlusFonts;
CCriticalSection CFontCache::s_CriticalSect;
PrivateFontCollection * CFontCache::s_GdiPlusFontCollection = NULL;
set<CUniString> CFontCache::s_PrivateGdiPlusFonts;

/*
 * Constructor
 */	
CFontCache::CFontCache(void)
{
}

/*
 * Destructor
 */	
CFontCache::~CFontCache(void)
{
}

/*
 * Initialises a font collection for GDI+ fonts (thread-safe)
 */
void CFontCache::InitPrivateFontCollection()
{
	CSingleLock lock(&s_CriticalSect, TRUE);
	if (s_GdiPlusFontCollection == NULL)
		s_GdiPlusFontCollection = new PrivateFontCollection();
	lock.Unlock();
}

/*
 * Adds non-installed font from a file for usage with GDI+
 */
void CFontCache::AddPrivateGdiPlusFont(CUniString fontName, CUniString filepath)
{
	InitPrivateFontCollection();
	s_GdiPlusFontCollection->AddFontFile(filepath.GetBuffer());
	s_PrivateGdiPlusFonts.insert(fontName);
}

/*
 * Deletes all cached fonts
 */
void CFontCache::DeleteFonts()
{
	CSingleLock lock(&s_CriticalSect, TRUE);
	for (map<CUniString, map<int, CFont *> * >::iterator it = s_NormalFonts.begin(); it != s_NormalFonts.end(); it++)
	{
		map<int, CFont *> * map2 = (*it).second;
		for (map<int, CFont *>::iterator it2 = map2->begin(); it2 != map2->end(); it2++)
		{
			(*it2).second->DeleteObject();
			delete (*it2).second;
		}
		delete map2;
	}
	for (map<CUniString, map<int, CFont *> * >::iterator it = s_BoldFonts.begin(); it != s_BoldFonts.end(); it++)
	{
		map<int, CFont *> * map2 = (*it).second;
		for (map<int, CFont *>::iterator it2 = map2->begin(); it2 != map2->end(); it2++)
		{
			(*it2).second->DeleteObject();
			delete (*it2).second;
		}
		delete map2;
	}
	for (map<CUniString, map<double, Gdiplus::Font *> * >::iterator it3 = s_GdiPlusFonts.begin(); it3 != s_GdiPlusFonts.end(); it3++)
	{
		map<double, Gdiplus::Font *> * map2 = (*it3).second;
		for (map<double, Gdiplus::Font *>::iterator it4 = map2->begin(); it4 != map2->end(); it4++)
		{
			delete (*it4).second;
		}
		delete map2;
	}
	lock.Unlock();
}

/*
 * Returns a font of the given name (see CFontCache::FONT_...) and height (in pixel).
 */
CFont * CFontCache::GetFont(CUniString id, int height, bool bold /*= false*/)
{
	CSingleLock lock(&s_CriticalSect, TRUE);

	map<CUniString, map<int, CFont *> * > & fonts = bold ? s_BoldFonts : s_NormalFonts;

	//Id
	map<CUniString, map<int, CFont *> * >::iterator itId = fonts.find(id);
	map<int, CFont *> * heightMap = NULL;
	if (itId == fonts.end())
	{
		heightMap = new map<int, CFont *>();
		fonts.insert(pair<CUniString, map<int, CFont *> *>(id, heightMap));
	}
	else
		heightMap = (*itId).second;

	//Size
	map<int, CFont *>::iterator itHeight = heightMap->find(height);
	CFont * font = NULL;
	if (itHeight == heightMap->end())
	{
		//Create font
		font = new CFont();
		font->CreateFont(
				   height,                    // nHeight
				   0,                         // nWidth
				   0,                         // nEscapement
				   0,                         // nOrientation
				   bold ? FW_BOLD : FW_NORMAL,// nWeight
				   FALSE,                     // bItalic
				   FALSE,                     // bUnderline
				   0,                         // cStrikeOut
				   ANSI_CHARSET,              // nwchar_tSet
				   OUT_DEFAULT_PRECIS,        // nOutPrecision
				   CLIP_DEFAULT_PRECIS,       // nClipPrecision
				   DEFAULT_QUALITY,           // nQuality
				   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				   id);						  // lpszFacename

		heightMap->insert(pair<int, CFont*>(height, font));
	}
	else
		font = (*itHeight).second;

	lock.Unlock();
	return font;
}


/*
 * Returns a font of the given name (see CFontCache::FONT_...) and size (in pixel or point).
 *
 * 'unitIsPoint' - If true, the size is interpreted as point, otherwise as pixel
 */
Gdiplus::Font * CFontCache::GetGdiPlusFont(CUniString id, double size, bool unitIsPoint /*= true*/)
{
	InitPrivateFontCollection();

	CSingleLock lock(&s_CriticalSect, TRUE);
	//Id
	map<CUniString, map<double, Gdiplus::Font *> * >::iterator itId = s_GdiPlusFonts.find(id);
	map<double, Gdiplus::Font *> * heightMap = NULL;
	if (itId == s_GdiPlusFonts.end())
	{
		heightMap = new map<double, Gdiplus::Font *>();
		s_GdiPlusFonts.insert(pair<const wchar_t *, map<double, Gdiplus::Font *> *>(id, heightMap));
	}
	else
		heightMap = (*itId).second;

	//Size
	map<double, Gdiplus::Font *>::iterator itHeight = heightMap->find(size);
	Gdiplus::Font * font = NULL;
	if (itHeight == heightMap->end())
	{
		//Is the font a private one?
		set<CUniString>::iterator itPrivate = s_PrivateGdiPlusFonts.find(id);
		if (itPrivate != s_PrivateGdiPlusFonts.end()) //Private
		{
			//Create font
			if (unitIsPoint)
				font = new Gdiplus::Font(id, (Gdiplus::REAL)size, FontStyleRegular, UnitPoint, s_GdiPlusFontCollection);
			else
				font = new Gdiplus::Font(id, (Gdiplus::REAL)size, FontStyleRegular, UnitPixel, s_GdiPlusFontCollection);
		}
		else //Normal system font
		{
			//Create font
			if (unitIsPoint)
				font = new Gdiplus::Font(id, (Gdiplus::REAL)size, FontStyleRegular);
			else
				font = new Gdiplus::Font(id, (Gdiplus::REAL)size, FontStyleRegular, UnitPixel);
		}
		heightMap->insert(pair<double, Gdiplus::Font*>(size, font));
	}
	else
		font = (*itHeight).second;

	lock.Unlock();
	return font;
}

} //end namespace