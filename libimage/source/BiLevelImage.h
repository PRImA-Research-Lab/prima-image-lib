#ifndef BILEVELIMAGE_H
#define BILEVELIMAGE_H

#ifndef _MSC_VER
#include <stdint.h>
#endif

#include "Image.h"
#include "ConnectedComponent.h"
#include "Run.h"

//#include "Compatibility.h"
#include <vector>
#include <queue>

#ifdef GTK
#include <gdk-pixbuf/gdk-pixbuf.h>
#endif // GTK

/*#ifdef _MSC_VER
#define WHITE 0x01
#define BLACK 0x00
#else
#define WHITE 0x00
#define BLACK 0xFF
#endif*/

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif

using namespace std;

namespace PRImA 
{
/*
 * Image class for black and white images.
 * Deprecated: Use OpenCV image classes
 *
 * CC 02.11.2009 - optimized the flood fill algorithm
 *
 */
class DllExport CBiLevelImage : public PRImA::CImage
{
public:
	static const int WHITE = 0x01;
	static const int BLACK = 0x00;
	// CONSTRUCTION
public:
	CBiLevelImage();
	CBiLevelImage(CBiLevelImage* img);
	~CBiLevelImage();

	// METHODS
public:
	void And(CBiLevelImage * B);
	void AndOffset(CBiLevelImage * B, int xoff, int yoff);
	void Clear(RGBCOLOUR colour);
	void ClearLine(int X1, int Y1, int X2, int Y2, bool isothetic = false);
	bool Create(int Width, int Height, RGBCOLOUR BackColour, const char * Name, bool reverse = false);
	CImage * CBiLevelImage::CreateNewImage();
	//CImage* CreateSubImage(int left, int top, int width, int height); //Creates an image of the same type copying the specified frame.
	virtual CImage * Clone(bool reverse = false);
	virtual CImage * Clone(bool reverse, bool flipVertically);
#ifdef GTK
	GdkPixbuf * CreatePixbuf();
#endif // GTK
	//bool DetectBaselineDist();
#ifdef _MSC_VER
	void FillBitmapInfo();
#endif
	void FloodFill(int x, int y, bool Black);
	void Frame(int FrameWidth, bool White);
	int  GetBaselineDist();
	RGBCOLOUR GetRGBColor(int x, int y);
	bool GetValue(int x, int y);
	bool IsBlack(int x, int y);
	bool IsWhite(int x, int y);
	bool ModifiedKFill(int KVal = 3);
	void SetBlack(const int x, const int y);
	void SetPixel(const int x, const int y, bool black);
	void SetLine(int X1, int Y1, int X2, int Y2);
	void SetLine(int X1, int Y1, int X2, int Y2, bool black);
	void SetRGBColor(int x, int y, RGBCOLOUR colour);
	void SetWhite(const int x, const int y);
	void Smear(int SmearValue);
	CBiLevelImage & operator=(CImage & Other);
	CBiLevelImage & operator=(CBiLevelImage & Other);
	void UnPackBitArray(uint8_t * BitArray);

	long CountPixels(int left, int top, int right, int bottom, bool black = true);
	long CountPixels(CRect * rect, bool black = true);
	long CountPixels(vector<CRect *> * rects, bool black = true);
	long CountPixels(bool black);
	void DrawComponent(CConnectedComponent* comp);

private:
	long m_NumberOfBlackPixels;
	long m_NumberOfWhitePixels;
};

}

#endif // BILEVELIMAGE_H
