#include "OpenCvImageRenderer.h"


namespace PRImA {

/*
 * Class COpenCvImageRenderer
 *
 * GDI rendering of images based on OpenCV library.
 *
 * CC 01/11/2013 - created
 */

/*
 * Constructor
 */
COpenCvImageRenderer::COpenCvImageRenderer(void)
{
}

/*
 * Destructor
 */
COpenCvImageRenderer::~COpenCvImageRenderer(void)
{
}

/*
 * Draws the given image on the specified device context.
 * This method does not scale the image.
 */
void COpenCvImageRenderer::Render(COpenCvImage * img, CDC * dc/*, Graphics graphics*/)
{
	if (img == NULL)
		return;

	int height = img->GetHeight();
	int width = img->GetWidth();
	uchar buffer[sizeof( BITMAPINFOHEADER ) + 1024]; 
	BITMAPINFO* bmi = (BITMAPINFO* )buffer; 
	int bpp = Bpp(img->GetData());
	FillBitmapInfo(bmi,width,height,bpp,0);

	uint8_t * pixelData = img->GetGdiCompatiblePixelData();
	
	SetDIBitsToDevice(dc->GetSafeHdc(), 0, 0, width,
		height, 0, 0, 0, height, pixelData, bmi,
		DIB_RGB_COLORS);
}

/*
 * Draws the given image on the specified device context.
 */
void COpenCvImageRenderer::Render(COpenCvImage * img, CDC * dc, 
				int xOriginDest, int yOriginDest, int widthDest, int heightDest,
				int xOriginSource, int yOriginSource, int widthSource, int heightSource,
				int zoomFactor /*=100*/,
				DWORD operation /*=SRCCOPY*/,
				DWORD stretchMode /*= COLORONCOLOR*/)
{
	if (img == NULL)
		return;

	int height = img->GetHeight();
	int width = img->GetWidth();
	uchar buffer[sizeof( BITMAPINFOHEADER ) + 1024]; 
	BITMAPINFO* bmi = (BITMAPINFO* )buffer; 
	int bpp = Bpp(img->GetData());
	FillBitmapInfo(bmi,width,height,bpp,0);

	uint8_t * pixelData = img->GetGdiCompatiblePixelData();

	dc->SetStretchBltMode(stretchMode);

	if (yOriginSource > 0) //No gap at top
	{
		if (yOriginSource + heightSource < height) //No gap at bottom
		{
			yOriginSource = height - yOriginSource - heightSource;
		}
		else /*if (zoomFactor == 100)*/ //Gap at bottom 
		{
			if (yOriginSource + heightSource <= height)
				yOriginSource = 0;
			else
				yOriginSource = -1;
			//yOriginDest -= (int)((double)zoomFactor/100.0);
		}
	}
	else //Gap at top
	{
		if (xOriginSource > 0) //No gap left
		{
			yOriginSource = height - yOriginSource - heightSource;
		}
		else if (zoomFactor != 100) //Gap left and zoomed
		{
			yOriginSource = height - yOriginSource - heightSource;
		}
	}

	StretchDIBits(dc->GetSafeHdc(), xOriginDest, yOriginDest, widthDest, heightDest,
					xOriginSource, yOriginSource, widthSource, heightSource,
					 pixelData, bmi, DIB_RGB_COLORS, operation);
}

/*
 * Returns bits per pixel.
 */
int COpenCvImageRenderer::Bpp(cv::Mat img) 
{ 
	return 8 * img.channels(); 
} 

void COpenCvImageRenderer::FillBitmapInfo(BITMAPINFO* bmi, int width, int height, int bpp, int origin) 
{ 
	assert(bmi && width >= 0 && height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32)); 

	BITMAPINFOHEADER* bmih = &(bmi->bmiHeader); 

	memset(bmih, 0, sizeof(*bmih)); 
	bmih->biSize = sizeof(BITMAPINFOHEADER); 
	bmih->biWidth = width; 
	bmih->biHeight = origin ? abs(height) : -abs(height); 
	bmih->biPlanes = 1; 
	bmih->biBitCount = (unsigned short)bpp; 
	bmih->biCompression = BI_RGB; 


	if (bpp == 8) 
	{ 
		RGBQUAD* palette = bmi->bmiColors; 

		for (int i = 0; i < 256; i++) 
		{ 
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i; 
			palette[i].rgbReserved = 0; 
		} 
	} 
}


} //end namespace