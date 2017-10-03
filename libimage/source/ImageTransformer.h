#pragma once

#include "opencvimage.h"
#include "algorithm.h"

namespace PRImA 
{

class COpenCvBiLevelImage;
class COpenCvGreyScaleImage;

/*
 * Class CImageTransformer
 *
 * Provides methods to transform images.
 *
 * CC 02.12.2010
 */
class CImageTransformer
{
private:
	CImageTransformer(void);

public:
	static COpenCvImage * Resize(COpenCvImage * image, int newWidth, int newHeight, bool highQuality = false);
	
	static COpenCvImage * Erode(COpenCvImage * image);
	static COpenCvImage * Dilate(COpenCvImage * image);
	
	static COpenCvBiLevelImage * Binarize(COpenCvImage * source, int threshold);
	static COpenCvBiLevelImage * OtsuBinarization(COpenCvImage * source);
	static COpenCvBiLevelImage * SauvolaBinarization(COpenCvImage * source, double k = 0.3, int w = 40, PRImA::CAlgorithm * stopSignalSource = NULL);
	static void CleanUpForSauvola(int64_t ** array1, int64_t ** array2, int64_t ** array3, int64_t ** array4, int count);

	static COpenCvGreyScaleImage * ConvertToGreyScale(COpenCvImage * source);
	static COpenCvColourImage * ConvertToColour(COpenCvImage * source);

	static void Rotate(COpenCvImage * source, bool clockwise);

private:
	static COpenCvImage * Erode(COpenCvBiLevelImage * image);
	static COpenCvImage * Erode(COpenCvGreyScaleImage * image);
	static COpenCvImage * Dilate(COpenCvBiLevelImage * image);
	static COpenCvImage * Dilate(COpenCvGreyScaleImage * image);
};

}