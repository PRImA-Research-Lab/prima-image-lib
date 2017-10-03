#pragma once

#ifndef OPENCVIMAGE_H
#define OPENCVIMAGE_H

#include "opencv2\opencv.hpp"
#include "afxwin.h"
//#include <pstdint.h>
#include "extrastring.h"
#include "image.h"

//using namespace cv;

namespace PRImA {

class COpenCvGreyScaleImage;
class COpenCvBiLevelImage;
class COpenCvColourImage;
template<class T> class COpenCvImageOps;
class CImageInfo;

/*
 * Class COpenCvImage
 *
 * Base class for images using OpenCV library.
 *
 * CC 01/11/2013 - created
 */
class COpenCvImage
{
public:
	static const int TYPE_AUTO		= 1;
	static const int TYPE_COLOUR	= 2;
	static const int TYPE_GREYSCALE = 3;
	static const int TYPE_BILEVEL	= 4;

protected:
	COpenCvImage(void);
public:
	virtual ~COpenCvImage(void);

public:
	static COpenCvImage * Create(cv::Mat imageData);
	static COpenCvImage * Create(cv::Mat imageData, int enforceType, bool ensurePixelValuesAreInRange = true);
	static COpenCvImage * Create(int type, int width, int height, RGBCOLOUR backColour);
	static COpenCvBiLevelImage		*	CreateB(int width, int height, RGBCOLOUR backColour);
	static COpenCvGreyScaleImage	*	CreateG(int width, int height, RGBCOLOUR backColour);
	static COpenCvColourImage		*	CreateC(int width, int height, RGBCOLOUR backColour);

	virtual COpenCvImage * Clone() = 0;
	void CopyFrom(COpenCvImage * other);

	virtual COpenCvImage * CreateSubImage(int left, int top, int width, int height) = 0; //Creates an image of the same type copying the specified frame.

	void SetData(cv::Mat imageData);

	int GetWidth();
	int GetHeight();

	cv::Mat GetData();

	RGBCOLOUR	GetRGBColor(int x, int y);
	void		SetRGBColor(int x, int y, RGBCOLOUR col);

	static int	CalcMaxValueForColorChannel(cv::Mat data);
	inline void SetMaxValueForColorChannel(int value) { m_MaxValueForColorChannel = value; };

	//HBITMAP				CreateBitmap();
	//inline HBITMAP		GetHBitmap() { if (m_ImageHBitmap==NULL) return CreateBitmap(); else return m_ImageHBitmap; };
	//inline void			ResetHBitmap() { DeleteObject(m_ImageHBitmap); m_ImageHBitmap = 0; };
	//virtual void FillBitmapInfo();

	uint8_t	*	GetGdiCompatiblePixelData();
	void		ResetGdiCompatiblePixelData();

	void				SetImageInfo(CImageInfo * info);
	inline CImageInfo * GetImageInfo() { return m_ImageInfo; };
	void				CopyImageInfo(CImageInfo * other);

	inline CUniString	GetFilePath() { return m_FilePath; };
	inline void			SetFilePath(CUniString filePath) { m_FilePath = filePath; };

	inline CUniString	GetName() { return m_Name; };
	inline void			SetName(CUniString name) { m_Name = name; };

	void Resize(int newWidth, int newHeight);
	void Resize(int originX, int originY, int newWidth, int newHeight);

protected:
	COpenCvImage * CreateSubImage(int left, int top, int width, int height, int type);

protected:
	CUniString		m_FilePath;
	CUniString		m_Name;
	CImageInfo	*	m_ImageInfo;
	cv::Mat				m_Data;
	//BITMAPINFO * m_BitmapInfo;
	//HBITMAP      m_ImageHBitmap;
	uint8_t		*	m_GdiCompatibleData;
	bool			m_OrigImageDataIsGdiCompatible;

	int m_MaxValueForColorChannel;

};


/*
 * Class COpenCvGreyScaleImage
 *
 * Class for grey scale images using OpenCV library.
 */
class COpenCvGreyScaleImage : public COpenCvImage
{
public:
	COpenCvGreyScaleImage(void);
	~COpenCvGreyScaleImage(void);

	COpenCvImage * Clone();
	COpenCvImage * CreateSubImage(int left, int top, int width, int height);

	int		GetGreyLevel(int x, int y);
	int		GetGreyLevel(int x, int y, int borderLevel);
	void	SetGreyLevel(int x, int y, int level);
};


/*
 * Class COpenCvBiLevelImage
 *
 * Class for black-and-white images using OpenCV library.
 */
class COpenCvBiLevelImage : public COpenCvImage
{
public:
	COpenCvBiLevelImage(void);
	~COpenCvBiLevelImage(void);

	COpenCvImage * Clone();
	COpenCvImage * CreateSubImage(int left, int top, int width, int height);

	bool IsBlack(int x, int y);
	bool IsBlack(int x, int y, bool borderValue);
	bool IsWhite(int x, int y);
	bool IsWhite(int x, int y, bool borderValue);

	void SetPixel(int x, int y, bool black);
	void SetBlack(int x, int y);
	void SetWhite(int x, int y);

	void DrawLine(int x1, int y1, int x2, int y2, bool black, bool isothetic = false);

	long CountPixels(int left, int top, int right, int bottom, bool black = true);
	long CountPixels(bool borderValue, int left, int top, int right, int bottom, bool black = true);
	long CountPixels(CRect * rect, bool black = true);
	long CountPixels(vector<CRect *> * rects, bool black = true);
	long CountPixels(bool black, bool forceRefresh = false);

	void And(COpenCvBiLevelImage * b);
	void AndOffset(COpenCvBiLevelImage * b, int offx, int offy);

	void Xor(COpenCvBiLevelImage * b);
	void XorOffset(COpenCvBiLevelImage * b, int offx, int offy);

	void FloodFill(int x, int y, bool black);

private:
	long m_NumberOfBlackPixels;
	long m_NumberOfWhitePixels;
};


/*
 * Class COpenCvColourImage
 *
 * Class for colour images using OpenCV library.
 */
class COpenCvColourImage : public COpenCvImage
{
public:
	COpenCvColourImage(void);
	~COpenCvColourImage(void);

	COpenCvImage * Clone();
	COpenCvImage * CreateSubImage(int left, int top, int width, int height);
};



/*
 * Class CImageInfo
 *
 * Image metadata.
 */
class CImageInfo
{
public:
	CImageInfo();

public:
	int bitsPerPixel;
	float resolutionX;
	float resolutionY;
	//unsigned short resolutionUnit;
};


/*
 * Template COpenCvImageOps
 *
 * Class template for image operations using OpenCV library.
 *
 * Type T: Type used for values in image channels (one channel for B/W and grey scale, three channels for RGB). 
 *         Usually 'uchar' for 8 bit or 'ushort' for 16 bit.
 */
template<class T>
class COpenCvImageOps 
{
public:
	
	static void MiddleThreshold(cv::Mat data);

	static COpenCvImage * DetectColorDepthAndCreateImage(cv::Mat data);

};


} //end namespace



#else

class COpenCvImage;
class COpenCvGreyScaleImage;
class COpenCvBiLevelImage;
class COpenCvColourImage;
template<class T> class COpenCvImageOps;
class CImageInfo;

#endif
