
#include "GreyScaleImage.h"

#include "Compatibility.h"

using namespace PRImA;

/*
 * Image class for greyscale images.
 * Deprecated: Use OpenCV image classes
 *
 *
 */

CGreyScaleImage::CGreyScaleImage()
{
	Init();
}

bool CGreyScaleImage::Create(int Width, int Height, RGBCOLOUR BackColour, const char * Name, bool reverse /*= false*/)
{
	int y;

	m_BitsPerPixel = 8; //Has to be set before calling parent create
	//Initialize the variables by calling parent
	if (!CImage::Create(Width, Height, BackColour, Name)) 
		return false;

	uint8_t Fill = BackColour.G; //a crude approximation of the grey value
	//Allocate Memory for everything
	m_pArray = new uint8_t[m_Width * m_Height];
	m_pLineArray = new uint8_t*[m_Height];
	m_pLineArray[0] = m_pArray;
	for(y = 1; y < m_Height; y++)
		m_pLineArray[y] = m_pLineArray[y-1] + m_Width;

	//Initialize
	memset(m_pArray, Fill, m_Width * m_Height * sizeof(uint8_t));

	return true;
}

#ifdef _MSC_VER
void CGreyScaleImage::FillBitmapInfo()
{
	//Create a BITMAPINFO structure
	if (m_pBitmapInfo != NULL) delete m_pBitmapInfo;
	m_pBitmapInfo = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256];
	m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	int adjustedWidth = m_Width + (8-(m_Width % 8)); //CC 07.01.2009 workaround for 'mod 8' problem (copied from BiLevelImage)
	m_pBitmapInfo->bmiHeader.biWidth = adjustedWidth;
	//m_pBitmapInfo->bmiHeader.biWidth = m_Width;
	m_pBitmapInfo->bmiHeader.biHeight = m_Height;
	m_pBitmapInfo->bmiHeader.biPlanes = 1;
	m_pBitmapInfo->bmiHeader.biBitCount = 8;
	m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo->bmiHeader.biSizeImage = 0; //no need to set it, since it is a BI_RGB image
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 2835;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 2835;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0; //Maximum number of colours used, as specified by biBitCount
	m_pBitmapInfo->bmiHeader.biClrImportant = 0; //All colours are required
	RGBQUAD * pPalette = (RGBQUAD *)((LPSTR)m_pBitmapInfo + (WORD)(m_pBitmapInfo->bmiHeader.biSize));
	for (int i = 0; i < 256; i++)
	{
		pPalette[i].rgbRed = pPalette[i].rgbGreen = pPalette[i].rgbBlue = i;
		pPalette[i].rgbReserved = 255;
	}
}
#endif

RGBCOLOUR CGreyScaleImage::GetRGBColor(int x, int y)
{
	RGBCOLOUR RGB = {m_pLineArray[y][x], m_pLineArray[y][x], m_pLineArray[y][x]};
	return RGB;
}

int CGreyScaleImage::GetGreyLevel(int x, int y)
{
	return m_pLineArray[y][x];
}

void CGreyScaleImage::SetRGBColor(int x, int y, RGBCOLOUR col)
{
	m_pLineArray[y][x] = (uint8_t)(0.3*((double)col.R) + 0.59*((double)col.G) + 0.11*((double)col.B));
}

void CGreyScaleImage::SetGreyLevel(int x, int y, int level)
{
	m_pLineArray[y][x] = (uint8_t)level;
}

CImage * CGreyScaleImage::CreateNewImage()
{
	return new CGreyScaleImage();
}

/*
 * Creates a deep copy of this image.
 */
CImage * CGreyScaleImage::Clone(bool reverse /*= false*/)
{
	CGreyScaleImage * copy = new CGreyScaleImage();

	copy->Create(m_Width, m_Height, m_BackColour, m_sName, reverse);
	copy->SetBitsPerPixel(m_BitsPerPixel);

	int x,y;
	for(y = 0; y < m_Height; y++)
	{
		for(x = 0; x < m_Width; x++)
			copy->SetRGBColor(x,y,GetRGBColor(x,y));
	}

	return copy;
}