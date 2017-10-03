#pragma once

#include "opencv2/opencv.hpp"
#include "opencvimage.h"
#include "stdafx.h"
//#include <gdiplus.h>

//using  Gdiplus::Graphics;

//using namespace cv;

namespace PRImA {

/*
 * Class COpenCvImageRenderer
 *
 * GDI rendering of images based on OpenCV library.
 *
 * CC 01/11/2013 - created
 */
class COpenCvImageRenderer
{
public:
	COpenCvImageRenderer(void);
	~COpenCvImageRenderer(void);

	void Render(COpenCvImage * img, CDC * dc/*, Graphics graphics*/);
	void Render(COpenCvImage * img, CDC * dc, 
				int xOriginDest, int yOriginDest, int widthDest, int heightDest,
				int xOriginSource, int yOriginSource, int widthSource, int heightSource,
				int zoomFactor = 100,
				DWORD operation = SRCCOPY,
				DWORD stretchMode = COLORONCOLOR);

private:
	void FillBitmapInfo(BITMAPINFO * bmi, int width, int height, int bpp, int origin);
	static int Bpp(cv::Mat img);
};

} //end namespace