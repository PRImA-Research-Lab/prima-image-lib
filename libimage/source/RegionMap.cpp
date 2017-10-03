#include <stdlib.h>

#include "RegionMap.h"
#include <math.h>

using namespace cv;

namespace PRImA
{

/*
 * Class CLayoutObjectMap
 *
 * A bitonal bitmap with some operations such as smearing.
 *
 * CC 24/02/2016 - Renamed from CRegionMap
 */

/*
 * Constructor
 */
CLayoutObjectMap::CLayoutObjectMap()
{
	m_pMap = NULL;
}

/*
 * Destructor
 */
CLayoutObjectMap::~CLayoutObjectMap()
{
	Delete();
}

/*
 * Initialises the internal data structures and fields for a bitmap of the given dimensions
 */
bool CLayoutObjectMap::Create(const int Width, const int Height, const int OffsetX, const int OffsetY)
{
	int i;

	Delete();

	m_nHeight = Height;
	m_nWidth  = Width;

	if((m_pMap = new uint8_t * [m_nHeight]) == NULL)
		return false;
	
	for(i = 0; i < m_nHeight; i++)
	{
		if((m_pMap[i] = new uint8_t[Width + 7 / 8]) == NULL)
			return false;
	}

	m_nOffsetX = OffsetX;
	m_nOffsetY = OffsetY;

	return true;
}

/*
 * Creates a new image as a copy of this map
 */
COpenCvBiLevelImage * CLayoutObjectMap::CreateImage()
{
	int x, y;
	COpenCvBiLevelImage * Im = COpenCvImage::CreateB(m_nWidth, m_nHeight, RGBWHITE);

	for(y = 0; y < m_nHeight; y++)
	{
		for(x = 0; x < m_nWidth; x++)
		{
			if(GetValueDirect(x, y))
				Im->SetBlack(x, y);
		}
	}

	return Im;
}

/*
 * Deletes all content and resets fields
 */
void CLayoutObjectMap::Delete()
{
	int i;

	if(m_pMap != NULL)
	{
		for(i = 0; i < m_nHeight; i++)
			delete [] m_pMap[i];
	}

	delete [] m_pMap;
	m_pMap = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
}

/*
 * Returns the height of this bitmap
 */
int CLayoutObjectMap::GetHeight()
{
	return m_nHeight;
}

/*
 * Returns the value (true or false) at the given position within the bitmap (not using offset)
 */
bool CLayoutObjectMap::GetValueDirect(const int x, const int y)
{
    uint8_t * b = &(m_pMap[y][x / 8]);
    uint8_t Mask;
    switch(x - ((x / 8) * 8))
    {
    case 0:
            Mask = 0x80;
            break;
    case 1:
            Mask = 0x40;
            break;
    case 2:
            Mask = 0x20;
            break;
    case 3:
            Mask = 0x10;
            break;
    case 4:
            Mask = 0x08;
            break;
    case 5:
            Mask = 0x04;
            break;
    case 6:
            Mask = 0x02;
            break;
    case 7:
            Mask = 0x01;
            break;
	default:
            Mask = 0x00;
            break;
    }

    return ((*b & Mask) != 0x00);
}

/*
 * Returns the value (true or false) at the given position within the bitmap using the specified offset
 */
bool CLayoutObjectMap::GetValueOffset(const int x, const int y)
{
	return GetValueDirect(x - m_nOffsetX, y - m_nOffsetY);
}

/*
 * Returns the width of this bitmap
 */
int CLayoutObjectMap::GetWidth()
{
	return m_nWidth;
}

/*
 * Sets all pixels to the given value
 */
void CLayoutObjectMap::SetAll(const bool Value)
{
	int x, y;

	for(y = 0; y < m_nHeight; y++)
	{
		for(x = 0; x < m_nWidth; x++)
			SetValueDirect(x, y, Value);
	}
}

/*
 * Sets the pixel value at the given position (not using offset)
 */
void CLayoutObjectMap::SetValueDirect(const int x, const int y, const bool Value)
{
    uint8_t * b = &(m_pMap[y][x / 8]);
    int bit = x - ((x / 8) * 8);
    uint8_t Mask;

    if(Value)
    {
            switch(bit)
            {
            case 0:
                    Mask = 0x80;
                    break;
            case 1:
                    Mask = 0x40;
                    break;
            case 2:
                    Mask = 0x20;
                    break;
            case 3:
                    Mask = 0x10;
                    break;
            case 4:
                    Mask = 0x08;
                    break;
            case 5:
                    Mask = 0x04;
                    break;
            case 6:
                    Mask = 0x02;
                    break;
            case 7:
                    Mask = 0x01;
                    break;
	default:
		// Shouldn't reach this point
		Mask = 0x00;
		break;
            }
            *b = *b | Mask;
    }
    else
    {
            switch(bit)
            {
            case 0:
                    Mask = 0x7f;
                    break;
            case 1:
                    Mask = 0xbf;
                    break;
            case 2:
                    Mask = 0xdf;
                    break;
            case 3:
                    Mask = 0xef;
                    break;
            case 4:
                    Mask = 0xf7;
                    break;
            case 5:
                    Mask = 0xfb;
                    break;
            case 6:
                    Mask = 0xfd;
                    break;
            case 7:
                    Mask = 0xfe;
                    break;
	default:
		// Shouldn't reach this point
		Mask = 0xff;
		break;
            }
            *b = *b & Mask;
	}
}

/*
 * Sets the pixel value at the given position using the internal offset
 */
void CLayoutObjectMap::SetValueOffset(const int x, const int y, const bool Value)
{
	SetValueDirect(x - m_nOffsetX, y - m_nOffsetY, Value);
}

/*
 * Sets the value at the given position corrected by the offset.
 * Checks if the coordinates are within the boundaries of the map.
 * CC 10.09.2010
 */
void CLayoutObjectMap::SetValueOffsetSave(const int x, const int y, const bool Value)
{
	int xt = x - m_nOffsetX;
	int yt = y - m_nOffsetY;
	if (xt >= 0 && yt >= 0 && xt < m_nWidth && yt < m_nHeight)
		SetValueDirect(xt, yt, Value);
}

/*
 * Fills all gaps that a equal or smaller than 'SmearVal'
 */
void CLayoutObjectMap::Smear(const int SmearVal, const SmearDir Dir, bool smearCol /*= true*/)
{
	int x, y, z, lastx, lasty;

	if(Dir == SD_VERT)
	{
		// Vertical

		for(x = 0; x < GetWidth(); x++)
		{
			lasty = -1;
	
			for(y = 0; y < GetHeight(); y++)
			{
				if(GetValueDirect(x, y) == smearCol)
				{
					if(lasty != -1 && y - lasty - 1 <= SmearVal)
					{
						for(z = lasty + 1; z < y; z++)
							SetValueDirect(x, z, smearCol);
					}
	
					lasty = y;
				}
			}
		}
	}
	else if(Dir == SD_HORZ)
	{
		// Horizontal
		for(y = 0; y < GetHeight(); y++)
		{
			lastx = -1;
	
			for(x = 0; x < GetWidth(); x++)
			{
				if(GetValueDirect(x, y) == smearCol)
				{
					if(lastx != -1 && x - lastx - 1 <= SmearVal)
					{
						for(z = lastx + 1; z < x; z++)
							SetValueDirect(z, y, smearCol);
					}
	
					lastx = x;
				}
			}
		}
	}
	else if(Dir == SD_UPRIGHT)
	{
	}
	else if(Dir == SD_DOWNRIGHT)
	{
	}
}

/*
 * Smears diagonally from connected component 'from' in direction connected component 'to'.
 *
 * CC 04.08.2010 - created
 */
void CLayoutObjectMap::Smear(const int smearVal, CConnectedComponent * from, CConnectedComponent * to)
{
	//Direction
	int x1 = from->GetCenterX();
	int y1 = from->GetCenterY();
	int x2 = to->GetCenterX();
	int y2 = to->GetCenterY();

	double dx = x2 - x1;
	double dy = y2 - y1;
	double hyp = sqrt(dx*dx + dy*dy);

	//Normize (so that the hypothenuse is 1)
	dx /= hyp;
	dy /= hyp;

	//Iterate over all pixel runs of the connected component 'from'
	CRun * run;
	int x, y, xOld, yOld;
	double xd, yd;
	vector<int> smearStartX;
	vector<int> smearStartY;
	vector<int> smearLength;
	for (int i=0; i<from->GetRunCount(); i++)
	{
		run = from->GetRun(i);
		//For each pixel of the run
		int yStart = run->GetY();
		for (int xStart = run->GetX1(); xStart <= run->GetX2(); xStart++)
		{
			//Smear
			// Check if there is a gap smaller than smearVal
			xd = xStart;
			yd = yStart;
			xd += 2*dx;		//Start a bit later to avoid the start component itself
			yd += 2*dy;
			for (int n=2; n<smearVal; n++)
			{
				xd += dx;
				yd += dy;
				x = (int)(xd + 0.5);  //round to pixel raster
				y = (int)(yd + 0.5);

				if (x < 0 || y < 0 || x >= m_nWidth || y >= m_nHeight)
					break;

				//if (x != xStart || y != yStart)
				//{
					if (GetValueDirect(x, y)) //black
					{
						//Store the start point and smear length for later
						if (n >= 1)
						{
							smearStartX.push_back(xStart);
							smearStartY.push_back(yStart);
							smearLength.push_back(n);
						}
						break;
					}
				//}
			}
		}
	}
	for (unsigned int i=0; i<smearStartX.size(); i++)
	{
		// Now smear all gaps that were small enough
		xd = smearStartX[i];
		yd = smearStartY[i];
		xOld = smearStartX[i];
		yOld = smearStartY[i];
		for (int n=0; n<=smearLength[i]; n++)
		{
			xd += dx;
			yd += dy;
			x = (int)(xd + 0.5);  //round to pixel raster
			y = (int)(yd + 0.5);
			SetValueDirect(x, y, true);

			//If x and y changed, we put an extra point to have a four-connected neighbourhood
			if (x != xOld && y != yOld)
				SetValueDirect(xOld, y, true);

			xOld = x;
			yOld = y;
		}
	}
}

/*
 * Dilation (morphological operation)
 */
void CLayoutObjectMap::Dilate()
{
	//TODO: Make more efficient

	COpenCvBiLevelImage * img = CreateImage();
	if (img == NULL)
		return;

	const int erosion_size = 1;
	//int erosion_type;
	//if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
	//else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
	//else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement( MORPH_RECT,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       cv::Point( erosion_size, erosion_size ) );

	cv::erode(img->GetData(), img->GetData(), element);

	//Copy back to map
	int x,y;
	for(y = 0; y < GetHeight(); y++)
	{
		for(x = 0; x < GetWidth(); x++)
		{
			if (img->IsBlack(x, y))
				SetValueDirect(x,y, true);
		}
	}

	delete img;
}

/*
 * Find all connected components (of 'true') within this bitmap
 */
CConnectedComponents * CLayoutObjectMap::ExtractComponents()
{
	CConnectedComponents * ccs = new CConnectedComponents();

	//(True == black)

	int Width = m_nWidth;
	int Height = m_nHeight;
	
	int x, y;
	bool LookForColour = true;

	ccs->SetSize(Width, Height);
	//TODO: Initialize arrays?

	//Find components
	try
	{
		bool InColour;
		int xStart = 0;
		// Look for runs line by line
		for(y = 0; y < m_nHeight; y++)
		{
			InColour = false;
			
			for(x = 0; x < Width; x++)
			{
				if(GetValueDirect(x,y) == LookForColour && !InColour)
				{
					// Found START OF RUN
					if(x == Width - 1) // RUN of WIDTH 1 at END of LINE
					{
						if(!ccs->AddRun(y,x,x,false))
						{
							return false;
						}
					}
					else
					{
						InColour = true;
						xStart = x;
					}
				}
				else if(x == Width - 1 && InColour && GetValueDirect(x,y) == LookForColour)
				{
					//End of line == END OF RUN
					InColour = false;
					if(!ccs->AddRun(y, xStart, x, false))
					{
						delete ccs;
						return NULL;
					}
				}
				else if(InColour && GetValueDirect(x,y) != LookForColour)
				{
					//Found END OF RUN
					InColour = false;
					if(!ccs->AddRun(y, xStart, x - 1, false))
					{
						delete ccs;
						return NULL;
					}
				}
			}
		}
	}
	catch (CMemoryException * )
	{
		delete ccs;
		return NULL;
	}
	return ccs;
}


} //end namespace