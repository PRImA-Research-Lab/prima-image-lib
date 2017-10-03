#include "OpenCvImage.h"

using namespace cv;

namespace PRImA {

/*
 * Class COpenCvImage
 *
 * Base class for images using OpenCV library.
 *
 * CC 01/11/2013 - created
 */

/*
 * Constructor
 */
COpenCvImage::COpenCvImage(void)
{
	m_GdiCompatibleData = NULL;
	m_OrigImageDataIsGdiCompatible = false;
	m_MaxValueForColorChannel = 255; //Default
	m_ImageInfo = NULL;
}

/*
 * Destructor
 */
COpenCvImage::~COpenCvImage(void)
{
	//The OpenCV class "Mat" uses reference counting and destroys itself

	ResetGdiCompatiblePixelData();
	delete m_ImageInfo;
}

/*
 * Static Image Factory
 *
 * Creates instances of OpenCV images (of type COpenCvColourImage, COpenCvGreyScaleImage or COpenCvBiLevelImage).
 *
 * 'data' - Image data (pixels) in OpenCV format.
 */
COpenCvImage * COpenCvImage::Create(Mat data)
{
	return COpenCvImage::Create(data, TYPE_AUTO);
}

/*
 * Static Image Factory 
 *
 * Creates instances of OpenCV images (of type COpenCvColourImage, COpenCvGreyScaleImage or COpenCvBiLevelImage).
 *
 * 'data' - Image data (pixels) in OpenCV format.
 * 'enforceType' - Image type (colour depth) to use. One of:
 *                   COpenCvImage::TYPE_COLOUR     - RGB colour image
 *                   COpenCvImage::TYPE_GREYSCALE  - Grey scale image
 *                   COpenCvImage::TYPE_BILEVEL    - Black-and-white image
 *                   COpenCvImage::TYPE_AUTO       - Auto-detect
 * 'ensurePixelValuesAreInRange' - If set to true, it is guaranteed that all pixel values are within the requested range (bilevel, greyscale or colour).
 *                                 If bi-level is requested, for example, the image will be binarised when this option is set to true.
 */
COpenCvImage * COpenCvImage::Create(Mat data, int enforceType, bool ensurePixelValuesAreInRange /* = true */)
{
	COpenCvImage * img = NULL;
	int maxValueForColorChannel = COpenCvImage::CalcMaxValueForColorChannel(data); //8 or 16 bit

	if (enforceType == TYPE_AUTO) //Auto detect image type
	{
		//Determine if colour, grey scale or bi-level (heuristic not using all pixels)
		if (data.depth() == CV_8U) //8 bit
			img = COpenCvImageOps<uchar>::DetectColorDepthAndCreateImage(data);
		else //16 bit
			img = COpenCvImageOps<ushort>::DetectColorDepthAndCreateImage(data);
	}
	else if (enforceType == TYPE_COLOUR) //Colour
	{
		//No checks required
		img = new COpenCvColourImage();
	}
	else if (enforceType == TYPE_GREYSCALE) //Grey scale
	{
		//No checks required either (because loaded in grey scale)
		img = new COpenCvGreyScaleImage();
	}
	else if (enforceType == TYPE_BILEVEL) //B/W
	{
		if (ensurePixelValuesAreInRange)
		{
			//Binarise
			if (data.depth() == CV_8U) //8 bit
				COpenCvImageOps<uchar>::MiddleThreshold(data);
			else //16 bit
				COpenCvImageOps<ushort>::MiddleThreshold(data);
		}

		//Check (heuristic)
		/*
		int x, y, width = data.cols, height = data.rows;
		int startX = 0;
		int stepX = max(2, width / 1000);
		for (y=0; y<height; y+=2)
		{
			for (x=startX; x<width; x+=stepX)
			{
				pixel = data.at<uchar>(y,x);
				if (pixel != 0 && pixel != maxValueForColorChannel) //Grey scale
				{
					img = new COpenCvGreyScaleImage();
					goto out2;
				}
			}
			startX++; //For diagonal pattern
			if (startX > stepX)
				startX = 0;
		}
out2:*/
		img = new COpenCvBiLevelImage();
	}
	else
		return NULL;

	img->SetData(data);

	//Fix for greyscale image created as 3-channel image
	if (data.type() == CV_8UC3 && typeid(*img) == typeid(COpenCvGreyScaleImage))
	{
		//Create empty image
		COpenCvGreyScaleImage * temp = COpenCvImage::CreateG(img->GetWidth(), img->GetHeight(), RGBWHITE);

		//Convert
		RGBCOLOUR cur;
		for (int x=0; x < img->GetWidth(); x++)
		{
			for(int y=0; y < img->GetHeight(); y++)
			{
				cur = img->GetRGBColor(x, y);
				temp->SetGreyLevel(x, y, (int)max(0.0, min(255.0, 0.299*cur.R + 0.587*cur.G + 0.114*cur.B)));
			}
		}
		delete img;
		img = temp;
	}

	img->SetMaxValueForColorChannel(maxValueForColorChannel);

	return img;
}

/*
 * Creates an emty image
 *
 * 'type' - One of TYPE_BILEVEL, TYPE_GREYSCALE or TYPE_COLOUR
 * 'backColour' - Initial fill colour
 */
COpenCvImage * COpenCvImage::Create(int type, int width, int height, RGBCOLOUR backColour)
{
	COpenCvImage * ret = NULL;
	if (type == TYPE_BILEVEL)
	{
		ret = new COpenCvBiLevelImage();
		Mat data(height, width, CV_8UC1, CV_RGB(backColour.R, backColour.G, backColour.B));
		ret->SetData(data);
	}
	else if (type == TYPE_GREYSCALE)
	{
		ret = new COpenCvGreyScaleImage();
		Mat data(height, width, CV_8UC1, CV_RGB(backColour.R, backColour.G, backColour.B));
		ret->SetData(data);
	}
	else //Colour
	{
		ret = new COpenCvColourImage();
		Mat data(height, width, CV_8UC3, CV_RGB(backColour.R, backColour.G, backColour.B));
		ret->SetData(data);
	}
	return ret;
}

/*
 * Convenience method to create a new bi-level image. 
 * See Create(int type, int width, int height, RGBCOLOUR backColour)
 */
COpenCvBiLevelImage	* COpenCvImage::CreateB(int width, int height, RGBCOLOUR backColour)
{
	return dynamic_cast<COpenCvBiLevelImage*>(Create(COpenCvImage::TYPE_BILEVEL, width, height, backColour));
}

/*
 * Convenience method to create a new grey scale image. 
 * See Create(int type, int width, int height, RGBCOLOUR backColour)
 */
COpenCvGreyScaleImage *	COpenCvImage::CreateG(int width, int height, RGBCOLOUR backColour)
{
	return dynamic_cast<COpenCvGreyScaleImage*>(Create(COpenCvImage::TYPE_GREYSCALE, width, height, backColour));
}

/*
 * Convenience method to create a new colour image. 
 * See Create(int type, int width, int height, RGBCOLOUR backColour)
 */
COpenCvColourImage * COpenCvImage::CreateC(int width, int height, RGBCOLOUR backColour)
{
	return dynamic_cast<COpenCvColourImage*>(Create(COpenCvImage::TYPE_COLOUR, width, height, backColour));
}

/*
 * Creates a new image from a rectangular subsection of this image
 * 'type' - TYPE_BILEVEL, TYPE_GREYSCALE, TYPE_COLOUR or TYPE_AUTO
 */
COpenCvImage * COpenCvImage::CreateSubImage(int left, int top, int width, int height, int type)
{
	//Sanity check (evaluated in debug mode only)
	ASSERT(left >=0 && top >=0 && left+width-1 < GetWidth() && top+height-1 < GetHeight());

	//Get sub matrix
	Mat subImageData = m_Data(Rect(left, top, width, height));

	//We need to make a copy, otherwise they will share the same pixel array
	Mat copy;
	copy.create(height, width, subImageData.type());
	subImageData.copyTo(copy);

	return COpenCvImage::Create(copy, type, false);
}

/*
 * Returns the maximum value one colour channel of the image according to the depth.
 * At the moment this can be either 255 (8bit) or 65535 (16bit).
 */
int COpenCvImage::CalcMaxValueForColorChannel(Mat data)
{
	return data.depth() == CV_8U ? 255 : 65535;
}

/*
 * Copies image data and properties from the given image to this instance.
 * (Deep copy)
 */
void COpenCvImage::CopyFrom(COpenCvImage * other)
{
	m_Data = other->m_Data.clone();
	//m_GdiCompatibleData = other->m_GdiCompatibleData; //Shouldn't copy this (only a viewing copy anyway)
	m_OrigImageDataIsGdiCompatible = other->m_OrigImageDataIsGdiCompatible;
	m_MaxValueForColorChannel = other->m_MaxValueForColorChannel; 
	CopyImageInfo(other->GetImageInfo());
}

/*
 * Sets the internal OpenCV image data (pixel matrix).
 */
void COpenCvImage::SetData(Mat imageData)
{
	m_Data = imageData;
}

/*
 * Returns the internal OpenCV image data (pixel matrix).
 */
Mat COpenCvImage::GetData()
{
	return m_Data;
}

/* 
 * Returns the width (cols) of this image
 */
int COpenCvImage::GetWidth()
{
	return m_Data.cols;
}

/* 
 * Returns the width (rows) of this image
 */
int COpenCvImage::GetHeight()
{
	return m_Data.rows;
}

/*
 * Returns the colour value of the pixel at the given position.
 */
RGBCOLOUR COpenCvImage::GetRGBColor(int x, int y)
{
	//Sanity check (evaluated in debug mode only)
	ASSERT(x >=0 && y >=0 && x < GetWidth() && y < GetHeight());

	//Grey / B/W
	if (m_Data.channels() == 1)
	{
		if (m_Data.depth() == CV_8U) //8 bit
		{
			uchar pixel = m_Data.at<uchar>(y,x);
			RGBCOLOUR rgb = {pixel, pixel, pixel};
			return rgb;
		}
		else //16 bit
		{
			ushort pixel = m_Data.at<ushort>(y,x);
			RGBCOLOUR rgb = {pixel/256, pixel/256, pixel/256}; //Convert to 8 bit
			return rgb;
		}
	}
	//Colour
	if (m_Data.depth() == CV_8U) //8 bit
	{
		Vec3b pixel = m_Data.at<Vec3b>(y,x);
		RGBCOLOUR rgb = {pixel[0], pixel[1], pixel[2]};
		return rgb;
	}
	else //16 bit
	{
		Vec3w pixel = m_Data.at<Vec3w>(y,x);
		RGBCOLOUR rgb = {pixel[0]/256, pixel[1]/256, pixel[2]/256}; //Convert to 8 bit
		return rgb;
	}
}

/*
 * Sets the colour value of the pixel at the given position.
 */
void COpenCvImage::SetRGBColor(int x, int y, RGBCOLOUR col)
{
	//Sanity check (evaluated in debug mode only)
	ASSERT(x >=0 && y >=0 && x < GetWidth() && y < GetHeight());

	//Grey / B/W
	if (m_Data.channels() == 1)
	{
		if (m_Data.depth() == CV_8U) //8 bit
			m_Data.at<uchar>(y,x) = col.R;
		else //16 bit
			m_Data.at<ushort>(y,x) = col.R * 256; //Convert to 16 bit
	}
	else //Colour
	{
		if (m_Data.depth() == CV_8U) //8 bit
		{
			Vec3b pixel(col.R, col.G, col.B);
			m_Data.at<Vec3b>(y,x) = pixel;
		}
		else //16 bit
		{
			Vec3w pixel(col.R * 256, col.G * 256, col.B * 256); //Convert to 16 bit
			m_Data.at<Vec3w>(y,x) = pixel;
		}
	}
}

/*
 * Returns (and creates if necessary) a pixel array that is compatible for GDI rendering.
 */
uint8_t * COpenCvImage::GetGdiCompatiblePixelData()
{
	if (m_GdiCompatibleData == NULL)
	{
		int height = GetHeight();
		int width = GetWidth();
		int bitsPerChannel = m_Data.depth() == CV_8U ? 8 : 16;
		int bpp = bitsPerChannel * m_Data.channels();
		int scanlineBytes = width * bpp / 8;
		int scanlineBytesCorrected = scanlineBytes;
		if (scanlineBytesCorrected % 4 != 0)
		{
			m_OrigImageDataIsGdiCompatible = false;
			scanlineBytesCorrected += (4 - (scanlineBytes % 4)); //Scanline has to be multiple of 4 byte
			m_GdiCompatibleData = new uint8_t[scanlineBytesCorrected * height];

			int i;
			int startPos = 0;
			int startPosCorrected = 0;
			for (i=0; i<height; i++)
			{
				memcpy(m_GdiCompatibleData+startPosCorrected, m_Data.data+startPos, scanlineBytes);
				startPos += scanlineBytes;
				startPosCorrected += scanlineBytesCorrected;
			}
		}
		else //No need to convert (save time and memory)
		{
			m_OrigImageDataIsGdiCompatible = true;
			m_GdiCompatibleData = m_Data.data;
		}
	}
	return m_GdiCompatibleData;
}

/*
 * Resets the pixel array that is compatible for GDI rendering.
 * Call this method if the pixel data has changed and the image needs to be re-rendered.
 */
void COpenCvImage::ResetGdiCompatiblePixelData()
{
	if (!m_OrigImageDataIsGdiCompatible)
		delete [] m_GdiCompatibleData;
	m_GdiCompatibleData = NULL;
	m_OrigImageDataIsGdiCompatible = false;
}

/*
 * Replaces the current image information with the given one (deletes the old info object)
 */
void COpenCvImage::SetImageInfo(CImageInfo * info)
{
	delete m_ImageInfo;
	m_ImageInfo = info;
}

/*
 * Copies the given image metadata to this image
 */
void COpenCvImage::CopyImageInfo(CImageInfo * other)
{
	if (other == NULL)
		return;
	if (m_ImageInfo == NULL)
		m_ImageInfo = new CImageInfo();

	m_ImageInfo->resolutionX = other->resolutionX;
	m_ImageInfo->resolutionY = other->resolutionY;
	if (m_ImageInfo->bitsPerPixel == 0)
		m_ImageInfo->bitsPerPixel = other->bitsPerPixel;
}

/*
 * Resizes the pixel matrix (no scaling).
 */
void COpenCvImage::Resize(int newWidth, int newHeight)
{
	//Sanity check (evaluated in debug mode only)
	ASSERT(newWidth > 0 && newHeight > 0);

	m_Data = m_Data(Rect(0, 0, newWidth, newHeight)).clone();
	ResetGdiCompatiblePixelData();
}

/*
 * Resizes the pixel matrix (no scaling).
 *
 * 'originX', 'originY' - Origin of the resized matrix within the current matrix (offset).
 */
void COpenCvImage::Resize(int originX, int originY, int newWidth, int newHeight)
{
	//Sanity check (evaluated in debug mode only)
	ASSERT(originX > 0 && originY > 0 && newWidth > 0 && newHeight > 0 && originX < GetWidth() && originY < GetHeight());

	m_Data = m_Data(Rect(originX, originY, newWidth, newHeight)).clone();
	ResetGdiCompatiblePixelData();
}


/*
 * Class COpenCvGreyScaleImage
 *
 * Class for grey scale images using OpenCV library.
 */

/*
 * Constructor
 */
COpenCvGreyScaleImage::COpenCvGreyScaleImage(void) : COpenCvImage()
{
}

/*
 * Destructor
 */
COpenCvGreyScaleImage::~COpenCvGreyScaleImage(void)
{
}

/*
 * Creates a deep copy.
 */
COpenCvImage * COpenCvGreyScaleImage::Clone()
{
	COpenCvImage * copy = new COpenCvGreyScaleImage();
	copy->CopyFrom(this);
	return copy;
}

/*
 * Creates a new image from a rectangular subsection of this image
 */
COpenCvImage * COpenCvGreyScaleImage::CreateSubImage(int left, int top, int width, int height)
{
	return COpenCvImage::CreateSubImage(left, top, width, height, COpenCvImage::TYPE_GREYSCALE);
}

/*
 * Returns the pixel grey value at the given position
 */
int COpenCvGreyScaleImage::GetGreyLevel(int x, int y)
{
	//Sanity check (evaluated in debug mode only)
	ASSERT(x >=0 && y >=0 && x < GetWidth() && y < GetHeight());

	//Grey 
	if (m_Data.channels() == 1)
	{
		if (m_Data.depth() == CV_8U) //8 bit
			return m_Data.at<uchar>(y,x);
		else //16 bit
			return m_Data.at<ushort>(y,x);
	}
	else //Colour (Can happen if we don't know what colour depth to expect when opening the image)
	{
		if (m_Data.depth() == CV_8U) //8 bit
			return m_Data.at<Vec3b>(y,x)[0];
		else //16 bit
			return m_Data.at<Vec3w>(y,x)[0];
	}
}

/*
 * Returns the pixel grey value at the given position. Returns the borderLevel if outside the image dimensions
 */
int COpenCvGreyScaleImage::GetGreyLevel(int x, int y, int borderLevel)
{
	if (x < 0 || y < 0 || x >= GetWidth() || y >= GetHeight())
		return borderLevel;

	//Grey 
	if (m_Data.channels() == 1)
	{
		if (m_Data.depth() == CV_8U) //8 bit
			return m_Data.at<uchar>(y, x);
		else //16 bit
			return m_Data.at<ushort>(y, x);
	}
	else //Colour (Can happen if we don't know what colour depth to expect when opening the image)
	{
		if (m_Data.depth() == CV_8U) //8 bit
			return m_Data.at<Vec3b>(y, x)[0];
		else //16 bit
			return m_Data.at<Vec3w>(y, x)[0];
	}
}

/*
 * Sets the pixel grey value at the given position
 */
void COpenCvGreyScaleImage::SetGreyLevel(int x, int y, int level)
{
	//Sanity check (evaluated in debug mode only)
	ASSERT(x >=0 && y >=0 && x < GetWidth() && y < GetHeight());

	//Grey 
	if (m_Data.channels() == 1)
	{
		if (m_Data.depth() == CV_8U) //8 bit
			m_Data.at<uchar>(y,x) = level;
		else //16 bit
			m_Data.at<ushort>(y,x) = level;
	}
	else //Colour (Can happen if we don't know what colour depth to expect when opening the image)
	{
		if (m_Data.depth() == CV_8U) //8 bit
			m_Data.at<Vec3b>(y,x)[0] = level;
		else //16 bit
			m_Data.at<Vec3w>(y,x)[0] = level;
	}
}


/*
 * Class COpenCvBiLevelImage
 *
 * Class for black-and-white images using OpenCV library.
 */

/*
 * Constructor
 */
COpenCvBiLevelImage::COpenCvBiLevelImage(void) : COpenCvImage()
{
	m_NumberOfBlackPixels = -1L;
	m_NumberOfWhitePixels = -1L;
}

/*
 * Destructor
 */
COpenCvBiLevelImage::~COpenCvBiLevelImage(void)
{
}

/*
 * Creates a deep copy.
 */
COpenCvImage * COpenCvBiLevelImage::Clone()
{
	COpenCvImage * copy = new COpenCvBiLevelImage();
	copy->CopyFrom(this);
	return copy;
}

/*
 * Creates a new image from a rectangular subsection of this image
 */
COpenCvImage * COpenCvBiLevelImage::CreateSubImage(int left, int top, int width, int height)
{
	return COpenCvImage::CreateSubImage(left, top, width, height, COpenCvImage::TYPE_BILEVEL);
}

/*
 * Checks if the pixel at the specified position is black.
 */
bool COpenCvBiLevelImage::IsBlack(int x, int y)
{
	//Sanity check (evaluated in debug mode only)
	ASSERT(x >=0 && y >=0 && x < GetWidth() && y < GetHeight());

	//Grey / B/W
	if (m_Data.channels() == 1)
	{
		if (m_Data.depth() == CV_8U) //8 bit
			return m_Data.at<uchar>(y,x) == 0;
		else //16 bit
			return m_Data.at<ushort>(y,x) == 0;
	}
	else //Colour (Can happen if we don't know what colour depth to expect when opening the image)
	{
		if (m_Data.depth() == CV_8U) //8 bit
			return m_Data.at<Vec3b>(y,x)[0] == 0;
		else //16 bit
			return m_Data.at<Vec3w>(y,x)[0] == 0;
	}
}

/*
 * Checks if the pixel at the specified position is black. Returns borderValue if outside the image dimensions
 * Slower than IsBlack(int x, int y)
 */
bool COpenCvBiLevelImage::IsBlack(int x, int y, bool borderValue)
{
	if (x < 0 || y < 0 || x >= GetWidth() || y >= GetHeight())
		return borderValue;

	//Grey / B/W
	if (m_Data.channels() == 1)
	{
		if (m_Data.depth() == CV_8U) //8 bit
			return m_Data.at<uchar>(y, x) == 0;
		else //16 bit
			return m_Data.at<ushort>(y, x) == 0;
	}
	else //Colour (Can happen if we don't know what colour depth to expect when opening the image)
	{
		if (m_Data.depth() == CV_8U) //8 bit
			return m_Data.at<Vec3b>(y, x)[0] == 0;
		else //16 bit
			return m_Data.at<Vec3w>(y, x)[0] == 0;
	}
}

/*
 * Checks if the pixel at the specified position is white.
 */
bool COpenCvBiLevelImage::IsWhite(int x, int y)
{
	//Sanity check (evaluated in debug mode only)
	ASSERT(x >=0 && y >=0 && x < GetWidth() && y < GetHeight());

	//Grey / B/W
	if (m_Data.channels() == 1)
	{
		if (m_Data.depth() == CV_8U) //8 bit
			return m_Data.at<uchar>(y,x) == m_MaxValueForColorChannel;
		else //16 bit
			return m_Data.at<ushort>(y,x) == m_MaxValueForColorChannel;
	}
	else //Colour (Can happen if we don't know what colour depth to expect when opening the image)
	{
		if (m_Data.depth() == CV_8U) //8 bit
			return m_Data.at<Vec3b>(y,x)[0] == m_MaxValueForColorChannel;
		else //16 bit
			return m_Data.at<Vec3w>(y,x)[0] == m_MaxValueForColorChannel;
	}
}

/*
 * Checks if the pixel at the specified position is white. Returns borderValue if outside the image dimensions
 * Slower than IsWhite(int x, int y)
 */
bool COpenCvBiLevelImage::IsWhite(int x, int y, bool borderValue)
{
	if (x < 0 || y < 0 || x >= GetWidth() || y >= GetHeight())
		return borderValue;

	//Grey / B/W
	if (m_Data.channels() == 1)
	{
		if (m_Data.depth() == CV_8U) //8 bit
			return m_Data.at<uchar>(y, x) == m_MaxValueForColorChannel;
		else //16 bit
			return m_Data.at<ushort>(y, x) == m_MaxValueForColorChannel;
	}
	else //Colour (Can happen if we don't know what colour depth to expect when opening the image)
	{
		if (m_Data.depth() == CV_8U) //8 bit
			return m_Data.at<Vec3b>(y, x)[0] == m_MaxValueForColorChannel;
		else //16 bit
			return m_Data.at<Vec3w>(y, x)[0] == m_MaxValueForColorChannel;
	}
}


/* 
 * Sets the pixel at the specified position to black or white.
 */
void COpenCvBiLevelImage::SetPixel(int x, int y, bool black)
{
	if (black)
		SetBlack(x, y);
	else
		SetWhite(x, y);
}

/* 
 * Sets the pixel at the specified position to black.
 */
void COpenCvBiLevelImage::SetBlack(int x, int y)
{
	//Sanity check (evaluated in debug mode only)
	ASSERT(x >=0 && y >=0 && x < GetWidth() && y < GetHeight());

	//Grey / B/W
	if (m_Data.channels() == 1)
	{
		if (m_Data.depth() == CV_8U) //8 bit
			m_Data.at<uchar>(y,x) = 0;
		else //16 bit
			m_Data.at<ushort>(y,x) = 0;
	}
	else //Colour (Can happen if we don't know what colour depth to expect when opening the image)
		SetRGBColor(x, y, RGBBLACK);
}

/* 
 * Sets the pixel at the specified position to black.
 */
void COpenCvBiLevelImage::SetWhite(int x, int y)
{
	//Sanity check (evaluated in debug mode only)
	ASSERT(x >=0 && y >=0 && x < GetWidth() && y < GetHeight());

	//Grey / B/W
	if (m_Data.channels() == 1)
	{
		if (m_Data.depth() == CV_8U) //8 bit
			m_Data.at<uchar>(y,x) = m_MaxValueForColorChannel;
		else //16 bit
			m_Data.at<ushort>(y,x) = m_MaxValueForColorChannel;
	}
	else //Colour (Can happen if we don't know what colour depth to expect when opening the image)
		SetRGBColor(x, y, RGBWHITE);
}

/*
 * Sets all pixel on the line from (x1,y1) to (x2,y2) to black or white.
 *
 * 'isothetic' - draw an isothetic line (only horizontal and vertical line segments)
 */
void COpenCvBiLevelImage::DrawLine(int x1, int y1, int x2, int y2, bool black, bool isothetic /*= false*/)
{
	//Sanity check (evaluated in debug mode only)
	ASSERT(x1 >=0 && y1 >=0 && x1 < GetWidth() && y1 < GetHeight()
		&& x2 >=0 && y2 >=0 && x2 < GetWidth() && y2 < GetHeight());

	int col = black ? 0 : m_MaxValueForColorChannel;
	int lineType = isothetic ? 8 : 4; //Isothetic = 8-connected line; otherwise 4-connected line
	line(m_Data, Point(x1,y1), Point(x2,y2), CV_RGB(col, col, col), 1, lineType);
}

/*
 * Pixel-wise 'and' operation.
 */
void COpenCvBiLevelImage::And(COpenCvBiLevelImage * b)
{
	if (b == NULL)
		return;
	bitwise_or(m_Data, b->GetData(), m_Data);
	//bitwise_not(m_Data, m_Data);
}

/*
 * Pixel-wise 'and' operation.
 */
void COpenCvBiLevelImage::AndOffset(COpenCvBiLevelImage * b, int offx, int offy)
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

	/*if (b == NULL)
		return;

	Mat pixelsSmall = b->GetData();
	Mat pixelsLarge = m_Data(cv::Rect(offx, offy, b->GetWidth(), b->GetHeight()));

	bitwise_or(pixelsLarge, pixelsSmall, pixelsLarge);*/
}

/*
 * Pixel-wise 'xor' operation.
 */
void COpenCvBiLevelImage::Xor(COpenCvBiLevelImage * b)
{
	if (b == NULL)
		return;
	bitwise_xor(m_Data, b->GetData(), m_Data);
	bitwise_not(m_Data, m_Data);
}

/*
 * Pixel-wise 'xor' operation.
 */
void COpenCvBiLevelImage::XorOffset(COpenCvBiLevelImage * b, int offx, int offy)
{
	if (b == NULL)
		return;

	Mat pixelsSmall = b->GetData();
	Mat pixelsLarge = m_Data(cv::Rect(offx, offy, b->GetWidth(), b->GetHeight()));

	bitwise_xor(pixelsLarge, pixelsSmall, pixelsLarge);
	bitwise_not(pixelsLarge, pixelsLarge);
}

/*
 * Flood fill the area at the specified point with black or white.
 */
void COpenCvBiLevelImage::FloodFill(int x, int y, bool black)
{
	//Sanity check (evaluated in debug mode only)
	ASSERT(x >=0 && y >=0 && x < GetWidth() && y < GetHeight());

	int col = black ? 0 : m_MaxValueForColorChannel;
	floodFill(m_Data, Point(x, y), CV_RGB(col, col, col));
}

/*
 * Counts the black or white pixels within the specified rectangular area.
 * TODO: use runs not the single pixels
 */
long COpenCvBiLevelImage::CountPixels(int left, int top, int right, int bottom, bool black /*= true*/)
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
 * Counts the black or white pixels within the specified rectangular area.
 * Uses borderValue if outside the image dimensions.
 */
long COpenCvBiLevelImage::CountPixels(bool borderValue, int left, int top, int right, int bottom, bool black /*= true*/)
{
	int x, y;
	long count = 0L;
	if (black) //avoids the condition within the loop
	{
		for (y = top; y <= bottom; y++)
			for (x = left; x <= right; x++)
				if (this->IsBlack(x, y, borderValue))
					count++;
	}
	else
	{
		for (y = top; y <= bottom; y++)
			for (x = left; x <= right; x++)
				if (this->IsWhite(x, y, borderValue))
					count++;
	}
	return count;
}

/*
 * Counts the black or white pixels within the specified rectangle.
 */
long COpenCvBiLevelImage::CountPixels(CRect * rect, bool black /*= true*/)
{
	return CountPixels(rect->left, rect->top, rect->right, rect->bottom, black);
}

/*
 * Counts the black or white pixels within the specified rectangles.
 */
long COpenCvBiLevelImage::CountPixels(vector<CRect *> * rects, bool black /*= true*/)
{
	long ret = 0L;
	for (unsigned int i=0; i<rects->size(); i++)
		ret += CountPixels(rects->at(i), black);
	return ret;
}

/*
 * Counts the pixels for the whole image.
 *
 * 'black' - count black pixels (true) or white pixels (false).
 * 'forceRefresh' (optional) - Clears the count buffer and recounts if set to true. Use this option if the image content has changed.
 */
long COpenCvBiLevelImage::CountPixels(bool black, bool forceRefresh /*= false*/)
{
	if (forceRefresh)
	{
		m_NumberOfBlackPixels = -1L;
		m_NumberOfWhitePixels = -1L;
	}

	long count = 0L;
	int height = GetHeight();
	int width = GetWidth();
	if (black)
	{
		if (m_NumberOfBlackPixels >= 0L)	//already counted
			return m_NumberOfBlackPixels;
		m_NumberOfBlackPixels = 0L;
		for (int x=0; x<width; x++)
			for (int y=0; y<height; y++)
				if (IsBlack(x, y))
					m_NumberOfBlackPixels++;
		m_NumberOfWhitePixels = (width*height) - m_NumberOfBlackPixels; //Total area - #black pixels
		return m_NumberOfBlackPixels;
	}
	else //white
	{
		if (m_NumberOfWhitePixels >= 0L)	//already counted
			return m_NumberOfWhitePixels;
		m_NumberOfWhitePixels = 0L;
		for (int x=0; x<width; x++)
			for (int y=0; y<height; y++)
				if (IsBlack(x, y))
					m_NumberOfWhitePixels++;
		m_NumberOfBlackPixels = (width*height) - m_NumberOfWhitePixels; //Total area - #black pixels
		return m_NumberOfWhitePixels;
	}
}


/*
 * Class COpenCvColourImage
 *
 * Class for colour images using OpenCV library.
 */

/*
 * Constructor
 */
COpenCvColourImage::COpenCvColourImage(void) : COpenCvImage()
{
}

/*
 * Destructor
 */
COpenCvColourImage::~COpenCvColourImage(void)
{
}

/*
 * Creates a deep copy.
 */
COpenCvImage * COpenCvColourImage::Clone()
{
	COpenCvImage * copy = new COpenCvColourImage();
	copy->CopyFrom(this);
	return copy;
}

/*
 * Creates a new image from a rectangular subsection of this image
 */
COpenCvImage * COpenCvColourImage::CreateSubImage(int left, int top, int width, int height)
{
	return COpenCvImage::CreateSubImage(left, top, width, height, COpenCvImage::TYPE_COLOUR);
}


/*
 * Class CImageInfo
 *
 * Image metadata.
 */

/*
 * Constructor
 */
CImageInfo::CImageInfo()
{
	resolutionX = 0;
	resolutionY = 0;
	bitsPerPixel = 0;
	//resolutionUnit = 0;
}


/*
 * Template COpenCvImageOps
 *
 * Class template for image operations using OpenCV library.
 *
 * Type T: Type used for values in image channels (one channel for B/W and grey scale, three channels for RGB). 
 *         Usually 'uchar' for 8 bit or 'ushort' for 16 bit.
 */

/*
 * Binarises the given image using the middle of the grey scale value range (e.g. 128 for 8bit depth).
 */
template<class T>
void COpenCvImageOps<T>::MiddleThreshold(Mat data)
{
	int maxValueForColorChannel = COpenCvImage::CalcMaxValueForColorChannel(data); //8 or 16 bit
	int x, y, width = data.cols, height = data.rows;
	T thresh = maxValueForColorChannel / 2;
	for (y=0; y<height; y++)
	{
		for (x=0; x<width; x++)
			data.at<T>(y,x) = (T)(data.at<T>(y,x) < thresh ? 0 : maxValueForColorChannel);
	}
}

/*
 * Auto-detects the colour depth of the given image and returns an instance of the appropriate image sub-class.
 */
template<class T>
COpenCvImage * COpenCvImageOps<T>::DetectColorDepthAndCreateImage(Mat data)
{
	COpenCvImage * img = NULL;
	int maxValueForColorChannel = COpenCvImage::CalcMaxValueForColorChannel(data); //8 or 16 bit

	//Determine if colour, grey scale or bi-level (heuristic not using all pixels)
	//(Fast for colour, slow for grey scale and B/W)
	int x, y, width = data.cols, height = data.rows;
	int startX = 0;
	int stepX = max(2, width / 1000);
	Vec<T,3> pixel;
	bool grey = false;
	for (y=0; y<height; y+=2)
	{
		for (x=startX; x<width; x+=stepX)
		{
			pixel = data.at<Vec<T,3>>(y,x);
			if (	pixel[0] != pixel[1]
				||	pixel[1] != pixel[2]
				||	pixel[0] != pixel[2]) //Colour
				{
					return new COpenCvColourImage();
					//goto out;
				}
			else //All three channels have the same value
			{
				if (pixel[0] != 0 && pixel[0] != maxValueForColorChannel) //Grey scale
				{
					grey = true;
					//img = new COpenCvGreyScaleImage();
					//goto out;
				}
			}
		}
		startX++; //For diagonal pattern
		if (startX > stepX)
			startX = 0;
	}
//out:

	//Must be black-and-white image
	//if (img == NULL)

	if (grey)
		img = new COpenCvGreyScaleImage();
	else //B/W
		img = new COpenCvBiLevelImage();

	return img;
}



} //end namespace