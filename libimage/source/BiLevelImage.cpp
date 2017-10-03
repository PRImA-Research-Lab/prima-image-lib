#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "BiLevelImage.h"
#include "ExtraMath.h"
#include "Histogram.h"
#include "Compatibility.h"

using namespace PRImA;

/*
 * Image class for black and white images.
 * Deprecated: Use OpenCV image classes
 *
 * CC 02.11.2009 - optimized the flood fill algorithm
 *
 */

// Construction

CBiLevelImage::CBiLevelImage()
{
	CImage::Init();
	m_BackColour = RGBWHITE;
	m_NumberOfBlackPixels = -1L;
	m_NumberOfWhitePixels = -1L;
}

/*
 * Creates a copy of the given image.
 */
CBiLevelImage::CBiLevelImage(CBiLevelImage* img)
{
	Create(img->GetWidth(), img->GetHeight(), img->GetBackColour(), img->GetName());

	//Memcopy whole array (doesn't work yet but ist the fastest)
	//memcpy(m_pArray, img->GetArray(), sizeof(unsigned char) * m_Width * m_nHeight);

	//if (m_pLineArray != NULL) 
	//	delete [] m_pLineArray;
	//m_pLineArray = new uint8_t*[m_nHeight];

	//m_pLineArray[m_nHeight - 1] = m_pArray;
	//for(int y = m_nHeight - 2; y >= 0; y--)
	//	m_pLineArray[y] = m_pLineArray[y + 1] + (m_Width + m_Width % 2);

	//Pixel by pixel (does work but is slowest)
	//for (int y=0; y<m_nHeight; y++)
	//	for (int x=0; x<m_Width; x++)
	//		this->SetPixel(x,y,img->IsBlack(x,y));

	//Memcopy line by line
	for (int y=0; y<m_Height; y++)
		memcpy(m_pLineArray[y], img->GetLineArray()[y], sizeof(unsigned char) * m_Width);

	this->SetBitsPerPixel(img->GetBitsPerPixel());
	this->SetIccLength(img->GetIccLength());
	this->SetIccProfile(img->GetIccProfile());
	this->SetResolution(img->GetXRes(), img->GetYRes());

	//TODO more to copy?

	m_NumberOfBlackPixels = -1L;
	m_NumberOfWhitePixels = -1L;
}


CBiLevelImage::~CBiLevelImage()
{	
}

// Methods

void CBiLevelImage::And(CBiLevelImage * b)
{
	int x, y;

	for(y = 0; y < CExtraMath::Min(GetHeight(), b->GetHeight()); y++)
	{
		for(x = 0; x < CExtraMath::Min(GetWidth(), b->GetWidth()); x++)
		{
			if(b->IsWhite(x, y))
				SetWhite(x, y);
		}
	}
	m_NumberOfBlackPixels = -1L;
	m_NumberOfWhitePixels = -1L;
}

void CBiLevelImage::AndOffset(CBiLevelImage * b, int offx, int offy)
{
	int x, y;

	for(y = 0; y < GetHeight(); y++)
	{
		for(x = 0; x < GetWidth(); x++)
		{
			if(x - offx < 0 || x - offx >= b->GetWidth() || y - offy < 0 || y - offy >= b->GetHeight())
				SetWhite(x, y);
			else
				if(b->IsWhite(x - offx, y - offy))
					SetWhite(x, y);
		}
	}
	m_NumberOfBlackPixels = -1L;
	m_NumberOfWhitePixels = -1L;
}

/*
 * Clears the line defined by the given points 1 and 2.
 *
 * 'isothetic' - draw an isothetic line (only horizontal and vertical line segments)
 */
void CBiLevelImage::ClearLine(int X1, int Y1, int X2, int Y2, bool isothetic /*= false */)
{
	if(X1 == X2) // Vertical
	{
		int y;

		if(X1 >= 0 && X1 < m_Width)
		{
			for(y = CExtraMath::Max(CExtraMath::Min(Y1, Y2), 0); y <= CExtraMath::Min(CExtraMath::Max(Y1, Y2), (m_Height - 1)); y++)
				SetWhite(X1, y);
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
					SetWhite(x, Y1);
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
					SetWhite(x, y);

					//If isothetic is used, draw an extra pixel if y changed
					if (isothetic && yOld != y && yOld >= 0 
						&& x-1 >= 0 && x-1 < m_Width)
					{
						SetWhite(x-1, y);
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
					SetWhite(x, y);

					//If isothetic is used, draw an extra pixel if x changed
					if (isothetic && xOld != x && xOld >= 0 
						&& y-1 >= 0 && y-1 < m_Height)
					{
						SetWhite(x, y-1);
					}
				}
				xOld = x;
			}
		}
	}
	m_NumberOfBlackPixels = -1L;
	m_NumberOfWhitePixels = -1L;
}

/*
 * Clears the whole image.
 */
void CBiLevelImage::Clear(RGBCOLOUR colour)
{
	int x,y;

	uint8_t bw = colour.R == 255 && colour.G == 255 && colour.B == 255 ? WHITE : BLACK;

	for (x=0; x<m_Width; x++)
		for (y=0; y<m_Height; y++)
			m_pLineArray[y][x] = bw;

	m_NumberOfBlackPixels = -1L;
	m_NumberOfWhitePixels = -1L;
}

bool CBiLevelImage::Create(int Width, int Height, RGBCOLOUR BackColour, const char * Name, bool reverse /*= false*/)
{
	int y;
	m_BackColour = BackColour;

	m_BitsPerPixel = 1; //Has to be set before calling parent create
	//Initialize the variables by calling parent
	if (!CImage::Create(Width, Height, BackColour, Name)) return false;

	uint8_t Fill;
	if(BackColour.R > 128 || BackColour.G > 128 || BackColour.B > 128) Fill = WHITE; else Fill = BLACK;
	//Allocate Memory for everything

	int adjustedWidth = m_Width + (8-(m_Width % 8)); //CC 17.11.2009 workaround for 'mod 8' problem

	m_pArray = new unsigned char[adjustedWidth * m_Height];
	m_pLineArray = new unsigned char*[m_Height];

	memset(m_pArray, Fill, sizeof(unsigned char) * adjustedWidth * m_Height);

	if (reverse)
	{
		m_pLineArray[m_Height - 1] = m_pArray;
		for(y = m_Height - 2; y >= 0; y--)
			m_pLineArray[y] = m_pLineArray[y + 1] + adjustedWidth;
	}
	else
	{
		m_pLineArray[0] = m_pArray;
		for(y = 1; y < m_Height; y++)
			m_pLineArray[y] = m_pLineArray[y-1] + adjustedWidth;
	}

	return true;
}

/*bool CBiLevelImage::DetectBaselineDist()
{
	CHistogram * SumHistogram = new CHistogram();
	
	if(SumHistogram==NULL)
	{
		return false;
	}

	srand((unsigned int) time(NULL));

	int i;
	int StartX;
	int StripWidth = (int)(m_ResX / 3.0);

	if(StripWidth > m_Width)
	{
		return false;
	}
	
	m_BaselineDist = 0;

	for(i=0;i < 3;i++)
	{
		StartX = rand() % (m_Width - StripWidth + 1);

		CHistogram * HorzProjProf = new CHistogram();
		
		if(HorzProjProf == NULL)
		{
			return false;
		}
		
		if(HorzProjProf->CreateHorzProjProfVertStrip(this,StartX,StartX + StripWidth - 1)==false)
		{
			return false;
		}
		if(HorzProjProf->FindValleys()==false)
		{
			return false;			
		}
		if(HorzProjProf->FindMetaValleys()==false)
		{
			return false;
		}
		HorzProjProf->BreakProfile();
		HorzProjProf->FindBaseLines();
		if(i==0)
		{
			SumHistogram->CreateBaselines(HorzProjProf);
		}
		else
		{
			CHistogram * BaselinesHist = new CHistogram();
			BaselinesHist->CreateBaselines(HorzProjProf);
			SumHistogram->Add(BaselinesHist);
			delete BaselinesHist;
		}
		delete HorzProjProf;
	}

	if(SumHistogram->GetMaxIndex() < 3)
	{
		return false;
	}

	m_BaselineDist = SumHistogram->GetMaxIndex();
	delete SumHistogram;
	return true;
}*/

#ifdef _MSC_VER
void CBiLevelImage::FillBitmapInfo()
{
	//Create a BITMAPINFO structure
	if (m_pBitmapInfo != NULL) 
		delete m_pBitmapInfo;
	m_pBitmapInfo = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2];
	m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	int adjustedWidth = m_Width + (8-(m_Width % 8)); //CC 17.11.2009 workaround for 'mod 8' problem
	m_pBitmapInfo->bmiHeader.biWidth = adjustedWidth;
	m_pBitmapInfo->bmiHeader.biHeight = m_Height;
	m_pBitmapInfo->bmiHeader.biPlanes = 1;
	m_pBitmapInfo->bmiHeader.biBitCount = 8;
	m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo->bmiHeader.biSizeImage = 0; //no need to set it, since it is a BI_RGB image
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 2835;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 2835;
	m_pBitmapInfo->bmiHeader.biClrUsed = 2; //Maximum number of colours used, as specified by biBitCount
	m_pBitmapInfo->bmiHeader.biClrImportant = 2; //All colours are required
	RGBQUAD * pPalette = (RGBQUAD *)((LPSTR)m_pBitmapInfo + (WORD)(m_pBitmapInfo->bmiHeader.biSize));
	pPalette[0].rgbRed = pPalette[0].rgbGreen = pPalette[0].rgbBlue = 0;
	pPalette[1].rgbRed = pPalette[1].rgbGreen = pPalette[1].rgbBlue = 255;
	pPalette[0].rgbReserved = pPalette[1].rgbReserved = 255;
}
#endif

/*
 * Flood fill (line wise).
 */
void CBiLevelImage::FloodFill(int x, int y, bool Black)
{
	queue<CPoint> points;
	//CLayoutPolygon RPL;
	unsigned W, E, XIter;
	bool Continue;

	if(IsBlack(x, y) == Black)
		return;

	points.push(CPoint(x, y));
	//RPL.AddPoint(x, y);

	while(!points.empty())
	{
		CPoint currPoint = points.front();
		points.pop();
		if(IsBlack(currPoint.x, currPoint.y) != Black)
		{
			W = currPoint.x;
			E = currPoint.x;

			//Find most western point
			Continue = true;
			while(Continue)
			{
				if(W == 0)
					Continue = false;
				else if(IsBlack(W - 1, currPoint.y) == Black)
					Continue = false;

				if(Continue)
					W--;
			}

			//Find most eastern point
			Continue = true;
			while(Continue)
			{
				if(E == m_Width - 1)
					Continue = false;
				else if(IsBlack(E + 1, currPoint.y) == Black)
					Continue = false;

				if(Continue)
					E++;
			}

			//Draw line from west to east and add neighbors to the queue
			int xRunAbove = -1;
			int xRunBelow = -1;
			for(XIter = W; XIter <= E; XIter++)
			{
				//Set pixel
				if(Black)
					SetBlack(XIter, currPoint.y);
				else
					SetWhite(XIter, currPoint.y);

				//Row above
				if(currPoint.y > 0)
				{
					//Only add the rightmost point of each run to the queue
					if(IsBlack(XIter, currPoint.y - 1) == Black)
					{
						if (xRunAbove >= 0)
							points.push(CPoint(xRunAbove, currPoint.y - 1));
						xRunAbove = -1;
					}
					else
						xRunAbove = XIter;
				}

				//Row below
				if(currPoint.y < m_Height - 1)
				{
					//Only add the rightmost point of each run to the queue
					if(IsBlack(XIter, currPoint.y + 1) == Black)
					{
						if (xRunBelow >= 0)
							points.push(CPoint(xRunBelow, currPoint.y + 1));
						xRunBelow = -1;
					}
					else
						xRunBelow = XIter;
				}
			}
			//Treat the last runs
			if(xRunAbove >= 0 && currPoint.y > 0)
				points.push(CPoint(xRunAbove, currPoint.y - 1));
			if(xRunBelow >= 0 && currPoint.y < m_Height + 1)
				points.push(CPoint(xRunBelow, currPoint.y + 1));
		}
	}
	m_NumberOfBlackPixels = -1L;
	m_NumberOfWhitePixels = -1L;
}

// CBiLevelImage::Frame Creates an outline at 
// the edge of the image in the given colour.

void CBiLevelImage::Frame(int FrameWidth, bool White)
{
	int x,y;
	
	for(x = 0; x < m_Width; x++)
	{
		for(y = 0; y < FrameWidth; y++)
		{
			if(White)
				SetWhite(x, y);
			else
				SetBlack(x, y);
		}

		for(y = m_Width - FrameWidth; y < m_Width; y++)
		{
			if(White)
				SetWhite(x, y);
			else
				SetBlack(x, y);
		}
	}

	for(y = 0; y < m_Height; y++)
	{
		for(x = 0; x < FrameWidth; x++)
		{
			if(White)
				SetWhite(x, y);
			else
				SetBlack(x, y);
		}

		for(x = m_Height - FrameWidth; x < m_Height; x++)
		{
			if(White)
				SetWhite(x, y);
			else
				SetBlack(x, y);
		}
	}
	m_NumberOfBlackPixels = -1L;
	m_NumberOfWhitePixels = -1L;
}

int CBiLevelImage::GetBaselineDist()
{
	return m_BaselineDist;	
}

RGBCOLOUR CBiLevelImage::GetRGBColor(int x, int y)
{
	RGBCOLOUR Black = {0,0,0, 255};
	RGBCOLOUR White = {255,255,255, 255};
	if(IsBlack(x,y))
		return Black;
	else
		return White;
}

bool CBiLevelImage::IsBlack(int x, int y)
{
	if(x < 0)
	{
		return false;
	}
	if(x > m_Width - 1)
	{
		return false;
	}
	if(y < 0)
	{
		return false;
	}
	if(y > m_Height - 1)
	{
		return false;
	}
	return(m_pLineArray[y][x]==BLACK);
}

bool CBiLevelImage::IsWhite(int x, int y)
{
	return(m_pLineArray[y][x]==WHITE);
}

// ModifiedKFill Is Noise Reduction Technique From:
//
// K. Chinnasarn, Y. Rangsanseri, P. Thitimajshima, "Removing Salt-and-Pepper 
// Noise in Text/Graphics Images," IEEE Asia-Pacific Conference on Circuits and 
// Systems, 1998, pp. 459 - 462

bool CBiLevelImage::ModifiedKFill(int KVal)
{
	int x, y;
	int dx,dy,i;
	int CoreOnCount;
	int NeighCount;
	int CornerCount;
	int CCCount;
	bool LastWasBlack;
	double OnThresh = double((KVal - 2) * (KVal - 2)) / 2.0;
	
	for(y = 0; y < m_Height - KVal + 1; y++)
	{
		for(x = 0; x < m_Width - KVal + 1; x++)
		{
			CoreOnCount = 0;
			NeighCount = 0;
			CornerCount = 0;
			CCCount = 0;
			
			for(dy = 1; dy < KVal - 1; dy++)
			{
				for(dx = 1; dx < KVal - 1; dx++)
				{
					if(IsBlack(x + dx, y + dy))
					{
						CoreOnCount++;
					}
				}
			}
			
			if(double(CoreOnCount) >= OnThresh)
			{
				LastWasBlack = true;
				
				dx = -1;
				dy = 0;
				
				for(i = 0; i < 4 * (KVal - 1); i++)
				{
					if(i <= KVal - 1)
						dx++;
					else if(i <= 2 * (KVal - 1))
						dy++;
					else if(i <= 3 * (KVal - 1))
						dx--;
					else if(i <= 4 * (KVal - 1))
						dy--;
					
					if(IsWhite(x + dx, y + dy))
					{
						NeighCount++;
						
						if(LastWasBlack)
						{
							CCCount++;
							LastWasBlack = false;
						}
						
						if(i % (KVal - 1) == 0)
						{
							CornerCount++;
						}
					}
					else
					{
						LastWasBlack = true;
					}
				}
				
				if(LastWasBlack == false)
				{
					if(IsWhite(x,y))
					{
						CCCount--;
					}
				}
				
				if
				(
					CCCount == 1 && 
					(
						(NeighCount > ((3 * KVal)-4)) || 
						(
							(NeighCount == ((3 * KVal) - 4)) && 
							(CornerCount == 2)
						)
					)
				)
				{
					for(dy = 1; dy < KVal - 1; dy++)
					{
						for(dx = 1; dx < KVal - 1; dx++)
						{
							SetWhite(x + dx, y + dy);
						}
					}
				}
				else
				{
					for(dy = 1; dy < KVal - 1; dy++)
					{
						for(dx = 1; dx < KVal - 1; dx++)
						{
							SetBlack(x + dx, y + dy);
						}
					}
				}
			}
			else
			{
				LastWasBlack = false;
				
				dx = -1;
				dy = 0;
				
				for(i = 0; i < 4 * (KVal - 1); i++)
				{
					if(i <= KVal - 1)
						dx++;
					else if(i <= 2 * (KVal - 1))
						dy++;
					else if(i <= 3 * (KVal - 1))
						dx--;
					else if(i <= 4 * (KVal - 1))
						dy--;
					
					if(IsBlack(x + dx, y + dy))
					{
						NeighCount++;
						
						if(!LastWasBlack)
						{
							CCCount++;
							LastWasBlack = true;
						}
						
						if(i % (KVal - 1) == 0)
						{
							CornerCount++;
						}
					}
					else
					{
						LastWasBlack = false;
					}
				}
				
				if(LastWasBlack == true)
				{
					if(IsBlack(x,y))
					{
						CCCount--;
					}
				}
				
				if
				(
					CCCount == 1 && 
					(
						(NeighCount > ((3 * KVal)-4)) || 
						(
							(NeighCount == ((3 * KVal) - 4)) && 
							(CornerCount == 2)
						)
					)
				)
				{
					for(dy = 1; dy < KVal - 1; dy++)
					{
						for(dx = 1; dx < KVal - 1; dx++)
						{
							SetBlack(x + dx, y + dy);
						}
					}
				}
				else
				{
					for(dy = 1; dy < KVal - 1; dy++)
					{
						for(dx = 1; dx < KVal - 1; dx++)
						{
							SetWhite(x + dx, y + dy);
						}
					}
				}
			}
		}
	}

	m_NumberOfBlackPixels = -1L;
	m_NumberOfWhitePixels = -1L;
	return true;
}

CBiLevelImage & CBiLevelImage::operator=(CBiLevelImage & B)
{
    int x,y;

    Create(B.GetWidth(),B.GetHeight(),RGBWHITE,"");

    for(y = 0; y < m_Height; y++)
    {
        for(x = 0; x < m_Width; x++)
		{
			if(B.IsBlack(x, y))
				SetBlack(x, y);
			else
				SetWhite(x, y);
		}
    }

	m_NumberOfBlackPixels = -1L;
	m_NumberOfWhitePixels = -1L;
    return *this;
}

CBiLevelImage & CBiLevelImage::operator=(CImage & B)
{
	int x,y;
	RGBCOLOUR Pixel;

	Create(B.GetWidth(), B.GetHeight(), RGBWHITE, "");

	for(y = 0; y < m_Height; y++)
	{
		for(x = 0; x < m_Width; x++)
		{
			Pixel = B.GetRGBColor(x, y);
			if((Pixel.R + Pixel.G + Pixel.B) < 384)
				SetBlack(x, y);
			else
				SetWhite(x, y);
		}
	}

	m_NumberOfBlackPixels = -1L;
	m_NumberOfWhitePixels = -1L;
	return *this;
}

void CBiLevelImage::SetBlack(const int x, const int y)
{
	m_pLineArray[y][x] = BLACK;
	m_NumberOfBlackPixels = -1L;
	m_NumberOfWhitePixels = -1L;
}

void CBiLevelImage::SetPixel(const int x, const int y, bool black)
{
	if (black)
		m_pLineArray[y][x] = BLACK;
	else
		m_pLineArray[y][x] = WHITE;
	m_NumberOfBlackPixels = -1L;
	m_NumberOfWhitePixels = -1L;
}


void CBiLevelImage::SetLine(int X1, int Y1, int X2, int Y2)
{
	SetLine(X1,Y1,X2,Y2,true);
}

void CBiLevelImage::SetLine(int X1, int Y1, int X2, int Y2, bool black)
{
//	printf("SetLine(X1=%d, Y1=%d, X2=%d, Y2=%d)\n", X1, Y1, X2, Y2);

	if(X1 == X2)
	{
		// Vertical

		unsigned y;

		if(X1 >= 0 && X1 < (long) m_Width)
		{
			for(y = CExtraMath::Max(CExtraMath::Min(Y1, Y2), 0); y <= (unsigned) CExtraMath::Min((long) CExtraMath::Max(Y1, Y2), (long) (m_Height - 1)); y++)
				SetPixel(X1, y, black);
		}
	}
	else if(Y1 == Y2)
	{
		// Horizontal

		unsigned x;

		if(Y1 >= 0 && Y1 < (int) m_Height)
		{
			for(x = CExtraMath::Max(CExtraMath::Min(X1, X2), 0); x <= (unsigned) CExtraMath::Min((long) CExtraMath::Max(X1, X2), (long) (m_Width - 1)); x++)
				SetPixel(x, Y1, black);
		}
	}
	else
	{
		// Diagonal

		if(abs(X2 - X1) >= abs(Y2 - Y1))
		{
			// Diagonal Line Closer To Horizontal

			long x, y;

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

			for(x = X1; x <= X2; x++)
			{
				y = int(double(x - X1) * Gradient) + Y1;

				if(y >= 0 && y < (long) m_Height && x >= 0 && x < (long) m_Width)
					SetPixel(x, y, black);
			}
		}
		else
		{
			// Diagonal Line Closer To Vertical

			long x, y;

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

			for(y = Y1; y <= Y2; y++)
			{
				x = int(double(y - Y1) * Gradient) + X1;

				if(y >= 0 && y < (long) m_Height && x >= 0 && x < (long) m_Width)
					SetPixel(x, y, black);
			}
		}
	}
	m_NumberOfBlackPixels = -1L;
	m_NumberOfWhitePixels = -1L;
}

void CBiLevelImage::SetWhite(const int x, const int y)
{
	m_pLineArray[y][x] = WHITE;
	m_NumberOfBlackPixels = -1L;
	m_NumberOfWhitePixels = -1L;
}

void CBiLevelImage::Smear(int SmearValue)
{
	int Column;
	int LastBlackIndex;
	int ZeroCount;
	int k, y;

	for(Column = 0; Column < m_Width; Column++)
	{
		LastBlackIndex = -1; // to cater for leading zeros
		ZeroCount = 0;
	
		for (y = 0; y < m_Height; y++)
		{
			if (IsWhite(Column,y))
			{
				ZeroCount++;
			}
			else
			{
				if (ZeroCount < SmearValue)
				{
					for (k = LastBlackIndex + 1; k < y; k++)
					{
						SetBlack(Column,k);
					}
				}
				SetBlack(Column,y);
	         	LastBlackIndex = y;
         		ZeroCount = 0;
			}
		}
		// check for trailing zeros which are fewer than vval and set them to 1
		if(IsWhite(Column,m_Height-1) && ZeroCount < SmearValue)
		{
			for(k = LastBlackIndex + 1; k < m_Height; k++)
			{
				SetBlack(Column,k);
			}
		}
	}
	m_NumberOfBlackPixels = -1L;
	m_NumberOfWhitePixels = -1L;
}

void CBiLevelImage::UnPackBitArray(uint8_t * BitArray)
{
	long y;

	//We consider the current values of Width, Height, BitsPerPixel and BytesPerLine, indicative of the bitArray
	if (m_pArray != NULL) 
		delete [] m_pArray;

	int adjustedWidth = m_Width + (8-(m_Width % 8)); //CC 17.11.2009 workaround for 'mod 8' problem
	m_pArray = new uint8_t[adjustedWidth * m_Height];

	if (m_pLineArray != NULL) 
		delete [] m_pLineArray;
	m_pLineArray = new uint8_t*[m_Height];

	m_pLineArray[m_Height - 1] = m_pArray;
	for(y = m_Height - 2; y >= 0; y--)
		m_pLineArray[y] = m_pLineArray[y + 1] + adjustedWidth;

	uint8_t * CurByte = m_pArray;
	uint8_t * CurBitByte = BitArray;

	for(y = 0; y < (long) m_Height; y++)
	{
		CurByte = m_pLineArray[y];
		//CurBitByte = BitArray + y * m_BytesPerLine;
		for (int b = 0; b < m_BytesPerLine; b++) //refering to the Bit Array
		{
			for(int i = 0; i < 8; i++)
			{
				if(b * 8 + i < adjustedWidth)
				{
					uint8_t Mask = (1 << (7-i));
					(*CurByte) = ((*CurBitByte) & Mask) != 0x00?WHITE:BLACK;
					CurByte++;
				}
			}
			CurBitByte++;
		}
	}
}

CImage * CBiLevelImage::CreateNewImage()
{
	return new CBiLevelImage();
}

/*
 * Creates an image of the same type copying the specified frame.
 */
/*CImage* CBiLevelImage::CreateSubImage(int left, int top, int width, int height)
{
	if (left < 0 || left >= m_Width || top < 0 || top >= m_Height)
		return NULL;

	CBiLevelImage* img = new CBiLevelImage();

	img->Create(width, height, this->GetBackColour(), this->GetName());
	
	img->SetBitsPerPixel(this->GetBitsPerPixel());
	img->SetIccLength(this->GetIccLength());
	img->SetIccProfile(this->GetIccProfile());
	img->SetResolution(this->GetXRes(), img->GetYRes());

	//Memcopy line by line
	for (int y=0; y<height; y++)
		memcpy(img->GetLineArray()[y], m_pLineArray[top+y] + sizeof(unsigned char)*left, sizeof(unsigned char) * width);

	return img;
}*/

void CBiLevelImage::SetRGBColor(int x, int y, RGBCOLOUR colour)
{
	if (colour.R == RGBWHITE.R && colour.G == RGBWHITE.G && colour.B == RGBWHITE.B)
		SetWhite(x,y);
	else
		SetBlack(x,y);
}

/*
 * Counts the black or white pixels within the specified rectangular area.
 * TODO: use runs not the single pixels
 */
long CBiLevelImage::CountPixels(int left, int top, int right, int bottom, bool black /*= true*/)
{
	int x,y;
	long count = 0L;
	if (black) //avoids the condition within the loop
	{
		for (y=top; y<=bottom; y++)
			for (x=left; x<=right; x++)
				if (this->IsBlack(x, y))
					count++;
	}
	else
	{
		for (y=top; y<=bottom; y++)
			for (x=left; x<=right; x++)
				if (this->IsWhite(x, y))
					count++;
	}
	return count;
}

/*
 * Counts the black or white pixels within the specified rectangle.
 */
long CBiLevelImage::CountPixels(CRect * rect, bool black /*= true*/)
{
	return CountPixels(rect->left, rect->top, rect->right, rect->bottom, black);
}

/*
 * Counts the black or white pixels within the specified rectangles.
 */
long CBiLevelImage::CountPixels(vector<CRect *> * rects, bool black /*= true*/)
{
	long ret = 0L;
	for (unsigned int i=0; i<rects->size(); i++)
		ret += CountPixels(rects->at(i), black);
	return ret;
}

/*
 * Counts the pixels for the whole image.
 */
long CBiLevelImage::CountPixels(bool black)
{
	long count = 0L;
	if (black)
	{
		if (m_NumberOfBlackPixels >= 0L)	//already counted
			return m_NumberOfBlackPixels;
		m_NumberOfBlackPixels = 0L;
		for (int x=0; x<m_Width; x++)
			for (int y=0; y<m_Height; y++)
				if (IsBlack(x, y))
					m_NumberOfBlackPixels++;
		m_NumberOfWhitePixels = (m_Width*m_Height) - m_NumberOfBlackPixels; //Total area - #black pixels
		return m_NumberOfBlackPixels;
	}
	else //white
	{
		if (m_NumberOfWhitePixels >= 0L)	//already counted
			return m_NumberOfWhitePixels;
		m_NumberOfWhitePixels = 0L;
		for (int x=0; x<m_Width; x++)
			for (int y=0; y<m_Height; y++)
				if (IsBlack(x, y))
					m_NumberOfWhitePixels++;
		m_NumberOfBlackPixels = (m_Width*m_Height) - m_NumberOfWhitePixels; //Total area - #black pixels
		return m_NumberOfWhitePixels;
	}
}

/*
 * Creates a deep copy of this image.
 */
CImage * CBiLevelImage::Clone(bool reverse /*= false*/)
{
	return Clone(reverse, false);
}

/*
 * Creates a deep copy of this image.
 */
CImage * CBiLevelImage::Clone(bool reverse, bool flipVertically)
{
	CBiLevelImage * copy = new CBiLevelImage();

	copy->Create(m_Width, m_Height, m_BackColour, m_sName, reverse);
	copy->SetBitsPerPixel(m_BitsPerPixel);

	int x,y;
	if (flipVertically)
	{
		for(y = 0; y < m_Height; y++)
		{
			for(x = 0; x < m_Width; x++)
				copy->SetRGBColor(x,y,GetRGBColor(x,y));
		}
	}
	else
	{
		for(y = 0; y < m_Height; y++)
		{
			for(x = 0; x < m_Width; x++)
				copy->SetRGBColor(x,y,GetRGBColor(x,m_Height-1-y));
		}
	}

	return copy;
}

//16/08/11 - AJF - Draws a component to an image

void CBiLevelImage::DrawComponent(CConnectedComponent* comp)
{
	CRun *run=NULL;
	int y=0;
	int x1=0;
	int x2=0;
	
	for(int r=0; r<comp->GetRunCount(); r++)
	{
		run = comp->GetRun(r);

		y=run->GetY();
		x1=run->GetX1();
		x2=run->GetX2();

		SetLine(x1, y, x2, y);
	}
}