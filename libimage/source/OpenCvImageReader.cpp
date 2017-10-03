#include "OpenCvImageReader.h"
#include "tiffio.h"
#include "extrafilehelper.h"

using namespace cv;

namespace PRImA {


/*
 * Class COpenCvImageReader
 *
 * Image file reader using OpenCV library.
 *
 * CC 01/11/2013 - created
 */

/*
 * Constructor
 */
COpenCvImageReader::COpenCvImageReader(void)
{
	m_Debug = false;
}

/*
 * Destructor
 */
COpenCvImageReader::~COpenCvImageReader(void)
{
}

/*
 * Loads an image from the given file location.
 * The colour depth of the image will be detected automatically.
 * 
 * Supported file types are: .bmp .jpg .jp2 .png .tif
 * 
 * Returns: Instance of COpenCvColourImage, COpenCvGreyScaleImage or COpenCvBiLevelImage or NULL.
 */
COpenCvImage * COpenCvImageReader::Read(CUniString filePath)
{
	return Read(filePath, COpenCvImage::TYPE_AUTO);
}

/*
 * Loads an image from the given file location.
 * 
 * Supported file types are: .bmp .jpg .jp2 .png .tif
 * 
 * 'enforceType' - Colour depth of image. One of:
 *                      COpenCvImage::TYPE_COLOUR     - RGB colour image
 *                      COpenCvImage::TYPE_GREYSCALE  - Grey scale image
 *                      COpenCvImage::TYPE_BILEVEL    - Black-and-white image
 *                      COpenCvImage::TYPE_AUTO       - Auto-detect
 * Returns: Instance of COpenCvColourImage, COpenCvGreyScaleImage or COpenCvBiLevelImage or NULL.
 */
COpenCvImage * COpenCvImageReader::Read(CUniString filePath, int enforceType)
{
	return Read(filePath, 0, enforceType);
}

/*
 * Loads an image from the given file location.
 *
 * Supported file types are: .bmp .jpg .jp2 .png .tif
 *
 * 'pageIndex' - If > 0, a multi-page image will be assumed and the corresponding page will be loaded
 * 'enforceType' - Colour depth of image. One of:
 *                      COpenCvImage::TYPE_COLOUR     - RGB colour image
 *                      COpenCvImage::TYPE_GREYSCALE  - Grey scale image
 *                      COpenCvImage::TYPE_BILEVEL    - Black-and-white image
 *                      COpenCvImage::TYPE_AUTO       - Auto-detect
 * Returns: Instance of COpenCvColourImage, COpenCvGreyScaleImage or COpenCvBiLevelImage or NULL.
 */
COpenCvImage * COpenCvImageReader::Read(CUniString filePath, int pageIndex, int enforceType)
{
		
	if (m_Debug)
	{
		printf("Reading file '%s'\n", filePath.ToC_Str());
		if (enforceType == COpenCvImage::TYPE_COLOUR)
			printf("  Requested type: COLOUR\n");
		else if (enforceType == COpenCvImage::TYPE_GREYSCALE)
			printf("  Requested type: GREYSCALE\n");
		else if (enforceType == COpenCvImage::TYPE_BILEVEL)
			printf("  Requested type: BILEVEL\n");
		else if (enforceType == COpenCvImage::TYPE_AUTO)
			printf("  Requested type: AUTO\n");
	}
	CT2CA pszConvertedAnsiString(filePath);
	string filename(pszConvertedAnsiString);
	Mat imageData;
	bool ensurePixelValuesAreInRange = true;

	if (m_Debug) 
		printf("  Reading image info\n");

	CImageInfo * info = ReadImageInfo(filePath);
	if (enforceType == COpenCvImage::TYPE_AUTO && info->bitsPerPixel == 1)
	{
		enforceType = COpenCvImage::TYPE_BILEVEL;
		ensurePixelValuesAreInRange = false; //Avoid binarisation (save time)
	}

	if (m_Debug) 
		printf("  Reading image data\n");

	if (pageIndex <= 0) //One page
	{
		if (enforceType == COpenCvImage::TYPE_AUTO || enforceType == COpenCvImage::TYPE_COLOUR)
			imageData = imread(filename); //Colour
		else //BiLevel, grey scale
			imageData = imread(filename, CV_LOAD_IMAGE_GRAYSCALE); //Grey scale
	}
	else //Multi-page
	{
		vector<Mat> allPages;
		bool success = true;
		
		if (enforceType == COpenCvImage::TYPE_AUTO || enforceType == COpenCvImage::TYPE_COLOUR)
			success = imreadmulti(filename, allPages); //Colour
		else //BiLevel, grey scale
			success = imreadmulti(filename, allPages, CV_LOAD_IMAGE_GRAYSCALE); //Grey scale

		if (!success) //Could not load
			return NULL;

		if (allPages.size() > pageIndex) //Enough pages
			imageData = allPages.at(pageIndex);
		else if (allPages.size() > 0) //Not enough pages -> Return first page
			imageData = allPages.at(0);
		else //No pages - Error 
			return NULL;
	}

	//TODO check for errors
	if (imageData.data == NULL)
	{
		//...
		delete info;
		return NULL;
	}

	if (enforceType == COpenCvImage::TYPE_AUTO && info->bitsPerPixel == 1)
		enforceType = COpenCvImage::TYPE_BILEVEL;

	if (m_Debug) 
		printf("  Creating OpenCV image\n");

	COpenCvImage * img = COpenCvImage::Create(imageData, enforceType, ensurePixelValuesAreInRange);

	if (img != NULL)
	{
		img->SetImageInfo(info);
		CUniString nameOnly, pathOnly;
		CExtraFileHelper::SplitPath(filePath, pathOnly, nameOnly);
		img->SetName(nameOnly);
		img->SetFilePath(filePath);
	}
	else
		delete info;

	if (m_Debug) 
		printf("  Finished loading\n");

	return img;
}

/*
 * Reads one page from a multi-page image file
 */
COpenCvImage * COpenCvImageReader::ReadMulti(CUniString filePath, int pageIndex)
{
	return ReadMulti(filePath, pageIndex, COpenCvImage::TYPE_AUTO);
}

/*
 * Reads one page from a multi-page image file
 */
COpenCvImage * COpenCvImageReader::ReadMulti(CUniString filePath, int pageIndex, int enforceType)
{
	return Read(filePath, pageIndex, enforceType);
}

/*
 * Reads image metadata (such as resolution).
 * At the moment only supported for TIFF images.
 */
CImageInfo * COpenCvImageReader::ReadImageInfo(CUniString filePath)
{
	CImageInfo * info = new CImageInfo();

	if (filePath.IsEmpty() || !CExtraFileHelper::FileExists(filePath))
		return info;

	CUniString lowerCase = filePath.MakeLower();

	//Use libtiff
	if (lowerCase.EndsWith(L".tif") || lowerCase.EndsWith(L".tiff"))
	{
		TIFF * tif;
		if(!(tif = TIFFOpen(filePath.ToC_Str(), "r")))
			return info;

		//Resolution
		unsigned w,h;
		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);

		float x=0.0f,y=0.0f;
		TIFFGetField(tif, TIFFTAG_XRESOLUTION, &x);
		TIFFGetField(tif, TIFFTAG_YRESOLUTION, &y);

		uint16_t ResUnit = RESUNIT_CENTIMETER;
		TIFFGetField(tif, TIFFTAG_RESOLUTIONUNIT, &ResUnit);
		if(ResUnit == RESUNIT_CENTIMETER)
		{
			info->resolutionX = x * 2.54f;
			info->resolutionY = y * 2.54f;
		}
		else
		{
			info->resolutionX = x;
			info->resolutionY = y;
		}
		//TIFFGetField(tif, TIFFTAG_XRESOLUTION, &(info->resolutionX));
		//TIFFGetField(tif, TIFFTAG_YRESOLUTION, &(info->resolutionY));
		//TIFFGetField(tif, TIFFTAG_RESOLUTIONUNIT, &(info->resolutionUnit));

		//bits per pixel
		unsigned short photometric;
		unsigned short bitsPerSample = 1;
		TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
		if (TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric))
		{
			if (photometric == PHOTOMETRIC_MINISWHITE || photometric == PHOTOMETRIC_MINISBLACK)
			{
				// Either Black and White, or Greyscale image
				info->bitsPerPixel = bitsPerSample;
			}
			else if (photometric == PHOTOMETRIC_PALETTE)
			{
				//Palletized Image
				info->bitsPerPixel = bitsPerSample;
			}
			else if (photometric == PHOTOMETRIC_RGB)
			{
				//RGB Image
				unsigned short samplesPerPixel = 1;
				if (TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel))
					info->bitsPerPixel = bitsPerSample * samplesPerPixel;
			}
		}

		TIFFClose(tif);
	}

	return info;
}

} //end namespace