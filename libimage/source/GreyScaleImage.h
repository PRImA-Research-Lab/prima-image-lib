#include "LoColorImage.h"

namespace PRImA 
{

#ifndef GREYSCALEIMAGE_H
#define GREYSCALEIMAGE_H

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif

/*
 * Image class for greyscale images.
 * Deprecated: Use OpenCV image classes
 *
 *
 */
class DllExport CGreyScaleImage : public CLoColorImage
{
	// METHODS
public:
	CGreyScaleImage();

	bool Create(int Width, int Height, RGBCOLOUR BackColour, const char * Name, bool reverse = false);
	CImage* CreateNewImage();
	virtual CImage * Clone(bool reverse = false);

#ifdef _MSC_VER
	void CGreyScaleImage::FillBitmapInfo();
#endif
	RGBCOLOUR GetRGBColor(int x, int y);
	int GetGreyLevel(int x, int y);
	void SetRGBColor(int x, int y, RGBCOLOUR col);
	void SetGreyLevel(int x, int y, int level);
#ifdef GTK
	GdkPixbuf * CreatePixbuf();
#endif // GTK
};

#else
class CGreyScaleImage;
#endif

}