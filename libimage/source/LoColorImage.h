#ifndef LOCOLORIMAGE_H
#define LOCOLORIMAGE_H

#ifndef _MSC_VER
// Following line is required by C++ standard to enable limits in stdint.h
#define __STDC_LIMIT_MACROS 1
#include <stdint.h>
#endif

#include <stdlib.h>

#include "Image.h"


#ifdef GTK
#include <gdk-pixbuf/gdk-pixbuf.h>
#endif // GTK

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif



namespace PRImA 
{

/*
 * Image class for 8 bit colour images.
 * Deprecated: Use OpenCV image classes
 *
 *
 */
class DllExport CLoColorImage : public PRImA::CImage
{
	// CONSTRUCTION
public:
	CLoColorImage();
	~CLoColorImage();

	// METHODS
public:
	bool Create(int Width, int Height, RGBCOLOUR BackColour, const char * Name, bool reverse = false);
	CImage* CreateNewImage();
	virtual CImage * Clone(bool reverse = false);
	void Clear(RGBCOLOUR colour);

#ifdef GTK
	GdkPixbuf * CreatePixbuf();
#endif // GTK

#ifdef _MSC_VER
	void FillBitmapInfo();
#endif
	inline RGBCOLOUR * GetPalette() { return m_Palette; };
	uint8_t GetPaletteColor(int x, int y);
	RGBCOLOUR GetRGBColor(int x, int y);
	void SetPalette(RGBCOLOUR *);
	void SetPaletteColor(int x, int y, uint8_t col);
	void SetRGBColor(int x, int y, RGBCOLOUR col);

	void DrawLine(int X1, int Y1, int X2, int Y2, uint8_t colour, bool isothetic = false);

	// DATA
protected:
	RGBCOLOUR m_Palette[256];
};

}

#endif // LOCOLORIMAGE_H
