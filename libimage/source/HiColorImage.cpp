#include <stdlib.h>

#include "HiColorImage.h"

#include "ExtraMath.h"
#include "Compatibility.h"

using namespace PRImA;

/*
 * Image class for 24/32 bit colour images.
 * Deprecated: Use OpenCV image classes
 *
 *
 */

// CONSTRUCTION
CHiColorImage::CHiColorImage()
{
	Init();
}

CHiColorImage::~CHiColorImage()
{

}

// METHODS

void CHiColorImage::AndOffset(CBiLevelImage * b, int offx, int offy)
{
	int x, y;

	for(y = 0; y < GetHeight(); y++)
	{
		for(x = 0; x < GetWidth(); x++)
		{
			if(x - offx < 0 || x - offx >= b->GetWidth() || y - offy < 0 || y - offy >= b->GetHeight())
				SetRGBColor(x, y, RGBWHITE);
			else
				if(b->IsWhite(x - offx, y - offy))
					SetRGBColor(x, y, RGBWHITE);
		}
	}
}

bool CHiColorImage::Create(int Width, int Height, RGBCOLOUR BackColour, const char * Name, bool reverse /*= false*/)
{
	int i, x, y;

	m_BitsPerPixel = 32; //Has to be set before calling parent create
	//Initialize the variables by calling parent
	if (!CImage::Create(Width, Height, BackColour, Name)) 
		return false;

	uint8_t Red,Green,Blue;
	Red = BackColour.R;
	Green = BackColour.G;
	Blue = BackColour.B;
	//Allocate Memory for everything
	m_pArray = new uint8_t[m_BytesPerLine * m_Height];
	m_pLineArray = new uint8_t*[m_Height];
	m_pLineArray[m_Height - 1] = m_pArray;
	for(i = 0; i < m_Height; i++)
		m_pLineArray[i] = &(m_pArray[m_BytesPerLine * i]);

	//Initialize
	for(y = 0; y < m_Height; y++)
	{
		for(x = 0; x < m_Width; x++)
		{
			m_pLineArray[y][x*4+POS_RED]   = Red;
			m_pLineArray[y][x*4+POS_GREEN] = Green;
			m_pLineArray[y][x*4+POS_BLUE]  = Blue;
			m_pLineArray[y][x*4+POS_ALPHA] = 255;
		}
	}

	m_nForeColour.R = 255;
	m_nForeColour.G = 0;
	m_nForeColour.B = 0;

	return true;
}

void CHiColorImage::Clear(RGBCOLOUR colour)
{
	int x,y;

	for (x=0; x<m_Width; x++)
		for (y=0; y<m_Height; y++)
		{
			PrivateSetR(x, y, colour.R);
			PrivateSetG(x, y, colour.G);
			PrivateSetB(x, y, colour.B);
		}
}

#ifdef GTK
GdkPixbuf * CHiColorImage::CreatePixbuf()
{
	return gdk_pixbuf_new_from_data(m_pArray, GDK_COLORSPACE_RGB, true, 8, m_nWidth, m_nHeight, m_BytesPerLine, NULL, NULL);
}
#endif // GTK

void CHiColorImage::DrawLine(int x1, int y1, int x2, int y2)
{
	DrawLine(x1, y1, x2, y2, 1);
}

void CHiColorImage::DrawLine(int x1, int y1, int x2, int y2, int Width)
{
	int maxSide = CExtraMath::Max(abs(x2-x1), abs(y2-y1));
	float StepX = (x2-x1)/(float)maxSide;
	float StepY = (y2-y1)/(float)maxSide;
	int x, y;

	if(x1 == x2)
	{
		if(y2 < y1)
		{
			y  = y1;
			y1 = y2;
			y2 = y;
		}

		DrawRect(CExtraMath::Max(x1 - (Width / 2), 0), CExtraMath::Max(y1 - (Width / 2), 0), CExtraMath::Min((long) (x2 - (Width / 2) + Width - 1), (long) (m_Width - 1)), CExtraMath::Min((long) (y2 - (Width / 2) + Width - 1), (long) (m_Height - 1)));
	}
	else if(y1 == y2)
	{
		if(x2 < x1)
		{
			x  = x1;
			x1 = x2;
			x2 = x;
		}

		DrawRect(CExtraMath::Max(x1 - (Width / 2), 0), CExtraMath::Max(y1 - (Width / 2), 0), CExtraMath::Min((long) (x2 - (Width / 2) + Width - 1), (long) (m_Width - 1)), CExtraMath::Min((long) (y2 - (Width / 2) + Width - 1), (long) (m_Height - 1)));
	}
	else
	{
		for (int i = 0; i < maxSide; i++)
		{
			x = x1 + (int)(i*StepX);
			y = y1 + (int)(i*StepY);
	
			DrawRect(CExtraMath::Max(x - (Width / 2), 0), CExtraMath::Max(y - (Width / 2), 0), CExtraMath::Min((long) (x - (Width / 2) + Width - 1), (long) (m_Width - 1)), CExtraMath::Min((long) (y - (Width / 2) + Width - 1), (long) (m_Height - 1)));
		}
	}

	return;
}


void CHiColorImage::DrawRect(int x1, int y1, int x2, int y2)
{
	int x, y;

	for (x = x1; x <= x2; x++)
	{
		for(y = y1; y <= y2; y++)
		{
			PrivateSetR(x,y,m_nForeColour.R);
			PrivateSetG(x,y,m_nForeColour.G);
			PrivateSetB(x,y,m_nForeColour.B);
		}
	}

	return;
}

#ifdef _MSC_VER
void CHiColorImage::FillBitmapInfo()
{
	//Create a BITMAPINFO structure
	if (m_pBitmapInfo != NULL) delete m_pBitmapInfo;
	m_pBitmapInfo = new BITMAPINFO;
	m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo->bmiHeader.biWidth = m_Width;
	m_pBitmapInfo->bmiHeader.biHeight = m_Height;
	m_pBitmapInfo->bmiHeader.biPlanes = 1;
	m_pBitmapInfo->bmiHeader.biBitCount = 32;
	m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo->bmiHeader.biSizeImage = 0; //no need to set it, since it is a BI_RGB image
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 2835;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 2835;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;
}
#endif

int CHiColorImage::GetBaselineDist()
{
	return -1;	
}

RGBCOLOUR CHiColorImage::GetRGBColor(int x, int y)
{
	RGBCOLOUR col;
	col.R = GetR(x,y);
	col.G = GetG(x,y);
	col.B = GetB(x,y);
	col.A = GetA(x,y);
	return col;
}

bool CHiColorImage::operator=(CImage & B)
{
	int x,y;
	
	Create(B.GetWidth(),B.GetHeight(),RGBWHITE,"");
	SetBitsPerPixel(B.GetBitsPerPixel());

	for(y = 0; y < m_Height; y++)
	{
		for(x = 0; x < m_Width; x++)
			SetRGBColor(x,y,B.GetRGBColor(x,y));
	}

	return true;
}

void CHiColorImage::SetForegroundColor(RGBCOLOUR col)
{
	m_nForeColour = col;
}

void CHiColorImage::SetRGBColor(int x, int y, RGBCOLOUR col)
{
	PrivateSetR(x,y, col.R);
	PrivateSetG(x,y, col.G);
	PrivateSetB(x,y, col.B);
	PrivateSetA(x,y, col.A);
}

CImage * CHiColorImage::CreateNewImage()
{
	return new CHiColorImage();
}

/*
 * Creates a deep copy of this image.
 */
CImage * CHiColorImage::Clone(bool reverse /*= false*/)
{
	CHiColorImage * copy = new CHiColorImage();

	copy->Create(m_Width, m_Height, m_BackColour, m_sName, reverse);
	copy->SetBitsPerPixel(m_BitsPerPixel);

	int x,y;
	for(y = 0; y < m_Height; y++)
	{
		for(x = 0; x < m_Width; x++)
			copy->SetRGBColor(x,y,GetRGBColor(x,m_Height-1-y));
	}

	return copy;
}
