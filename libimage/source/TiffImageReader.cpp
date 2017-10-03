
#include <typeinfo>
//#include "StdAfx.h"
#include "TiffImageReader.h"
#include "Image.h"
#include "GreyScaleImage.h"

namespace PRImA
{


/*
 * Class CTiffImageReader
 * 
 * Implementation of the CImageReader class using libtiff.
 *
 * Deprecated: Use OpenCV image classes
 *
 * CC 06.10.2009 - created
 */

CTiffImageReader::CTiffImageReader(void)
{
}

CTiffImageReader::~CTiffImageReader(void)
{
}

CImage * CTiffImageReader::ReadImage(const char* fileName)
{
	CImage * pImage = NULL;
	// Find out the Bits Per Pixel
	TIFF * Tif;

	if(fileName == NULL)
		return NULL;

	char * FN = new char[strlen(fileName) + 1];
	memcpy(FN,fileName,sizeof(char) * (strlen(fileName) + 1));

	// open tiff file and get parameters
	if(!(Tif = TIFFOpen(FN, "r")))
		return NULL;

	unsigned short photometric;
	if (TIFFGetField(Tif, TIFFTAG_PHOTOMETRIC, &photometric) != 1)
		return NULL;

	if (photometric == PHOTOMETRIC_MINISWHITE || photometric == PHOTOMETRIC_MINISBLACK)
	{
		// Either Black and White, or Greyscale image
		unsigned short  BitsPerSample;
		if (TIFFGetField(Tif, TIFFTAG_BITSPERSAMPLE, &BitsPerSample) != 1)
			BitsPerSample = 1;
		if (BitsPerSample > 1) 
		{
			pImage = new CGreyScaleImage();
			ReadFromTIFF((CGreyScaleImage*)pImage, Tif, FN);
		}
		else
		{
			pImage = new CBiLevelImage();
			ReadFromTIFF((CBiLevelImage*)pImage, Tif, FN);
		}
	}
	else if (photometric == PHOTOMETRIC_PALETTE)
	{
		//Palletized Image
		pImage = new CLoColorImage();
		ReadFromTIFF((CLoColorImage*)pImage, Tif, FN);
	}
	else if (photometric == PHOTOMETRIC_RGB)
	{
		//RGB Image
		pImage = new CHiColorImage();
		ReadFromTIFF((CHiColorImage*)pImage, Tif, FN);
	}

	if(pImage == NULL)
	{
		delete [] FN;
		return NULL;
	}	

	pImage->SetFilePath(CUniString(fileName));

	//ReadFromTIFF(pImage, Tif, FN);
	TIFFClose(Tif);
	delete [] FN;
	return pImage;
}

/*
 * Reads high color image
 */
bool CTiffImageReader::ReadFromTIFF(CHiColorImage * pImage, TIFF * pTif, char * name)
{
	int i;

	if (pTif == NULL) return false;

	unsigned short  config;
	uint8_t * scan_buffer;

	// check for contiguous planar configuration
	TIFFGetField(pTif, TIFFTAG_PLANARCONFIG, &config);
	if (config != PLANARCONFIG_CONTIG)
	{
		return false;
	}

	uint16_t tempInt;

	TIFFGetField(pTif, TIFFTAG_BITSPERSAMPLE, &tempInt);
	unsigned int samples = 0;
	TIFFGetField(pTif, TIFFTAG_SAMPLESPERPIXEL, &samples);
	pImage->SetBitsPerPixel(tempInt*samples);

	this->getImageSizeAndResolution(pImage, pTif);

	pImage->Create(pImage->GetWidth(), pImage->GetHeight(), RGBBLACK, name);

	int BytesInBuffer = (int)TIFFScanlineSize(pTif);
	scan_buffer = new uint8_t[BytesInBuffer * sizeof(uint8_t)];

	//Create Image Arrays
	pImage->InitArrays();

	uint8_t * TempImage = NULL;
	try 
	{
		TempImage = new uint8_t[pImage->GetBytesPerLine() * pImage->GetHeight()];
	}
	catch (CMemoryException * exc)
	{
		delete pImage;
		delete [] scan_buffer;
		throw exc;
	}
	TIFFReadRGBAImage(pTif, pImage->GetWidth(), pImage->GetHeight(), (uint32*) TempImage, 0);

	for(i = 0; i < pImage->GetHeight(); i++)
	{
		pImage->GetLineArray()[i] = &(pImage->GetArray()[pImage->GetBytesPerLine() * (pImage->GetHeight() - i - 1)]);

		memcpy(pImage->GetLineArray()[i], &(TempImage[pImage->GetBytesPerLine() * (pImage->GetHeight() - i - 1)]), 
				sizeof(uint8_t) * pImage->GetBytesPerLine());

		int j;
		unsigned char SwapByte;

		for(j = 0; j < pImage->GetWidth(); j++)
		{
			SwapByte = pImage->GetLineArray()[i][j * 4 + 0];
			pImage->GetLineArray()[i][j * 4 + 0] = pImage->GetLineArray()[i][j * 4 + 2];
			pImage->GetLineArray()[i][j * 4 + 2] = SwapByte;
		}
	}

	delete [] TempImage;
	delete [] scan_buffer;

	// Get Colour Profile
	getColorProfile(pImage, pTif);

	return true;
}

/*
 * Reads low color image
 */
bool CTiffImageReader::ReadFromTIFF(CLoColorImage * pImage, TIFF * pTif, char * name)
{
	int        y;
	unsigned short  config, photometric;
	unsigned char * scan_buffer;

	// check for contiguous planar configuration
	TIFFGetField(pTif, TIFFTAG_PLANARCONFIG, &config);
	if (config != PLANARCONFIG_CONTIG)
	{
		return false;
	}

	getImageSizeAndResolution(pImage, pTif);

	//Call Parent Class' Create, to initialize member variables
	pImage->SetBitsPerPixel(8);
	pImage->Create(pImage->GetWidth(), pImage->GetHeight(), RGBBLACK, name);

	// get photometric interpretation information
	TIFFGetField(pTif, TIFFTAG_PHOTOMETRIC, &photometric);

	// allocate memory for scan_buffer
	scan_buffer = new uint8_t[TIFFScanlineSize(pTif) * sizeof(uint8_t)];

	//Create Image Arrays
	int BytesInBuffer = (int)TIFFScanlineSize(pTif);
	uint8_t * arr = NULL;
	try 
	{
		arr = new uint8_t[pImage->GetBytesPerLine() * pImage->GetHeight()];
	}
	catch (CMemoryException * exc)
	{
		delete pImage;
		delete [] scan_buffer;
		throw exc;
	}
	delete [] pImage->GetArray();
	pImage->SetArray(arr);

	for (y=0; y<pImage->GetHeight(); y++)
	{
		TIFFReadScanline(pTif, scan_buffer, y, 0);
		//copy pixels in BitArray
		memcpy(arr+(y*pImage->GetBytesPerLine()), scan_buffer, BytesInBuffer);
		if (photometric == PHOTOMETRIC_MINISWHITE)
		{
			//Invert pixels, so that we get black to be 0 and white to be 1
			for (int b = 0; b < pImage->GetBytesPerLine(); b++)
				arr[y * pImage->GetBytesPerLine() + b] = 255 - arr[y*pImage->GetBytesPerLine() + b];
		}
	}

	delete [] pImage->GetLineArray();
	pImage->SetLineArray(new uint8_t * [pImage->GetHeight()]);
	for(y = 0; y < pImage->GetHeight(); y++)
	{
		pImage->GetLineArray()[y] = &(arr[y * pImage->GetBytesPerLine()]);
		#ifdef TIFF_IMAGE_READER_DEBUG
			for(int x = 0; x < 15; x++)
			{
				printf("%d\t", pImage->GetLineArray()[y][x]);
			}
			printf("\n");
		#endif
	}

	delete [] scan_buffer;

	/* Now load palette */

	uint16_t* R;
	uint16_t* G;
	uint16_t* B;
	TIFFGetField(pTif, TIFFTAG_COLORMAP, &R, &G, &B);

	for(y = 0; y < 256; y++)
	{
		pImage->GetPalette()[y].R = R[y] / 256;
		pImage->GetPalette()[y].G = G[y] / 256;
		pImage->GetPalette()[y].B = B[y] / 256;
	}

	// Get Colour Profile
	getColorProfile(pImage, pTif);

	return true;
}

/*
 * Reads bi-level image
 */
bool CTiffImageReader::ReadFromTIFF(CBiLevelImage * pImage, TIFF * pTif, char * name)
{
	int y;
	unsigned short  config, photometric;
	unsigned char * scan_buffer;

	// check for contiguous planar configuration
	TIFFGetField(pTif, TIFFTAG_PLANARCONFIG, &config);
	if (config != PLANARCONFIG_CONTIG)
	{
		return false;
	}

	getImageSizeAndResolution(pImage, pTif);

	//Call Parent Class' Create, to initialize member variables
	pImage->SetBitsPerPixel(1);
	pImage->Create(pImage->GetWidth(), pImage->GetHeight(), RGBBLACK, name);

	// get photometric interpretation information
	TIFFGetField(pTif, TIFFTAG_PHOTOMETRIC, &photometric);

	// allocate memory for scan_buffer
	scan_buffer = new unsigned char[TIFFScanlineSize(pTif) * sizeof(uint8_t)];

	//Create Image Arrays
	int BytesInBuffer = (int)TIFFScanlineSize(pTif);

	//CC 17.11.2009 Added this because of problems with bi-level images
	//pImage->SetBytesBerLine(BytesInBuffer);

	unsigned char * BitArray = NULL;
	try 
	{
		BitArray = new unsigned char[pImage->GetBytesPerLine() * pImage->GetHeight()];
	}
	catch (CMemoryException * exc)
	{
		delete pImage;
		delete [] scan_buffer;
		throw exc;
	}

	for(y = 0; y < pImage->GetHeight(); y++)
	{
		TIFFReadScanline(pTif, scan_buffer, y, 0);
		//copy pixels in BitArray
		memcpy(BitArray+(y*pImage->GetBytesPerLine()), scan_buffer, BytesInBuffer);
		if (photometric == PHOTOMETRIC_MINISWHITE)
		{
			//Invert pixels, so that we get black to be 0 and white to be 1
			for (int b = 0; b < pImage->GetBytesPerLine(); b++)
				BitArray[y * pImage->GetBytesPerLine() + b] = ~BitArray[y*pImage->GetBytesPerLine() + b];
		}
	}

	pImage->UnPackBitArray(BitArray);
	
	delete [] BitArray;

	delete [] scan_buffer;

	// Get Colour Profile
	getColorProfile(pImage, pTif);

	return true;
}

/*
 * Reads grey scale image
 */
bool CTiffImageReader::ReadFromTIFF(CGreyScaleImage * pImage, TIFF * pTif, char * name)
{
	int y;

	if (pTif == NULL) return false;

	unsigned short  config, photometric;
	uint8_t * scan_buffer;

	// check for contiguous planar configuration
	TIFFGetField(pTif, TIFFTAG_PLANARCONFIG, &config);
	if (config != PLANARCONFIG_CONTIG)
	{
		return false;
	}

	getImageSizeAndResolution(pImage, pTif);

	// ICC Colour Profile, If Present

	if(pImage->GetIccLength() != 0)
	{
		if(TIFFSetField(pTif, TIFFTAG_ICCPROFILE, pImage->GetIccLength(), pImage->GetIccProfile()) == 1)
			printf("Setting ICC Tag Succeeded\n");
		else
			printf("Setting ICC Tag Failed\n");
	}

	//Call Parent Class' Create, to initialize member variables
	pImage->SetBitsPerPixel(8); //By Definition, we treat all GreyScale Images as 8bit
//	unsigned short  BitsPerSample, SamplesPerPixel;
//	TIFFGetField(Tif, TIFFTAG_BITSPERSAMPLE, &BitsPerSample);
//	TIFFGetField(Tif, TIFFTAG_SAMPLESPERPIXEL, &SamplesPerPixel);
//	m_BitsPerPixel = BitsPerSample * SamplesPerPixel;
	pImage->Create(pImage->GetWidth(), pImage->GetHeight(), RGBBLACK, name);

	// get photometric interpretation information
	TIFFGetField(pTif, TIFFTAG_PHOTOMETRIC, &photometric);

	// allocate memory for scan_buffer
	int uint8_tsInBuffer = (int)TIFFScanlineSize(pTif);
	scan_buffer = new uint8_t[uint8_tsInBuffer * sizeof(uint8_t)];

	//Create Image Arrays
	delete [] pImage->GetArray();
	try 
	{
		pImage->SetArray(new uint8_t[pImage->GetWidth() * pImage->GetHeight()]);
	}
	catch (CMemoryException * exc)
	{
		delete pImage;
		delete [] scan_buffer;
		throw exc;
	}
	delete [] pImage->GetLineArray();
	pImage->SetLineArray(new uint8_t*[pImage->GetHeight()]);
	pImage->GetLineArray()[0] = pImage->GetArray();
	for(y = 1; y < pImage->GetHeight(); y++)
	{
		pImage->GetLineArray()[y] = pImage->GetLineArray()[y-1] + pImage->GetWidth();
	}

	for(y = 0; y < pImage->GetHeight(); y++)
	{
		TIFFReadScanline(pTif, scan_buffer, y, 0);
		//copy pixels in m_pArray
		memcpy(pImage->GetArray()+(y*pImage->GetWidth()), scan_buffer, uint8_tsInBuffer);
		if (photometric == PHOTOMETRIC_MINISWHITE)
		{
			//Invert pixels, so that we get black to be 0 and white to be 1
			for (int b = 0; b < uint8_tsInBuffer; b++)
				pImage->GetArray()[y*uint8_tsInBuffer + b] = ~pImage->GetArray()[y*uint8_tsInBuffer + b];
		}
	}

	delete [] scan_buffer;

	// Get Colour Profile
	getColorProfile(pImage, pTif);

	return true;
}

/*
 * Retrieves size and resolution of the image
 */
void CTiffImageReader::getImageSizeAndResolution(CImage* pImage, TIFF * pTif)
{
	unsigned w,h;
	TIFFGetField(pTif, TIFFTAG_IMAGEWIDTH, &w);
	TIFFGetField(pTif, TIFFTAG_IMAGELENGTH, &h);
	pImage->SetSize(w,h);

	float x=0.0f,y=0.0f;
	TIFFGetField(pTif, TIFFTAG_XRESOLUTION, &x);
	TIFFGetField(pTif, TIFFTAG_YRESOLUTION, &y);

	uint16_t ResUnit = RESUNIT_CENTIMETER;
	TIFFGetField(pTif, TIFFTAG_RESOLUTIONUNIT, &ResUnit);
	if(ResUnit == RESUNIT_CENTIMETER)
		pImage->SetResolution(x * 2.54f, y * 2.54f);
	else
		pImage->SetResolution(x,y);
}

/*
 * Retrieves the color profile of the image
 */
void CTiffImageReader::getColorProfile(CImage* pImage, TIFF * pTif)
{
	uint8_t * TempProfile;
	uint32_t iccLength = 0;
	TIFFGetField(pTif, TIFFTAG_ICCPROFILE, &iccLength, &TempProfile);
	pImage->SetIccLength(iccLength);
	delete [] pImage->GetIccProfile();
	pImage->SetIccProfile(new uint8_t[iccLength]);
	if (iccLength > 0) 
		memcpy(pImage->GetIccProfile(), TempProfile, sizeof(uint8_t) * iccLength);
}


} //end namespace