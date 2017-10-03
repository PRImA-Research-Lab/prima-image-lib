#include "ProjectionProfile.h"
#include "histogram.h"
#include "ImageTransformer.h"


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

/*
 * Constructor
 */
CProjectionProfile::CProjectionProfile()
{
}

/*
 * Destructor
 */
CProjectionProfile::~CProjectionProfile()
{
}

/*
 * Calculates the vertical projection for the given image and renders the results to an image of the same size
 */
COpenCvBiLevelImage * CProjectionProfile::CalculateVerticalProjectionProfile(COpenCvImage * inputImage)
{
	if (inputImage == NULL)
		return NULL;
	if (typeid(*inputImage) == typeid(COpenCvBiLevelImage))
		return CalculateVerticalProjectionProfile((COpenCvBiLevelImage*)inputImage);
	if (typeid(*inputImage) == typeid(COpenCvGreyScaleImage))
		return CalculateVerticalProjectionProfile((COpenCvGreyScaleImage*)inputImage);
	if (typeid(*inputImage) == typeid(COpenCvColourImage))
		return CalculateVerticalProjectionProfile((COpenCvColourImage*)inputImage);
	return NULL;
}

/*
 * Calculates the vertical projection for the given bitonal image and renders the results to an image of the same size.
 */
COpenCvBiLevelImage * CProjectionProfile::CalculateVerticalProjectionProfile(COpenCvBiLevelImage * inputImage)
{
	COpenCvBiLevelImage * outputImage = COpenCvImage::CreateB(inputImage->GetWidth(), inputImage->GetHeight(), RGBBLACK);

	vector<int> profile(inputImage->GetWidth());
	int width = inputImage->GetWidth();
	int height = inputImage->GetHeight();

	//Count
	for (int x = 0; x<width; x++)
		profile.at(x) = cv::countNonZero(inputImage->GetData()(cv::Rect(x, 0, 1, height)));

	//Draw
	for (int x = 0; x < width; x++)
		outputImage->DrawLine(x, 0, x, min(profile.at(x), height - 1), false);

	return outputImage;
}

/*
 * Calculates the vertical projection for the given greyscale image and renders the results to an image of the same size.
 * White pixels (value 255) count as 1, values below that count less respectively.
 */
COpenCvBiLevelImage * CProjectionProfile::CalculateVerticalProjectionProfile(COpenCvGreyScaleImage * inputImage)
{
	COpenCvBiLevelImage * outputImage = COpenCvImage::CreateB(inputImage->GetWidth(), inputImage->GetHeight(), RGBBLACK);

	vector<int> profile(inputImage->GetWidth());
	int width = inputImage->GetWidth();
	int height = inputImage->GetHeight();

	//Count
	for (int x = 0; x<width; x++)
		profile.at(x) = cv::sum(inputImage->GetData()(cv::Rect(x, 0, 1, inputImage->GetHeight())))[0];

	//Draw
	for (int x = 0; x < width; x++)
		outputImage->DrawLine(x, 0, x, min(profile.at(x) / 256, height - 1), false);

	return outputImage;
}

/*
 * Calculates the vertical projection for the given colour image and renders the results to an image of the same size.
 * Converts to greyscale.
 */
COpenCvBiLevelImage * CProjectionProfile::CalculateVerticalProjectionProfile(COpenCvColourImage * inputImage)
{
	//Convert to greyscale
	COpenCvGreyScaleImage * greyScaleImage = CImageTransformer::ConvertToGreyScale(inputImage);

	COpenCvBiLevelImage * outputImage = CalculateVerticalProjectionProfile(greyScaleImage);

	delete greyScaleImage;

	return outputImage;
}

/*
 * Calculates the horizontal projection for the given image and renders the results to an image of the same size
 */
COpenCvBiLevelImage * CProjectionProfile::CalculateHorizontalProjectionProfile(COpenCvImage * inputImage)
{
	if (inputImage == NULL)
		return NULL;
	if (typeid(*inputImage) == typeid(COpenCvBiLevelImage))
		return CalculateHorizontalProjectionProfile((COpenCvBiLevelImage*)inputImage);
	if (typeid(*inputImage) == typeid(COpenCvGreyScaleImage))
		return CalculateHorizontalProjectionProfile((COpenCvGreyScaleImage*)inputImage);
	if (typeid(*inputImage) == typeid(COpenCvColourImage))
		return CalculateHorizontalProjectionProfile((COpenCvColourImage*)inputImage);
	return NULL;
}

/*
 * Calculates the horizontal projection for the given bitonal image and renders the results to an image of the same size.
 */
COpenCvBiLevelImage * CProjectionProfile::CalculateHorizontalProjectionProfile(COpenCvBiLevelImage * inputImage)
{
	COpenCvBiLevelImage * outputImage = COpenCvImage::CreateB(inputImage->GetWidth(), inputImage->GetHeight(), RGBBLACK);

	vector<int> profile(inputImage->GetHeight());
	int width = inputImage->GetWidth();
	int height = inputImage->GetHeight();

	//Count
	for (int y = 0; y<inputImage->GetHeight(); y++)
		profile.at(y) = cv::countNonZero(inputImage->GetData()(cv::Rect(0, y, inputImage->GetWidth(), 1)));

	//Draw
	for (int y = 0; y<inputImage->GetHeight(); y++)
		outputImage->DrawLine(0, y, min(profile.at(y), width - 1), y, false);

	return outputImage;
}

/*
 * Calculates the horizontal projection for the given greyscale image and renders the results to an image of the same size.
 * White pixels (value 255) count as 1, values below that count less respectively.
 */
COpenCvBiLevelImage * CProjectionProfile::CalculateHorizontalProjectionProfile(COpenCvGreyScaleImage * inputImage)
{
	COpenCvBiLevelImage * outputImage = COpenCvImage::CreateB(inputImage->GetWidth(), inputImage->GetHeight(), RGBBLACK);

	vector<int> profile(inputImage->GetHeight());
	int width = inputImage->GetWidth();
	int height = inputImage->GetHeight();

	//Count
	for (int y = 0; y<height; y++)
		profile.at(y) = cv::sum(inputImage->GetData()(cv::Rect(0, y, width, 1)))[0];

	//Draw
	for (int y = 0; y<height; y++)
		outputImage->DrawLine(0, y, min(profile.at(y) / 256, width - 1), y, false);

	return outputImage;
}

/*
 * Calculates the horizontal projection for the given colour image and renders the results to an image of the same size.
 * Converts to greyscale.
 */
COpenCvBiLevelImage * CProjectionProfile::CalculateHorizontalProjectionProfile(COpenCvColourImage * inputImage)
{
	//Convert to greyscale
	COpenCvGreyScaleImage * greyScaleImage = CImageTransformer::ConvertToGreyScale(inputImage);

	COpenCvBiLevelImage * outputImage = CalculateHorizontalProjectionProfile(greyScaleImage);

	delete greyScaleImage;

	return outputImage;
}


} //end namespace