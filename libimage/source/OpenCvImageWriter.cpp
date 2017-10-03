#include "OpenCvImageWriter.h"
#include "TiffImageWriter.h"
#include "extrafilehelper.h"

namespace PRImA {

/*
 * Class COpenCvImageWriter
 *
 * Image file writer using OpenCV library.
 *
 * CC 01/11/2013 - created
 */

/*
 * Constructor
 */
COpenCvImageWriter::COpenCvImageWriter(void)
{
}

/*
 * Destructor
 */
COpenCvImageWriter::~COpenCvImageWriter(void)
{
}

/*
 * Saves the given image to a file.
 *
 * Supported file types are: .bmp .jpg .jp2 .png .tif
 */
bool COpenCvImageWriter::Write(COpenCvImage * image, CUniString filePath)
{
	if (image == NULL || filePath.IsEmpty())
		return false;

	CUniString lowerCase = filePath;
	lowerCase.MakeLower();

	//OpenCV cannot write in bi-level format. If the given file is a bi-level image 
	//and the file extension is TIFF, we use the TiffImageWriter class.

	if (typeid(*image) == typeid(COpenCvBiLevelImage)
		&& lowerCase.EndsWith(L".tif") || lowerCase.EndsWith(L".tiff"))
	{
		COpenCvBiLevelImage * openCVImg = (COpenCvBiLevelImage*)image;
		//Convert OpenCV image to (old) bi-level image
		CBiLevelImage bilevel;
		int width = image->GetWidth();
		int height = image->GetHeight();
		bilevel.Create(width, height, RGBWHITE, NULL);

		int x, y;
		for (y = 0; y < height; y++)
			for (x = 0; x < width; x++)
				bilevel.SetPixel(x, y, openCVImg->IsBlack(x, y));

		if (openCVImg->GetImageInfo() != NULL)
		{
			bilevel.SetResolution((double)openCVImg->GetImageInfo()->resolutionX, (double)openCVImg->GetImageInfo()->resolutionY);
			//bilevel.SetResolutionUnit(openCVImg->GetImageInfo()->resolutionUnit);
		}

		CTiffImageWriter tiffWriter;
		tiffWriter.WriteImage(&bilevel, const_cast<char*>(filePath.ToC_Str()));
		return true;
	}

	//Colour or grey scale (or not a TIFF)
	bool success = imwrite(filePath.ToC_Str(), image->GetData());

	if (success)
		WriteImageInfo(image->GetImageInfo(), filePath);

	return success;
}

/*
 * Writes image metadata (such as resolution).
 */
void COpenCvImageWriter::WriteImageInfo(CImageInfo * info, CUniString filePath)
{
	if (info == NULL || !CExtraFileHelper::FileExists(filePath))
		return;

	//Can't use libtiff (can only write everything, not just the header)
	/*TIFF * tif = TIFFOpen(filePath.ToC_Str(), "a");
	TIFFSetField(tif, TIFFTAG_XRESOLUTION, info->resolutionX);
	TIFFSetField(tif, TIFFTAG_YRESOLUTION, info->resolutionY);
	TIFFClose(tif);*/
}

} //end namespace