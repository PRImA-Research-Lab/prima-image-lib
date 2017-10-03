
#include <typeinfo>
#include "StdAfx.h"
#include "TiffImageReader.h"
#include "Image.h"
#include "GreyScaleImage.h"


/*
 * Implementation of the CImageReader class using libtiff.
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

	unsigned short  photometric;
	TIFFGetField(Tif, TIFFTAG_PHOTOMETRIC, &photometric);
	if (photometric == PHOTOMETRIC_MINISWHITE || photometric == PHOTOMETRIC_MINISBLACK)
	{
		// Either Black and White, or Greyscale image
		unsigned short  BitsPerSample;
		TIFFGetField(Tif, TIFFTAG_BITSPERSAMPLE, &BitsPerSample);
		if (BitsPerSample > 1) 
		{
			pImage = new CGreyScaleImage;
			ReadFromTIFF((CGreyScaleImage*)pImage, Tif, FN);
		}
		else
		{
			pImage = new CBiLevelImage;
			ReadFromTIFF((CBiLevelImage*)pImage, Tif, FN);
		}
	}
	else if (photometric == PHOTOMETRIC_PALETTE)
	{
		//Palletized Image
		pImage = new CLoColorImage;
		ReadFromTIFF((CLoColorImage*)pImage, Tif, FN);
	}
	else if (photometric == PHOTOMETRIC_RGB)
	{
		//RGB Image
		pImage = new CHiColorImage;
		ReadFromTIFF((CHiColorImage*)pImage, Tif, FN);
	}

	if(pImage == NULL)
	{
		delete [] FN;
		return NULL;
	}	

	
	TIFFClose(Tif);
	delete [] FN;
	return pImage;
}

/*
 * Reads an image
 */
/*bool CTiffImageReader::ReadFromTIFF(CImage * pImage, TIFF * pTif, char * name) {
	if (typeid(pImage) == typeid(CHiColorImage*))
		return ReadFromTIFF((CHiColorImage*)pImage, pTif, name);
	if (typeid(pImage) == typeid(CLoColorImage*))
		return ReadFromTIFF((CLoColorImage*)pImage, pTif, name);
	if (typeid(pImage) == typeid(CBiLevelImage*))
		return ReadFromTIFF((CBiLevelImage*)pImage, pTif, name);
	if (typeid(pImage) == typeid(CGreyScaleImage*))
		return ReadFromTIFF((CGreyScaleImage*)pImage, pTif, name);
	return false;
}*/

/*
 * Reads high color image
 */
bool CTiffImageReader::ReadFromTIFF(CHiColorImage * pImage, TIFF * pTif, char * name)
{
	if (pImage == NULL) return false;
	if (pTif == NULL) return false;

	unsigned i;
	unsigned short  config;
	uint8_t * scan_buffer;

	// check for contiguous planar configuration
	TIFFGetField(pTif, TIFFTAG_PLANARCONFIG, &config);
	if (config != PLANARCONFIG_CONTIG)
	{
		return false;
	}

	uint16_t TempInt;

	TIFFGetField(pTif, TIFFTAG_BITSPERSAMPLE, &TempInt);
	TIFFGetField(pTif, TIFFTAG_SAMPLESPERPIXEL, pImage->GetBitsPerPixel());
	pImage->SetBitsPerPixel(TempInt);
	TIFFGetField(pTif, TIFFTAG_IMAGEWIDTH, pImage->GetWidth());
	TIFFGetField(pTif, TIFFTAG_IMAGELENGTH, pImage->GetHeight());
	TIFFGetField(pTif, TIFFTAG_XRESOLUTION, pImage->GetXRes());
	TIFFGetField(pTif, TIFFTAG_YRESOLUTION, pImage->GetYRes());
	TIFFGetField(pTif, TIFFTAG_RESOLUTIONUNIT, &TempInt);
	if(TempInt == RESUNIT_CENTIMETER)
	{
		pImage->SetResolution(pImage->GetXRes() * 2.54f, pImage->GetYRes() * 2.54f);
	}

	pImage->Create(pImage->GetWidth(), pImage->GetHeight(), RGBBLACK, name);

	int BytesInBuffer = TIFFScanlineSize(pTif);
	scan_buffer = new uint8_t[BytesInBuffer * sizeof(uint8_t)];

	//Create Image Arrays
	pImage->InitArrays();

	uint8_t * TempImage = new uint8_t[pImage->GetBytesPerLine() * pImage->GetHeight()];
	TIFFReadRGBAImage(pTif, pImage->GetWidth(), pImage->GetHeight(), (uint32*) TempImage, 0);

	for(i = 0; i < pImage->GetHeight(); i++)
	{
#ifdef _MSC_VER
		pImage->GetLineArray()[i] = &(pImage->GetArray()[pImage->GetBytesPerLine() * (pImage->GetHeight() - i - 1)]);
#else
		m_pLineArray[i] = &(m_pArray[m_BytesPerLine * i]);
#endif
		memcpy(pImage->GetLineArray()[i], &(TempImage[pImage->GetBytesPerLine() * (pImage->GetHeight() - i - 1)]), 
				sizeof(uint8_t) * pImage->GetBytesPerLine());

#ifdef _MSC_VER
		unsigned j;
		unsigned char SwapByte;

		for(j = 0; j < pImage->GetWidth(); j++)
		{
			SwapByte = pImage->GetLineArray()[i][j * 4 + 0];
			pImage->GetLineArray()[i][j * 4 + 0] = pImage->GetLineArray()[i][j * 4 + 2];
			pImage->GetLineArray()[i][j * 4 + 2] = SwapByte;
		}
#endif
	}

	delete [] TempImage;
	delete [] scan_buffer;

	// Get Colour Profile

	uint8_t * TempProfile;
	TIFFGetField(pTif, TIFFTAG_ICCPROFILE, pImage->GetIccLength(), &TempProfile);
	delete [] pImage->GetIccProfile();
	pImage->SetIccProfile(new uint8_t[pImage->GetIccLength()]);
	memcpy(pImage->GetIccProfile(), TempProfile, sizeof(uint8_t) * pImage->GetIccLength());

	return true;
}

/*
 * Reads low color image
 */
bool CTiffImageReader::ReadFromTIFF(CLoColorImage * pImage, TIFF * pTif, char * name)
{
	if (pImage == NULL) return false;
	if (pTif == NULL) return false;

	unsigned        y;
	unsigned short  config, photometric;
	unsigned char * scan_buffer;

	// check for contiguous planar configuration
	TIFFGetField(pTif, TIFFTAG_PLANARCONFIG, &config);
	if (config != PLANARCONFIG_CONTIG)
	{
		return false;
	}

	uint16_t ResUnit;

	TIFFGetField(pTif, TIFFTAG_IMAGEWIDTH, pImage->GetWidth());
	TIFFGetField(pTif, TIFFTAG_IMAGELENGTH, pImage->GetHeight());
	TIFFGetField(pTif, TIFFTAG_XRESOLUTION, pImage->GetXRes());
	TIFFGetField(pTif, TIFFTAG_YRESOLUTION, pImage->GetYRes());
	TIFFGetField(pTif, TIFFTAG_RESOLUTIONUNIT, &ResUnit);
	if(ResUnit == RESUNIT_CENTIMETER)
	{
		pImage->SetResolution(pImage->GetXRes() * 2.54f, pImage->GetYRes() * 2.54f);
	}

	//Call Parent Class' Create, to initialize member variables
	pImage->SetBitsPerPixel(8);
	pImage->Create(pImage->GetWidth(), pImage->GetHeight(), RGBBLACK, name);

	// get photometric interpretation information
	TIFFGetField(pTif, TIFFTAG_PHOTOMETRIC, &photometric);

	// allocate memory for scan_buffer
	scan_buffer = new uint8_t[TIFFScanlineSize(pTif) * sizeof(uint8_t)];

	//Create Image Arrays
	int BytesInBuffer = TIFFScanlineSize(pTif);
	uint8_t * m_pArray = new uint8_t[pImage->GetBytesPerLine() * pImage->GetHeight()];

	for (y=0; y<pImage->GetHeight(); y++)
	{
		TIFFReadScanline(pTif, scan_buffer, y, 0);
		//copy pixels in BitArray
		memcpy(m_pArray+(y*pImage->GetBytesPerLine()), scan_buffer, BytesInBuffer);
		if (photometric == PHOTOMETRIC_MINISWHITE)
		{
			//Invert pixels, so that we get black to be 0 and white to be 1
			for (int b = 0; b < pImage->GetBytesPerLine(); b++)
				m_pArray[y * pImage->GetBytesPerLine() + b] = 255 - m_pArray[y*pImage->GetBytesPerLine() + b];
		}
	}

	pImage->SetLineArray(new uint8_t * [pImage->GetHeight()]);
	for(y = 0; y < pImage->GetHeight(); y++)
	{
		pImage->GetLineArray()[y] = &(pImage->GetArray()[y * pImage->GetBytesPerLine()]);
		for(int x = 0; x < 15; x++)
		{
			printf("%d\t", pImage->GetLineArray()[y][x]);
		}
		printf("\n");
	}

	delete [] scan_buffer;

	/* Now load palette */

	uint16_t * R;
	uint16_t * G;
	uint16_t * B;
	TIFFGetField(pTif, TIFFTAG_COLORMAP, &R, &G, &B);

	for(y = 0; y < 256; y++)
	{
		pImage->GetPalette()[y].R = R[y] * 256 / 65536;
		pImage->GetPalette()[y].G = G[y] * 256 / 65536;
		pImage->GetPalette()[y].B = B[y] * 256 / 65536;
	}

	// Get Colour Profile

	uint8_t * TempProfile;
	TIFFGetField(pTif, TIFFTAG_ICCPROFILE, pImage->GetIccLength(), &TempProfile);
	delete [] pImage->GetIccProfile();
	pImage->SetIccProfile(new uint8_t[pImage->GetIccLength()]);
	memcpy(	pImage->GetIccProfile(), 
			TempProfile, 
			sizeof(uint8_t) * pImage->GetIccLength());

	return true;
}

/*
 * Reads bi-level image
 */
bool CTiffImageReader::ReadFromTIFF(CBiLevelImage * pImage, TIFF * pTif, char * name)
{
	if (pImage == NULL) return false;
	if (pTif == NULL) return false;

	unsigned y;
	unsigned short  config, photometric;
	unsigned char * scan_buffer;

	// check for contiguous planar configuration
	TIFFGetField(pTif, TIFFTAG_PLANARCONFIG, &config);
	if (config != PLANARCONFIG_CONTIG)
	{
		return false;
	}

	uint16_t ResUnit = RESUNIT_CENTIMETER;

	TIFFGetField(pTif, TIFFTAG_IMAGEWIDTH, pImage->GetWidth());
	TIFFGetField(pTif, TIFFTAG_IMAGELENGTH, pImage->GetHeight());
	TIFFGetField(pTif, TIFFTAG_XRESOLUTION, pImage->GetXRes());
	TIFFGetField(pTif, TIFFTAG_YRESOLUTION, pImage->GetYRes());
	TIFFGetField(pTif, TIFFTAG_RESOLUTIONUNIT, &ResUnit);
	if(ResUnit == RESUNIT_CENTIMETER)
	{
		pImage->SetResolution(pImage->GetXRes() * 2.54f, pImage->GetYRes() * 2.54f);
	}

	//Call Parent Class' Create, to initialize member variables
	pImage->SetBitsPerPixel(1);
	pImage->Create(pImage->GetWidth(), pImage->GetHeight(), RGBBLACK, name);

	// get photometric interpretation information
	TIFFGetField(pTif, TIFFTAG_PHOTOMETRIC, &photometric);

	// allocate memory for scan_buffer
	scan_buffer = new unsigned char[TIFFScanlineSize(pTif) * sizeof(uint8_t)];

	//Create Image Arrays
	int BytesInBuffer = TIFFScanlineSize(pTif);
	unsigned char * BitArray = new unsigned char[pImage->GetBytesPerLine() * pImage->GetHeight()];

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

	uint8_t * TempProfile;
	TIFFGetField(pTif, TIFFTAG_ICCPROFILE, pImage->GetIccLength(), &TempProfile);
	delete [] pImage->GetIccProfile();
	pImage->SetIccProfile(new uint8_t[pImage->GetIccLength()]);
	memcpy(pImage->GetIccProfile(), TempProfile, sizeof(uint8_t) * pImage->GetIccLength());

	return true;
}

/*
 * Reads grey scale image
 */
bool CTiffImageReader::ReadFromTIFF(CGreyScaleImage * pImage, TIFF * pTif, char * name)
{
	if (pImage == NULL) return false;
	if (pTif == NULL) return false;

	unsigned y;
	unsigned short  config, photometric;
	uint8_t * scan_buffer;

	// check for contiguous planar configuration
	TIFFGetField(pTif, TIFFTAG_PLANARCONFIG, &config);
	if (config != PLANARCONFIG_CONTIG)
	{
		return false;
	}

	uint16_t ResUnit;

	TIFFGetField(pTif, TIFFTAG_IMAGEWIDTH, pImage->GetWidth());
	TIFFGetField(pTif, TIFFTAG_IMAGELENGTH, pImage->GetHeight());
	TIFFGetField(pTif, TIFFTAG_XRESOLUTION, pImage->GetXRes());
	TIFFGetField(pTif, TIFFTAG_YRESOLUTION, pImage->GetYRes());
	TIFFGetField(pTif, TIFFTAG_RESOLUTIONUNIT, &ResUnit);
	if(ResUnit == RESUNIT_CENTIMETER)
	{
		pImage->SetResolution(pImage->GetXRes() * 2.54f, pImage->GetYRes() * 2.54f);
	}

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
	int uint8_tsInBuffer = TIFFScanlineSize(pTif);
	scan_buffer = new uint8_t[uint8_tsInBuffer * sizeof(uint8_t)];

	//Create Image Arrays
	pImage->SetArray(new uint8_t[pImage->GetWidth() * pImage->GetHeight()]);
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
			for (int b = 0; b < pImage->GetBytesPerLine(); b++)
				pImage->GetArray()[y*pImage->GetBytesPerLine() + b] = ~pImage->GetArray()[y*pImage->GetBytesPerLine() + b];
		}
	}

	delete [] scan_buffer;

	// Get Colour Profile

	uint8_t * TempProfile;
	TIFFGetField(pTif, TIFFTAG_ICCPROFILE, pImage->GetIccLength(), pImage->GetIccProfile());
	delete [] pImage->GetIccProfile();
	pImage->SetIccProfile(new uint8_t[pImage->GetIccLength()]);
	memcpy(pImage->GetIccProfile(), TempProfile, sizeof(uint8_t) * pImage->GetIccLength());

	return true;
}