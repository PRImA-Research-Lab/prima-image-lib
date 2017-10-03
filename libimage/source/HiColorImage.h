#include "Image.h"

#include "BiLevelImage.h"
//#include "Compatibility.h"

#define POS_BLUE   2
#define POS_GREEN  1
#define POS_RED    0
#define POS_ALPHA  3

#ifndef HICOLORIMAGE_H
#define HICOLORIMAGE_H

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
 * Image class for 24/32 bit colour images.
 * Deprecated: Use OpenCV image classes
 *
 *
 */
class DllExport CHiColorImage : public PRImA::CImage
{
	// CONSTRUCTION
public:
	CHiColorImage();
	~CHiColorImage();

	// METHODS
public:
	void AndOffset(CBiLevelImage * b, int offx, int offy);
	bool Create(int Width, int Height, RGBCOLOUR BackColour, const char * Name, bool reverse = false);
	CImage* CreateNewImage();
	virtual CImage * Clone(bool reverse = false);
#ifdef GTK
	GdkPixbuf * CreatePixbuf();
#endif // GTK
	void DrawLine(int x1, int y1, int x2, int y2);
	void DrawLine(int x1, int y1, int x2, int y2, int Width);
	void DrawRect(int x1, int y1, int x2, int y2);

	
	void Clear(RGBCOLOUR colour);
	
#ifdef _MSC_VER
	void FillBitmapInfo();
#endif
	int  GetBaselineDist();
	RGBCOLOUR GetRGBColor(int x, int y);
	bool operator=(CImage&);
	void SetForegroundColor(RGBCOLOUR col);
	void SetRGBColor(int x, int y, RGBCOLOUR col);

	inline uint8_t GetR(int x, int y)
		{return m_pLineArray[y][x*4+POS_RED];}
	inline uint8_t GetG(int x, int y)
		{return m_pLineArray[y][x*4+POS_GREEN];}
	inline uint8_t GetB(int x, int y)
		{return m_pLineArray[y][x*4+POS_BLUE];}
	inline uint8_t GetA(int x, int y)
		{return m_pLineArray[y][x*4+POS_ALPHA];}

private:
	inline void PrivateSetR(int x, int y, unsigned char val)
		{m_pLineArray[y][x*4+POS_RED] = val;}
	inline void PrivateSetG(int x, int y, unsigned char val)
		{m_pLineArray[y][x*4+POS_GREEN] = val;}
	inline void PrivateSetB(int x, int y, unsigned char val)
		{m_pLineArray[y][x*4+POS_BLUE] = val;}
	inline void PrivateSetA(int x, int y, unsigned char val)
		{m_pLineArray[y][x*4+POS_ALPHA] = val;}
};

}

#else

namespace PRImA 
{
class CHiColorImage;
}
#endif
