#pragma once

#include "opencvimage.h"

namespace PRImA
{

/*
 * Class CProjectionProfile
 *
 * Create horizontal or vertical projection profile for an image
 * See also CHistogram
 *
 * CC 19/08/2016 - created
 */
class CProjectionProfile
{
public:
	CProjectionProfile();
	~CProjectionProfile();

	COpenCvBiLevelImage * CalculateVerticalProjectionProfile(COpenCvImage * inputImage);
	COpenCvBiLevelImage * CalculateHorizontalProjectionProfile(COpenCvImage * inputImage);

private:
	COpenCvBiLevelImage * CalculateVerticalProjectionProfile(COpenCvBiLevelImage * inputImage);
	COpenCvBiLevelImage * CalculateVerticalProjectionProfile(COpenCvGreyScaleImage * inputImage);
	COpenCvBiLevelImage * CalculateVerticalProjectionProfile(COpenCvColourImage * inputImage);

	COpenCvBiLevelImage * CalculateHorizontalProjectionProfile(COpenCvBiLevelImage * inputImage);
	COpenCvBiLevelImage * CalculateHorizontalProjectionProfile(COpenCvGreyScaleImage * inputImage);
	COpenCvBiLevelImage * CalculateHorizontalProjectionProfile(COpenCvColourImage * inputImage);
};


} //end namespace