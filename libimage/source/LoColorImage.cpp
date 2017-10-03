#include "LoColorImage.h"
#include <math.h>
#include "extramath.h"

using namespace PRImA;

// CONSTRUCTION

/*
 * Image class for 8 bit colour images.
 * Deprecated: Use OpenCV image classes
 *
 *
 */

CLoColorImage::CLoColorImage()
{
	Init();
}

CLoColorImage::~CLoColorImage()
{

}

// METHODS

bool CLoColorImage::Create(int Width, int Height, RGBCOLOUR BackColour, const char * Name, bool reverse /*= false*/)
{
	int i, x, y;

	m_BitsPerPixel = 8; //Has to be set before calling parent create
	//Initialize the variables by calling parent
	if (!CImage::Create(Width, Height, BackColour, Name)) return false;

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
	for (y = 0; y < m_Height; y++)
	{
		for(x = 0; x < m_Width; x++)
			m_pLineArray[y][x] = 0;
	}

	return true;
}

void CLoColorImage::Clear(RGBCOLOUR col)
{
	int x,y;

	uint8_t ClosestIndex = 0;
	int16_t CurrentIndex;
	double  ClosestDist  = sqrt((double) ((col.R - m_Palette[0].R) * (col.R - m_Palette[0].R)) + ((col.G - m_Palette[0].G) * (col.G - m_Palette[0].G)) + ((col.B - m_Palette[0].B) * (col.B - m_Palette[0].B)));
	double  CurrentDist;

	for(CurrentIndex = 0; CurrentIndex <= 256; CurrentIndex++)
	{
		CurrentDist = sqrt((double) ((col.R - m_Palette[CurrentIndex].R) * (col.R - m_Palette[CurrentIndex].R)) + ((col.G - m_Palette[CurrentIndex].G) * (col.G - m_Palette[CurrentIndex].G)) + ((col.B - m_Palette[CurrentIndex].B) * (col.B - m_Palette[CurrentIndex].B)));
		if(CurrentDist < ClosestDist)
		{
			ClosestDist  = CurrentDist;
			ClosestIndex = (uint8_t)CurrentIndex;
		}
	}

	for (x=0; x<m_Width; x++)
		for (y=0; y<m_Height; y++)
			m_pLineArray[y][x] = ClosestIndex;
}

#ifdef _MSC_VER
void CLoColorImage::FillBitmapInfo()
{
	//Create a BITMAPINFO structure
	if(m_pBitmapInfo != NULL) 
		delete m_pBitmapInfo;
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
	m_pBitmapInfo->bmiHeader.biClrUsed = 256; //Maximum number of colours used, as specified by biBitCount
	m_pBitmapInfo->bmiHeader.biClrImportant = 256; //All colours are required
	RGBQUAD * pPalette = (RGBQUAD *)((LPSTR)m_pBitmapInfo + (WORD)(m_pBitmapInfo->bmiHeader.biSize));
	for (int i = 0; i < 256; i++)
	{
		pPalette[i].rgbRed = m_Palette[i].R;
		pPalette[i].rgbGreen = m_Palette[i].G;
		pPalette[i].rgbBlue = m_Palette[i].B;
		pPalette[i].rgbReserved = 255;
	}
}
#endif

//RGBCOLOUR * CLoColorImage::GetPalette()
//{
//	return m_Palette;
//}

uint8_t CLoColorImage::GetPaletteColor(int x, int y)
{
	return m_pLineArray[y][x];
}

RGBCOLOUR CLoColorImage::GetRGBColor(int x, int y)
{
	return m_Palette[m_pLineArray[y][x]];
}

void CLoColorImage::SetPalette(RGBCOLOUR * Pal)
{
	int16_t i;

	for(i = 0; i < 256; i++)
		m_Palette[i] = Pal[i];
}

void CLoColorImage::SetPaletteColor(int x, int y, uint8_t col)
{
	m_pLineArray[y][x] = col;
}

void CLoColorImage::SetRGBColor(int x, int y, RGBCOLOUR col)
{
	uint8_t ClosestIndex = 0;
	int16_t CurrentIndex;
	double  ClosestDist  = sqrt((double) ((col.R - m_Palette[0].R) * (col.R - m_Palette[0].R)) + ((col.G - m_Palette[0].G) * (col.G - m_Palette[0].G)) + ((col.B - m_Palette[0].B) * (col.B - m_Palette[0].B)));
	double  CurrentDist;

	for(CurrentIndex = 0; CurrentIndex <= 256; CurrentIndex++)
	{
		CurrentDist = sqrt((double) ((col.R - m_Palette[CurrentIndex].R) * (col.R - m_Palette[CurrentIndex].R)) + ((col.G - m_Palette[CurrentIndex].G) * (col.G - m_Palette[CurrentIndex].G)) + ((col.B - m_Palette[CurrentIndex].B) * (col.B - m_Palette[CurrentIndex].B)));
		if(CurrentDist < ClosestDist)
		{
			ClosestDist  = CurrentDist;
			ClosestIndex = (uint8_t)CurrentIndex;
		}
	}

	m_pLineArray[y][x] = ClosestIndex;
}

CImage * CLoColorImage::CreateNewImage()
{
	CLoColorImage * img = new CLoColorImage();
	img->SetPalette(this->GetPalette());
	return img;
}

/*
 * Creates a deep copy of this image.
 */
CImage * CLoColorImage::Clone(bool reverse /*= false*/)
{
	CLoColorImage * copy = new CLoColorImage();

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

void CLoColorImage::DrawLine(int X1, int Y1, int X2, int Y2, uint8_t colour, bool isothetic /*= false */)
{
	if(X1 == X2) // Vertical
	{
		int y;

		if(X1 >= 0 && X1 < m_Width)
		{
			for(y = CExtraMath::Max(CExtraMath::Min(Y1, Y2), 0); y <= CExtraMath::Min(CExtraMath::Max(Y1, Y2), (m_Height - 1)); y++)
				SetPaletteColor(X1, y, colour);
		}
	}
	else if(Y1 == Y2) // Horizontal
	{
		int x;

		if(Y1 >= 0 && Y1 < m_Height)
		{
			for(x = CExtraMath::Max(CExtraMath::Min(X1, X2), 0); x <= CExtraMath::Min(CExtraMath::Max(X1, X2), (m_Width - 1)); x++)
			{
				if(x >= 0 && x < m_Width)
					SetPaletteColor(x, Y1, colour);
			}
		}
	}
	else // Diagonal
	{
		if(abs(X2 - X1) >= abs(Y2 - Y1)) // Diagonal Line Closer To Horizontal
		{
			int x, y, yOld;

			if(X2 < X1)
			{
				int TempX, TempY;

				TempX = X1;
				TempY = Y1;
				X1 = X2;
				Y1 = Y2;
				X2 = TempX;
				Y2 = TempY;
			}

			double Gradient = double(Y2 - Y1) / double(X2 - X1);

			yOld = -1;
			for(x = X1; x <= X2; x++)
			{
				y = int(double(x - X1) * Gradient) + Y1;

				if(y >= 0 && y < m_Height && x >= 0 && x < m_Width)
				{
					SetPaletteColor(x, y, colour);

					//If isothetic is used, draw an extra pixel if y changed
					if (isothetic && yOld != y && yOld >= 0 
						&& x-1 >= 0 && x-1 < m_Width)
					{
						SetPaletteColor(x-1, y, colour);
					}
				}
				yOld = y;
			}
		}
		else // Diagonal Line Closer To Vertical
		{
			int x, y, xOld;

			if(Y2 < Y1)
			{
				int TempX, TempY;

				TempX = X1;
				TempY = Y1;
				X1 = X2;
				Y1 = Y2;
				X2 = TempX;
				Y2 = TempY;
			}

			double Gradient = double(X2 - X1) / double(Y2 - Y1);

			xOld = -1;
			for(y = Y1; y <= Y2; y++)
			{
				x = int(double(y - Y1) * Gradient) + X1;

				if(y >= 0 && y < m_Height && x >= 0 && x < m_Width)
				{
					SetPaletteColor(x, y, colour);

					//If isothetic is used, draw an extra pixel if x changed
					if (isothetic && xOld != x && xOld >= 0 
						&& y-1 >= 0 && y-1 < m_Height)
					{
						SetPaletteColor(x, y-1, colour);
					}
				}
				xOld = x;
			}
		}
	}
}