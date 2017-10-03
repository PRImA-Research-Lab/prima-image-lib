#include <math.h>

#include "Image.h"

#include "BiLevelImage.h"
#include "GreyScaleImage.h"
#include "HiColorImage.h"
#include "LoColorImage.h"
#include "typeinfo.h"

using namespace PRImA;

/*
 * Class CImage
 *
 * Base class for PRImA image classes.
 *
 * Deprecated: Use OpenCV image classes.
 *
 * CC 08.12.2009 - CreateBitmap: switched from CreateBitmap to CreateDIBSection because of memory shortage for large images
 */

CImage::CImage()
{
	Init();
}

CImage::~CImage()
{
	//delete m_ImageBitmap;
	DeleteObject(m_ImageHBitmap);
	delete [] m_sName;
	delete [] m_pArray;
	delete [] m_pLineArray;
}

void CImage::Init()
{
	m_ResX = 0;
	m_ResY = 0;
	m_sName = NULL;
	m_nICCLength = 0;
	m_sICCProfile = NULL;
	m_pArray = NULL;
	m_pLineArray = NULL;
#ifdef _MSC_VER
	m_pBitmapInfo = NULL;
	//m_ImageBitmap = NULL;
	m_ImageHBitmap = 0;
#endif
}

// CLASS METHODS

bool CImage::Create(int Width, int Height, RGBCOLOUR BackColour, const char * Name, bool reverse /*= false */ )
{
	m_BackColour = BackColour;
	if(BackColour.R == 0)
	{
		// This is just here to remove a warning.
	}
	m_Width=Width;
	m_Height=Height;
	m_NumberOfPixels=Width*Height;
	if(m_BitsPerPixel == 24)
	{
		// We always store 32-bits in memory, but only save the correct amount
		m_BytesPerLine = (m_Width*32 + 15) / 16 * 2;
	}
	else
		m_BytesPerLine = (m_Width*m_BitsPerPixel + 15) / 16 * 2;
	m_Info.RangeLeft = 0;
	m_Info.RangeRight = 255;
	delete [] m_sName;
	if(Name == NULL)
		m_sName = NULL;
	else
	{
		m_sName = new char[strlen(Name) + 1];
		memcpy(m_sName, Name, sizeof(char) * (strlen(Name) + 1));
	}
	return true;
}

void CImage::SetName(const char * name)
{
	delete [] m_sName;
	if(name == NULL)
		m_sName = NULL;
	else
	{
		m_sName = new char[strlen(name) + 1];
		memcpy(m_sName, name, sizeof(char) * (strlen(name) + 1));
	}
}

/*bool CImage::DetectBaselineDist(int)
{
	return false;
}*/

void CImage::Frame(int, bool)
{
}

int CImage::GetBaselineDist()
{
	return -1;	
}

#ifdef _MSC_VER
/*
 * Creates a HBITMAP from the image bit array.
 *
 * CC 08.12.2009 - CreateBitmap: switched from CreateBitmap to CreateDIBSection because of memory shortage for large images
 */
HBITMAP CImage::CreateBitmap()
{
	//delete m_ImageBitmap;
	DeleteObject(m_ImageHBitmap);

	//Recreate the m_ImageBitmap based on the current resolution/colour depth
	CDC dc;
	dc.CreateCompatibleDC(NULL);

	int bpp=dc.GetDeviceCaps(BITSPIXEL);
	//int planes=dc.GetDeviceCaps(PLANES);
	//m_ImageBitmap->DeleteObject(); //TODO needed?

	//Bitmapinfo of new bitmap (DIB)
	BITMAPINFO * dibBitmapInfo = new BITMAPINFO;
	dibBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	dibBitmapInfo->bmiHeader.biWidth = m_Width;
	dibBitmapInfo->bmiHeader.biHeight = m_Height;
	dibBitmapInfo->bmiHeader.biPlanes = 1;
	dibBitmapInfo->bmiHeader.biBitCount = 32;
	dibBitmapInfo->bmiHeader.biCompression = BI_RGB;
	dibBitmapInfo->bmiHeader.biSizeImage = 0; //no need to set it, since it is a BI_RGB image
	dibBitmapInfo->bmiHeader.biXPelsPerMeter = 2835;
	dibBitmapInfo->bmiHeader.biYPelsPerMeter = 2835;
	dibBitmapInfo->bmiHeader.biClrUsed = 0;
	dibBitmapInfo->bmiHeader.biClrImportant = 0;

	void ** ppvBits = NULL;

	//Create empty bitmap (DIB)
	//TODO usage dependent on image type (DIB_RGB_COLORS or DIB_PAL_COLORS)
	m_ImageHBitmap = CreateDIBSection(	dc.GetSafeHdc(), 
										dibBitmapInfo, //m_pBitmapInfo, 
										DIB_RGB_COLORS, //m_BitsPerPixel >= 24 ? DIB_RGB_COLORS : DIB_PAL_COLORS,
										ppvBits,	//pointer to bit-array (out)
										NULL, 0);

	if(m_ImageHBitmap == 0)
	{
		//CC 24.01.2011 - Not a good idea to show a message box from here
		//char * buffer = new char[512];
		//sprintf(buffer, "Not enough memory! Close documents and try again or restart the tool.\n(Failed to create HBITMAP object in CImage (w: %d, h: %d, bpp: %d))", m_Width,m_Height,bpp);
		//CString str(buffer);
		//::MessageBox(NULL, str.GetBuffer(), _T("Error"), MB_OK);
		//delete [] buffer;
		//TRACE ("Failed to create HBITMAP object in CImage\n");
		return NULL;
	}

	/*if(!m_ImageBitmap->CreateBitmap(m_Width,m_Height,planes,bpp,NULL))
	//if(!m_ImageBitmap->CreateCompatibleBitmap(&dc,m_Width,m_Height))
	{
		char * buffer = new char[256];
		sprintf(buffer, "Failed to create CBitmap object in CImage (w: %d, h: %d, bpp: %d)", m_Width,m_Height,bpp);
		CString str(buffer);
		::MessageBox(NULL, str.GetBuffer(), _T("Error"), MB_OK);
		delete [] buffer;
		TRACE ("Failed to create CBitmap object in CImage\n");
		return NULL;
	}
	if(m_ImageBitmap->GetSafeHandle() == NULL) //TODO should't be needed anymore (the test above should handle it)
	{
		char * buffer = new char[256];
		sprintf(buffer, "Could not get bitmap handle in CImage (w: %d, h: %d, bpp: %d)", m_Width,m_Height,bpp);
		CString str(buffer);
		::MessageBox(NULL, str.GetBuffer(), _T("Error"), MB_OK);
		delete [] buffer;
		TRACE ("Failed to create CBitmap object in CImage::UpdateNormal()\n");
		return NULL;
	}
	*/
	//Obtain the first version of the image. Here are functions which each derived image class defines.
	//These functions work in the colour depth of the CImage derived class.

	uint8_t * ArrayToUse = NULL;

	CImage * img = this;
	//Workaround for display problems of low colour and grey-scale images
	if (typeid(*img) == typeid(CLoColorImage) || typeid(*img) == typeid(CGreyScaleImage))
	{
		//Create a new pixel array with adjusted width and copy the pixel values
		int adjustedWidth = m_Width + (8-(m_Width % 8)); //CC 17.11.2009 workaround for 'mod 8' problem
		ArrayToUse = new uint8_t[m_Height*adjustedWidth];
		for (int y=0; y<m_Height; y++)
			for (int x=0; x<m_Width; x++)
				ArrayToUse[y*adjustedWidth + x] = m_pLineArray[m_Height-1-y][x];
	}
	else //Hi-colour and bi-level images
	{
		ArrayToUse = m_pArray;
	}

	//BitmapInfo of the bit-array
	FillBitmapInfo();

	//TODO remove!!!
	/*BITMAPINFO* tempBitmapInfo = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256];
	tempBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	//int adjustedWidth = m_Width + (8-(m_Width % 8)); //CC 07.01.2009 workaround for 'mod 8' problem (copied from BiLevelImage)
	tempBitmapInfo->bmiHeader.biWidth = adjustedWidth;
	//tempBitmapInfo->bmiHeader.biWidth = m_Width;
	tempBitmapInfo->bmiHeader.biHeight = m_Height;
	tempBitmapInfo->bmiHeader.biPlanes = 1;
	tempBitmapInfo->bmiHeader.biBitCount = 8;
	tempBitmapInfo->bmiHeader.biCompression = BI_RGB;
	tempBitmapInfo->bmiHeader.biSizeImage = 0; //no need to set it, since it is a BI_RGB image
	tempBitmapInfo->bmiHeader.biXPelsPerMeter = 2835;
	tempBitmapInfo->bmiHeader.biYPelsPerMeter = 2835;
	tempBitmapInfo->bmiHeader.biClrUsed = 256; //Maximum number of colours used, as specified by biBitCount
	tempBitmapInfo->bmiHeader.biClrImportant = 256; //All colours are required
	RGBQUAD * pPalette = (RGBQUAD *)((LPSTR)tempBitmapInfo + (WORD)(tempBitmapInfo->bmiHeader.biSize));
	for (int i = 0; i < 256; i++)
	{
		pPalette[i].rgbRed = ((CLoColorImage *)this)->GetPalette()[i].R;
		pPalette[i].rgbGreen =((CLoColorImage *)this)->GetPalette()[i].G;
		pPalette[i].rgbBlue = ((CLoColorImage *)this)->GetPalette()[i].B;
		pPalette[i].rgbReserved = 255;
	}*/


	/*
	//Set the DDB bitmap bits (device dependand bitmap), from the DIB bitmap bits (Device Independend Bitmap)
	int ret = ::SetDIBits(dc.GetSafeHdc(),// handle to DC
				(HBITMAP)m_ImageBitmap->GetSafeHandle(),	// handle to bitmap
				0,				// starting scan line
				m_Height,		// number of scan lines
				ArrayToUse,		// array of bitmap bits
				m_pBitmapInfo,	// bitmap data
				DIB_RGB_COLORS);// type of color indexes to use
	*/

	//Copy the bit-array to the empty bitmap
	//Set the DDB bitmap bits (device dependand bitmap), from the DIB bitmap bits (Device Independend Bitmap)
	int ret = ::SetDIBits(dc.GetSafeHdc(),// handle to DC
				m_ImageHBitmap,	// handle to target bitmap
				0,				// starting scan line
				m_Height,		// number of scan lines
				ArrayToUse,		// array of source bitmap bits
				m_pBitmapInfo,	// source bitmap data
				//tempBitmapInfo,
				DIB_RGB_COLORS);// type of color indexes to use (we use always DIB_RGB_COLORS)

	if (typeid(*img) == typeid(CLoColorImage) || typeid(*img) == typeid(CGreyScaleImage))
	{
		delete [] ArrayToUse;
	}

	return m_ImageHBitmap;
}
#endif

//unsigned CImage::GetBitsPerPixel()
//{
//	return m_BitsPerPixel;
//}

//void CImage::SetBitsPerPixel(unsigned BPP)
//{
//	m_BitsPerPixel = BPP;
//}

//unsigned CImage::GetWidth()
//{
//	return m_nWidth;
//}

/*
double CImage::GetColourDistance(RGBCOLOUR a, RGBCOLOUR b)
{
	double Result = sqrt(pow(a.R - b.R, 2) + pow(a.G - b.G, 2) + pow(a.B - b.B, 2));
	return Result;
}
*/

double CImage::GetColourDistance(RGBCOLOUR a, RGBCOLOUR b)
{
	double Result = double((a.R + a.G + a.B) - (b.R + b.G + b.B)) / 3.0;
	if(Result < 0)
		Result = -Result;
	return Result;
}

//unsigned CImage::GetHeight()
//{
//	return m_nHeight;
//}

void CImage::GetICCProfile(uint32_t * ICCLength, uint8_t ** ICCProfile)
{
	*ICCLength  = m_nICCLength;
	*ICCProfile = m_sICCProfile;
}

//float CImage::GetXRes()
//{
//	return m_ResX;
//}

//float CImage::GetYRes()
//{
//	return m_ResY;
//}

//void CImage::SetResolution(const float XRes, const float YRes)
//{
//	printf("SetResolution:");
//	printf("\tOld Resolution = %f x %f\n", m_ResX, m_ResY);
//	m_ResX = XRes;
//	m_ResY = YRes;
//	printf("\tNew Resolution = %f x %f\n", m_ResX, m_ResY);
//}

void CImage::SetICCProfile(uint32_t ICCLength, uint8_t * ICCProfile)
{
	m_nICCLength = ICCLength;
	delete [] m_sICCProfile;
	m_sICCProfile = new uint8_t[m_nICCLength];
	memcpy(m_sICCProfile, ICCProfile, sizeof(uint8_t) * m_nICCLength);
}

void CImage::InitArrays() {
	delete [] m_pArray;
	delete [] m_pLineArray;
	m_pArray = new uint8_t[m_BytesPerLine * m_Height];
	m_pLineArray = new uint8_t*[m_Height];
	m_pLineArray[m_Height - 1] = m_pArray;
}

/*
 * Creates an image of the same type copying the specified frame.
 */
CImage* CImage::CreateSubImage(int left, int top, int width, int height)
{
	if (left < 0 || left >= m_Width || top < 0 || top >= m_Height 
		|| width <= 0 || height <= 0)
		return NULL;

	CImage* img = CreateNewImage();

	img->Create(width, height, this->GetBackColour(), this->GetName());
	
	img->SetBitsPerPixel(this->GetBitsPerPixel());
	img->SetIccLength(this->GetIccLength());
	img->SetIccProfile(this->GetIccProfile());
	img->SetResolution(this->GetXRes(), this->GetYRes());

	//Memcopy line by line //TODO - use bits per pixel...
	//for (int y=0; y<height; y++)
	//	memcpy(img->GetLineArray()[y], m_pLineArray[top+y] + sizeof(unsigned char)*left, sizeof(unsigned char) * width);

	if (typeid(*img) == typeid(CLoColorImage))
	{
		CLoColorImage * loColSource = (CLoColorImage*)this;
		CLoColorImage * loColTarget = (CLoColorImage*)img;
		for (int y=0; y<height; y++)
			for (int x=0; x<width; x++)
				loColTarget->SetPaletteColor(x,y, loColSource->GetPaletteColor(left+x,top+y));
	}
	else
	{
		for (int y=0; y<height; y++)
			for (int x=0; x<width; x++)
				img->SetRGBColor(x,y, this->GetRGBColor(left+x,top+y));
	}

	return img;
}
