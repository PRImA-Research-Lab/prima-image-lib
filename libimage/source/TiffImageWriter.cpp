//#include "StdAfx.h"
#include "TiffImageWriter.h"
#include <typeinfo>

namespace PRImA
{

/*
 * Class CTiffImageWriter
 *
 * Implementation of the CImageWriter class using libtiff.
 *
 * Deprecated: Use OpenCV image classes
 *
 * CC 09.10.2009 - created
 */

CTiffImageWriter::CTiffImageWriter(void)
{
}

CTiffImageWriter::~CTiffImageWriter(void)
{
}

/*
 * Writes an image
 */
void CTiffImageWriter::WriteImage(CImage* pImage, char* fileName)
{
	if (typeid(*pImage) == typeid(CBiLevelImage))
		WriteImage((CBiLevelImage*)pImage, fileName);
	else if (typeid(*pImage) == typeid(CGreyScaleImage))
		WriteImage((CGreyScaleImage*)pImage, fileName);
	else if (typeid(*pImage) == typeid(CLoColorImage))
		WriteImage((CLoColorImage*)pImage, fileName);
	else if (typeid(*pImage) == typeid(CHiColorImage))
		WriteImage((CHiColorImage*)pImage, fileName);
}

/*
 * Writes bi-level image
 */
void CTiffImageWriter::WriteImage(CBiLevelImage* pImage, char* fileName)
{
	TIFF* pTif;
	uint8_t *buffer;
	int x,y;

	pTif = TIFFOpen(fileName, "w");

	//Set TAGs
	SetCommonFields(pImage, pTif);
	TIFFSetField(pTif, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(pTif, TIFFTAG_BITSPERSAMPLE, 1);
	TIFFSetField(pTif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE);

	// ICC Colour Profile, If Present

	if(pImage->GetIccLength() != 0)
	{
		if(TIFFSetField(pTif, TIFFTAG_ICCPROFILE, pImage->GetIccLength(), pImage->GetIccProfile()) == 1)
			printf("Setting ICC Tag Succeeded\n");
		else
			printf("Setting ICC Tag Failed\n");
	}

	// allocate memory for buffer
	int BytesInBuffer = (int)TIFFScanlineSize(pTif);
	buffer = new uint8_t[BytesInBuffer];

	//write image rows
	for (y = 0; y < pImage->GetHeight(); y++)
	{
		memset(buffer, 0, sizeof(uint8_t) * BytesInBuffer);

		for(x = 0; x < pImage->GetWidth(); x++)
		{
			if(pImage->IsBlack(x, y))
				buffer[x / 8] |= 0x01 << (7 - (x % 8));
		}

		TIFFWriteScanline(pTif, buffer, y, 0);
	}

	TIFFClose(pTif);
	delete [] buffer;
}

/*
 * Writes grey scale image
 */
void CTiffImageWriter::WriteImage(CGreyScaleImage* pImage, char* fileName)
{
	TIFF *pTif;
	uint8_t *buffer;
	int y;

	pTif = TIFFOpen(fileName, "w");

	//Set TAGs
	SetCommonFields(pImage, pTif);	
	TIFFSetField(pTif, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(pTif, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(pTif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

	// allocate memory for buffer
	int BytesInBuffer = (int)TIFFScanlineSize(pTif);
	buffer = new uint8_t[BytesInBuffer];

	//write image rows
	for (y = 0; y < pImage->GetHeight(); ++y)
	{
		memcpy(buffer, pImage->GetLineArray()[y], BytesInBuffer);
		TIFFWriteScanline(pTif, buffer, y, 0);
	}

	TIFFClose(pTif);
	delete buffer;
}

/*
 * Writes low color image
 */
void CTiffImageWriter::WriteImage(CLoColorImage* pImage, char* fileName)
{
	TIFF *pTif;
	uint8_t *buffer;
	int y;

	pTif = TIFFOpen(fileName, "w");

	//Set TAGs
	SetCommonFields(pImage, pTif);
	TIFFSetField(pTif, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(pTif, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(pTif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_PALETTE);

	// ICC Colour Profile, If Present

	if(pImage->GetIccLength() != 0)
	{
		if(TIFFSetField(pTif, TIFFTAG_ICCPROFILE, pImage->GetIccLength(), pImage->GetIccProfile()) == 1)
			printf("Setting ICC Tag Succeeded\n");
		else
			printf("Setting ICC Tag Failed\n");
	}

	/* Now save palette */

	uint16_t* R = new uint16_t[256];
	uint16_t* G = new uint16_t[256];
	uint16_t* B = new uint16_t[256];
	TIFFGetField(pTif, TIFFTAG_COLORMAP, &R, &G, &B);


	for(y = 0; y < 256; y++)
	{
		R[y] = pImage->GetPalette()[y].R * 256;
		G[y] = pImage->GetPalette()[y].G * 256;
		B[y] = pImage->GetPalette()[y].B * 256;
	}

	TIFFSetField(pTif, TIFFTAG_COLORMAP, R, G, B);

	// allocate memory for buffer
	int BytesInBuffer = (int)TIFFScanlineSize(pTif);
	buffer = new uint8_t[BytesInBuffer];


	//write image rows
	for (y = 0; y < pImage->GetHeight(); y++)
	{
		memcpy(buffer, pImage->GetLineArray()[y], BytesInBuffer);
		TIFFWriteScanline(pTif, buffer, y, 0);
	}

	TIFFClose(pTif);
	delete buffer;
}

/*
 * Writes high color image
 */
void CTiffImageWriter::WriteImage(CHiColorImage* pImage, char* fileName)
{
	TIFF * pTif;
	uint8_t * buffer;
	int x, y;

	pTif = TIFFOpen(fileName, "w");

	//Set TAGs
	SetCommonFields(pImage, pTif);
	if(pImage->GetBitsPerPixel() == 24)
		TIFFSetField(pTif, TIFFTAG_SAMPLESPERPIXEL, 3);
	else
		TIFFSetField(pTif, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(pTif, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(pTif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	// ICC Colour Profile, If Present

	if(pImage->GetIccLength() != 0)
	{
		if(TIFFSetField(pTif, TIFFTAG_ICCPROFILE, pImage->GetIccLength(), pImage->GetIccProfile()) == 1)
			printf("Setting ICC Tag Succeeded\n");
		else
			printf("Setting ICC Tag Failed\n");
	}

	// allocate memory for buffer
	int BytesInBuffer = (int)TIFFScanlineSize(pTif);
	buffer = new uint8_t[BytesInBuffer];

	//write image rows
	for (y = 0; y < pImage->GetHeight(); y++)
	{
		if(pImage->GetBitsPerPixel() == 24)
		{
			for(x = 0; x < pImage->GetWidth(); x++)
			{
				buffer[x * 3 + 0] = pImage->GetLineArray()[y][x * 4 + 2]; //swap colors
				buffer[x * 3 + 1] = pImage->GetLineArray()[y][x * 4 + 1];
				buffer[x * 3 + 2] = pImage->GetLineArray()[y][x * 4 + 0];
			}
			TIFFWriteScanline(pTif, buffer, y, 0);
		}
		else
		{
			//TIFFWriteScanline(pTif, pImage->GetLineArray()[y], y, 0);
			for(x = 0; x < pImage->GetWidth(); x++)
			{
				buffer[x * 4 + 0] = pImage->GetLineArray()[y][x * 4 + 2]; //swap colors
				buffer[x * 4 + 1] = pImage->GetLineArray()[y][x * 4 + 1];
				buffer[x * 4 + 2] = pImage->GetLineArray()[y][x * 4 + 0];
				buffer[x * 4 + 3] = pImage->GetLineArray()[y][x * 4 + 3];
			}
			TIFFWriteScanline(pTif, buffer, y, 0);
		}
	}


	TIFFClose(pTif);
	delete [] buffer;
}

void CTiffImageWriter::SetCommonFields(CImage* pImage, TIFF* pTif)
{
	TIFFSetField(pTif, TIFFTAG_IMAGEWIDTH, pImage->GetWidth());
	TIFFSetField(pTif, TIFFTAG_IMAGELENGTH, pImage->GetHeight());
	TIFFSetField(pTif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
//	TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
	TIFFSetField(pTif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(pTif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(pTif, TIFFTAG_XRESOLUTION, pImage->GetXRes());
    TIFFSetField(pTif, TIFFTAG_YRESOLUTION, pImage->GetYRes());
    TIFFSetField(pTif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
}


} //end namespace