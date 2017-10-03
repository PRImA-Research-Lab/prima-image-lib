#include "StdAfx.h"
#include "ImageTransformer.h"
#include "math.h"
#include "typeinfo.h"

using namespace cv;

namespace PRImA
{

/*
 * Class CImageTransformer
 *
 * Provides methods to transform images.
 *
 * CC 02.12.2010 - created
 */

/*
 * Resizes the given image.
 * 'highQuality' - If set to true, area interpolation will be used, otherwise linear interpolation
 * Returns a new image with the specified size by resampling the original image.
 */
COpenCvImage * CImageTransformer::Resize(COpenCvImage * image, int newWidth, int newHeight, bool highQuality /*= false*/)
{
	if (newWidth < 1 || newHeight < 1 || image == NULL)
		return NULL;

	Mat resizedData(newHeight, newWidth, image->GetData().type());

	int method = highQuality ? INTER_AREA : INTER_LINEAR;

	resize(image->GetData(), resizedData, Size(newWidth, newHeight), 0.0, 0.0, method);

	int type = COpenCvImage::TYPE_COLOUR;
	if (typeid(*image) == typeid(COpenCvBiLevelImage))
		type = COpenCvImage::TYPE_BILEVEL;
	else if (typeid(*image) == typeid(COpenCvGreyScaleImage))
		type = COpenCvImage::TYPE_GREYSCALE;

	return COpenCvImage::Create(resizedData, type, false);
}

/*
 * Rotates the given image by 90 degrees
 * 'clockwise' - If 'true', rotates clockwise, otherwise counter clockwise
 */
void CImageTransformer::Rotate(COpenCvImage * source, bool clockwise)
{
	//1=CW, 2=CCW, 3=180
	if (clockwise)
	{
		Mat temp = source->GetData().t();
		//transpose(source->GetData(), source->GetData());  
		flip(temp, temp, 1); //transpose+flip(1)=CW
		source->SetData(temp);
	} 
	else //Counter clockwise
	{
		Mat temp = source->GetData().t();
		//transpose(source->GetData(), source->GetData());  
		flip(temp, temp, 0); //transpose+flip(0)=CCW     
		source->SetData(temp);
	} 
}

/*
 * Erosion (thins the dark objects of an image)
 * Note: Works only for bilevel and greyscale images. For colour the input image is returned!
 */
COpenCvImage * CImageTransformer::Erode(COpenCvImage * image)
{
	if (image == NULL)
		return NULL;
	
	if (typeid(*image) == typeid(COpenCvBiLevelImage))
	{
		return Erode((COpenCvBiLevelImage*)image);
	}
	else if (typeid(*image) == typeid(COpenCvGreyScaleImage))
	{
		return Erode((COpenCvGreyScaleImage*)image);
	}
	return image;
}

/*
 * Dilation (thickens the dark objects of an image)
 * Note: Works only for bilevel and greyscale images. For colour the input image is returned!
 */
COpenCvImage * CImageTransformer::Dilate(COpenCvImage * image)
{
	if (image == NULL)
		return NULL;
	
	if (typeid(*image) == typeid(COpenCvBiLevelImage))
	{
		return Dilate((COpenCvBiLevelImage*)image);
	}
	else if (typeid(*image) == typeid(COpenCvGreyScaleImage))
	{
		return Dilate((COpenCvGreyScaleImage*)image);
	}
	return image;
}

/*
 * Erosion of binary image
 */
COpenCvImage * CImageTransformer::Erode(COpenCvBiLevelImage * image)
{
	COpenCvBiLevelImage * res = (COpenCvBiLevelImage*)image->CreateSubImage(0, 0, image->GetWidth(), image->GetHeight());

	int x,y;

	//Set the border to white
	x = image->GetWidth()-1;
	for (int y=0; y<image->GetHeight(); y++)
	{
		res->SetWhite(0, y);
		if (x >= 0)
			res->SetWhite(x, y);
	}
	y = image->GetHeight()-1;
	for (int x=0; x<image->GetWidth(); x++)
	{
		res->SetWhite(x, 0);
		if (y >= 0)
			res->SetWhite(x, y);
	}

	//Now erode the rest
	for (y=1; y<image->GetHeight()-1; y++)
	{
		for (x=1; x<image->GetWidth()-1; x++)
		{
			if (image->IsBlack(x,y)) 
			{
				if (image->IsWhite(x-1,y)
					||	image->IsWhite(x+1,y)
					||	image->IsWhite(x,y-1)
					||	image->IsWhite(x,y+1))
				{
					res->SetWhite(x,y);
				}
			}
		}
	}
	return res;
}

/*
 * Dilation of binary image
 */
COpenCvImage * CImageTransformer::Dilate(COpenCvBiLevelImage * image)
{
	COpenCvBiLevelImage * res = (COpenCvBiLevelImage*)image->CreateSubImage(0, 0, image->GetWidth(), image->GetHeight());

	int x,y;

	//Horizontally
	for (y=0; y<image->GetHeight(); y++)
	{
		for (x=1; x<image->GetWidth()-1; x++)
		{
			if (image->IsBlack(x,y))
			{
				res->SetBlack(x-1,y);
				res->SetBlack(x+1,y);
			}
		}
	}
	//Vertically
	for (x=0; x<image->GetWidth(); x++)
	{
		for (y=1; y<image->GetHeight()-1; y++)
		{
			if (image->IsBlack(x,y))
			{
				res->SetBlack(x,y-1);
				res->SetBlack(x,y+1);
			}
		}
	}
	return res;
}

/*
 * Erosion of greyscale image
 */
COpenCvImage * CImageTransformer::Erode(COpenCvGreyScaleImage * image)
{
	//TODO Use OpenCV method

	COpenCvGreyScaleImage * res = (COpenCvGreyScaleImage*)image->Clone();

	int x,y,i,max;
	int right = image->GetWidth()-1;
	int bottom = image->GetHeight()-1;
	int * arr = new int[5];

	//Corners
	// Top Left
	arr[0] = image->GetGreyLevel(0,0);
	arr[1] = image->GetGreyLevel(0,1);
	arr[2] = image->GetGreyLevel(1,0);
	max = 0;
	for (i=0; i<3; i++)
	{
		if (arr[i] > max)
			max = arr[i];
	}
	res->SetGreyLevel(0,0,max);
	// Top right
	arr[0] = image->GetGreyLevel(right,0);
	arr[1] = image->GetGreyLevel(right,1);
	arr[2] = image->GetGreyLevel(right-1,0);
	max = 0;
	for (i=0; i<3; i++)
	{
		if (arr[i] > max)
			max = arr[i];
	}
	res->SetGreyLevel(right,0,max);
	// Bottom Left
	arr[0] = image->GetGreyLevel(0,bottom);
	arr[1] = image->GetGreyLevel(0,bottom-1);
	arr[2] = image->GetGreyLevel(1,bottom);
	max = 0;
	for (i=0; i<3; i++)
	{
		if (arr[i] > max)
			max = arr[i];
	}
	res->SetGreyLevel(0,bottom,max);
	// Bottom Right
	arr[0] = image->GetGreyLevel(right,bottom);
	arr[1] = image->GetGreyLevel(right,bottom-1);
	arr[2] = image->GetGreyLevel(right-1,bottom);
	max = 0;
	for (i=0; i<3; i++)
	{
		if (arr[i] > max)
			max = arr[i];
	}
	res->SetGreyLevel(right,bottom,max);

		//Borders
	// Left, Right
	for (y=1; y<bottom; y++)
	{
		//Left
		arr[0] = image->GetGreyLevel(0,y);
		arr[1] = image->GetGreyLevel(0,y-1);
		arr[2] = image->GetGreyLevel(0,y+1);
		arr[3] = image->GetGreyLevel(1,y);
		max = 0;
		for (i=0; i<4; i++)
		{
			if (arr[i] > max)
				max = arr[i];
		}
		res->SetGreyLevel(0,y,max);
		//Right
		arr[0] = image->GetGreyLevel(right,y);
		arr[1] = image->GetGreyLevel(right,y-1);
		arr[2] = image->GetGreyLevel(right,y+1);
		arr[3] = image->GetGreyLevel(right-1,y);
		max = 0;
		for (i=0; i<4; i++)
		{
			if (arr[i] > max)
				max = arr[i];
		}
		res->SetGreyLevel(right,y,max);
	}
	// Top, Bottom
	for (x=1; x<right; x++)
	{
		//Top
		arr[0] = image->GetGreyLevel(x,0);
		arr[1] = image->GetGreyLevel(x-1,0);
		arr[2] = image->GetGreyLevel(x+1,0);
		arr[3] = image->GetGreyLevel(x,1);
		max = 0;
		for (i=0; i<4; i++)
		{
			if (arr[i] > max)
				max = arr[i];
		}
		res->SetGreyLevel(x,0,max);
		//Right
		arr[0] = image->GetGreyLevel(x,bottom);
		arr[1] = image->GetGreyLevel(x-1,bottom);
		arr[2] = image->GetGreyLevel(x+1,bottom);
		arr[3] = image->GetGreyLevel(x,bottom-1);
		max = 0;
		for (i=0; i<4; i++)
		{
			if (arr[i] > max)
				max = arr[i];
		}
		res->SetGreyLevel(x,bottom,max);
	}

	//Now erode the rest (center)
	for (y=1; y<image->GetHeight()-1; y++)
	{
		for (x=1; x<image->GetWidth()-1; x++)
		{
			//Find the maximum grey value
			max = 0;
			arr[0] = image->GetGreyLevel(x,y);
			arr[1] = image->GetGreyLevel(x-1,y);
			arr[2] = image->GetGreyLevel(x+1,y);
			arr[3] = image->GetGreyLevel(x,y-1);
			arr[4] = image->GetGreyLevel(x,y+1);
			for (i=0; i<5; i++)
			{
				if (arr[i] > max)
					max = arr[i];
			}
			res->SetGreyLevel(x,y,max);
		}
	}
	delete [] arr;
	return res;
}

/*
 * Dilation of greyscale image
 */
COpenCvImage * CImageTransformer::Dilate(COpenCvGreyScaleImage * image)
{
	//TODO Use OpenCV method

	COpenCvGreyScaleImage * res = (COpenCvGreyScaleImage*)image->Clone();

	int x,y, min, i;
	int * arr = new int[5];
	int right = image->GetWidth()-1;
	int bottom = image->GetHeight()-1;

	//Corners
	// Top Left
	arr[0] = image->GetGreyLevel(0,0);
	arr[1] = image->GetGreyLevel(0,1);
	arr[2] = image->GetGreyLevel(1,0);
	min = 256;
	for (i=0; i<3; i++)
	{
		if (arr[i] < min)
			min = arr[i];
	}
	res->SetGreyLevel(0,0,min);
	// Top right
	arr[0] = image->GetGreyLevel(right,0);
	arr[1] = image->GetGreyLevel(right,1);
	arr[2] = image->GetGreyLevel(right-1,0);
	min = 256;
	for (i=0; i<3; i++)
	{
		if (arr[i] < min)
			min = arr[i];
	}
	res->SetGreyLevel(right,0,min);
	// Bottom Left
	arr[0] = image->GetGreyLevel(0,bottom);
	arr[1] = image->GetGreyLevel(0,bottom-1);
	arr[2] = image->GetGreyLevel(1,bottom);
	min = 256;
	for (i=0; i<3; i++)
	{
		if (arr[i] < min)
			min = arr[i];
	}
	res->SetGreyLevel(0,bottom,min);
	// Bottom Right
	arr[0] = image->GetGreyLevel(right,bottom);
	arr[1] = image->GetGreyLevel(right,bottom-1);
	arr[2] = image->GetGreyLevel(right-1,bottom);
	min = 256;
	for (i=0; i<3; i++)
	{
		if (arr[i] < min)
			min = arr[i];
	}
	res->SetGreyLevel(right,bottom,min);

	//Borders
	// Left, Right
	for (y=1; y<bottom; y++)
	{
		//Left
		arr[0] = image->GetGreyLevel(0,y);
		arr[1] = image->GetGreyLevel(0,y-1);
		arr[2] = image->GetGreyLevel(0,y+1);
		arr[3] = image->GetGreyLevel(1,y);
		min = 256;
		for (i=0; i<4; i++)
		{
			if (arr[i] < min)
				min = arr[i];
		}
		res->SetGreyLevel(0,y,min);
		//Right
		arr[0] = image->GetGreyLevel(right,y);
		arr[1] = image->GetGreyLevel(right,y-1);
		arr[2] = image->GetGreyLevel(right,y+1);
		arr[3] = image->GetGreyLevel(right-1,y);
		min = 256;
		for (i=0; i<4; i++)
		{
			if (arr[i] < min)
				min = arr[i];
		}
		res->SetGreyLevel(right,y,min);
	}
	// Top, Bottom
	for (x=1; x<right; x++)
	{
		//Top
		arr[0] = image->GetGreyLevel(x,0);
		arr[1] = image->GetGreyLevel(x-1,0);
		arr[2] = image->GetGreyLevel(x+1,0);
		arr[3] = image->GetGreyLevel(x,1);
		min = 256;
		for (i=0; i<4; i++)
		{
			if (arr[i] < min)
				min = arr[i];
		}
		res->SetGreyLevel(x,0,min);
		//Right
		arr[0] = image->GetGreyLevel(x,bottom);
		arr[1] = image->GetGreyLevel(x-1,bottom);
		arr[2] = image->GetGreyLevel(x+1,bottom);
		arr[3] = image->GetGreyLevel(x,bottom-1);
		min = 256;
		for (i=0; i<4; i++)
		{
			if (arr[i] < min)
				min = arr[i];
		}
		res->SetGreyLevel(x,bottom,min);
	}

	//Rest (center)
	for (y=1; y<bottom; y++)
	{
		for (x=1; x<right; x++)
		{
			//Find the minimum grey value
			int min = 256;
			arr[0] = image->GetGreyLevel(x,y);
			arr[1] = image->GetGreyLevel(x-1,y);
			arr[2] = image->GetGreyLevel(x+1,y);
			arr[3] = image->GetGreyLevel(x,y-1);
			arr[4] = image->GetGreyLevel(x,y+1);
			for (i=0; i<5; i++)
			{
				if (arr[i] < min)
					min = arr[i];
			}
			res->SetGreyLevel(x,y,min);
		}
	}
	delete [] arr;
	return res;
}

/*
 * Binarises the given colour or grey scale image using the specified threshold.
 */
COpenCvBiLevelImage * CImageTransformer::Binarize(COpenCvImage * source, int thresh)
{
	if (source == NULL || typeid(*source) == typeid(COpenCvBiLevelImage))
		return NULL;


	//Create image
	COpenCvBiLevelImage * destImage = NULL;
	
	if (source->GetData().channels() == 1)
	{
		destImage = COpenCvImage::CreateB(source->GetWidth(), source->GetHeight(), RGBWHITE);
		destImage->CopyImageInfo(source->GetImageInfo());
	}
	else //Source is RGB
	{
		Mat destMat;
		cvtColor(source->GetData(), destMat, CV_RGB2GRAY, 1);
		destImage = (COpenCvBiLevelImage*)COpenCvImage::Create(destMat, COpenCvImage::TYPE_BILEVEL, false);
		destImage->CopyImageInfo(source->GetImageInfo());
		source = destImage;
	}

	int maxVal = COpenCvImage::CalcMaxValueForColorChannel(destImage->GetData());
	thresh = min(maxVal, thresh);
	thresh = max(0, thresh);

	try
	{
		threshold(source->GetData(), destImage->GetData(), thresh, maxVal, 
					THRESH_BINARY);
	}
	catch (Exception & exc)
	{
		CUniString msg (exc.msg);
		delete destImage;
		return NULL;
	}

	return destImage;
}

/*
 * Otsu binarization method
 */
COpenCvBiLevelImage * CImageTransformer::OtsuBinarization(COpenCvImage * source)
{
	//Create image
	COpenCvBiLevelImage * destImage = NULL;
	
	if (source->GetData().channels() == 1)
	{
		destImage = COpenCvImage::CreateB(source->GetWidth(), source->GetHeight(), RGBWHITE);
		destImage->CopyImageInfo(source->GetImageInfo());
	}
	else //Source is RGB
	{
		Mat destMat;
		cvtColor(source->GetData(), destMat, CV_RGB2GRAY, 1);
		destImage = (COpenCvBiLevelImage*)COpenCvImage::Create(destMat, COpenCvImage::TYPE_BILEVEL, false);
		destImage->CopyImageInfo(source->GetImageInfo());
		source = destImage;
	}

	int maxVal = COpenCvImage::CalcMaxValueForColorChannel(destImage->GetData());
	int thresh = 0;

	try
	{
		threshold(source->GetData(), destImage->GetData(), thresh, maxVal, 
					THRESH_BINARY | THRESH_OTSU);
	}
	catch (Exception & exc)
	{
		CUniString msg (exc.msg);
		delete destImage;
		return NULL;
	}
	return destImage;

	//CC 08/11/13 - In OpenCV the Otsu method is only implemented for 8 bit image.
	//              We therefore use the old method to find the threshold and use
	//              OpenCV only to binarise.

	//Create image
	/*COpenCvBiLevelImage * destImage = COpenCvImage::CreateB(source->GetWidth(), source->GetHeight(), RGBWHITE);
	if (source->GetImageInfo() != NULL)
	{
		CImageInfo * info = destImage->GetImageInfo();
		if (info == NULL)
		{
			info = new CImageInfo();
			destImage->SetImageInfo(info);
		}
		info->resolutionX = source->GetImageInfo()->resolutionX;
		info->resolutionY = source->GetImageInfo()->resolutionY;
	}

	int maxVal = COpenCvImage::CalcMaxValueForColorChannel(destImage->GetData());
	int thresh = 0;

	threshold(source->GetData(), destImage->GetData(), thresh, maxVal, 
				THRESH_BINARY | THRESH_OTSU);

	return destImage;
	*/

	/*if (source == NULL || typeid(*source) == typeid(COpenCvBiLevelImage))
		return NULL;

	//See http://www.sas.bg/code-snippets/image-binarization-the-otsu-method.html

	int max_x = source->GetWidth();
	int max_y = source->GetHeight();
	const int L = 256;
	float hist[L]={0.0F};
	bool isGreyScale = 	typeid(*source) == typeid(COpenCvGreyScaleImage);

	//calculate grayscale histogram
	for (int x=0; x < max_x; x++)
	{
		for(int y=0; y < max_y; y++)
		{
			RGBCOLOUR cur;
			cur = source->GetRGBColor(x, y);
			int graylevel = isGreyScale ? cur.R : (int)max(0.0, min(255.0, 0.299*cur.R + 0.587*cur.G + 0.114*cur.B));
			hist[graylevel]++;
		}
	}

	int N = max_x*max_y;

	//normalize histogram
	for (int i=0; i<L; i++)
		hist[i] /= N;


	float ut = 0;
	for (int i=0; i<L; i++)
		ut += i*hist[i];

	int max_k = 0;
	int max_sigma_k_ = 0;
	for (int k=0; k < L; k++)
	{
		float wk = 0;
		for (int i = 0; i <=k; i++)
			wk += hist[i];
		float uk = 0;
		for (int i = 0; i <=k; i++)
			uk += i*hist[i];

		float sigma_k = 0;
		if (wk !=0 && wk!=1)
			sigma_k = ((ut*wk - uk)*(ut*wk - uk))/(wk*(1-wk));

		if (sigma_k > max_sigma_k_)
		{
			max_k = k;
			max_sigma_k_ = (int)sigma_k;
		}
	}

	return Binarize(source, max_k);*/
}

// Copyright 2006-2008 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
// or its licensors, as applicable.
//
// You may not use this file except under the terms of the accompanying license.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you
// may not use this file except in compliance with the License. You may
// obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Project: OCRopus
// File: ocr-binarize-sauvola.cc
// Purpose: An efficient implementation of the Sauvola's document binarization
//          algorithm based on integral images as described in
//          F. Shafait, D. Keysers, T.M. Breuel. "Efficient Implementation of
//          Local Adaptive Thresholding Techniques Using Integral Images".
//          Document Recognition and Retrieval XV, San Jose.
//
// Responsible: Faisal Shafait (faisal.shafait@dfki.de)
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de
// 
// >> Chacnges by PRImA <<
// 
// CC 25.03.2011 - Adapted to the PRImA image classes and removed dependencies to Ocropus libraries.
//
// Source: http://code.google.com/p/ocropus/source/browse/ocr-binarize/?r=5f019d223ad0264c02d5101e00726e7893e18352

#define MAXVAL 256

//BinarizeBySauvola() {
//    pdef("k",0.3,"Weighting factor");
//    pdef("w",40,"Local window size. Should always be positive");
//}

COpenCvBiLevelImage * CImageTransformer::SauvolaBinarization(COpenCvImage * source, double k /*= 0.3*/, int w /*= 40*/,
													   CAlgorithm * stopSignalSource /*= NULL*/)
{
	if (source == NULL || typeid(*source) == typeid(COpenCvBiLevelImage))
		return NULL;

	bool isGreyScale = 	typeid(*source) == typeid(COpenCvGreyScaleImage);

	//Check params
    if(k<0.05)
		k = 0.05;
	if (k > 0.95)
		k = 0.95;
    if (w<0)
		w = 0;
	if (w>1000)
		w = 1000;

	int whalf = w / 2;

	//Create image
	COpenCvBiLevelImage * destImage = COpenCvImage::CreateB(source->GetWidth(), source->GetHeight(), RGBWHITE);
	destImage->CopyImageInfo(source->GetImageInfo());

	const int image_width  = source->GetWidth();
	const int image_height = source->GetHeight();

    // Calculate the integral image, and integral of the squared image
	int64_t ** integral_image = new int64_t * [image_height];
	int64_t ** rowsum_image = new int64_t * [image_height];
	int64_t ** integral_sqimg = new int64_t * [image_height];
	int64_t ** rowsum_sqimg = new int64_t * [image_height];
	int i = 0;
	try {
		for (i=0; i<image_height; i++)
		{
			integral_image[i] = new int64_t[image_width];
			rowsum_image[i] = new int64_t[image_width];
			integral_sqimg[i] = new int64_t[image_width];
			rowsum_sqimg[i] = new int64_t[image_width];
			if (	integral_image[i] == NULL
				||	rowsum_image[i] == NULL
				||	integral_sqimg[i] == NULL
				||	rowsum_sqimg[i] == NULL)
			{
				CleanUpForSauvola(integral_image, rowsum_image, integral_sqimg, rowsum_sqimg, i-1);
				delete destImage;
				return NULL;
			}
		}
	}
	catch (CMemoryException * )
	{
		CleanUpForSauvola(integral_image, rowsum_image, integral_sqimg, rowsum_sqimg, i-1);
		delete destImage;
		return NULL;
	}

	int xmin,ymin,xmax,ymax;
    double diagsum,idiagsum,diff,sqdiagsum,sqidiagsum,sqdiff,area;
    double mean,std,threshold;

    for(int y=0; y<image_height; y++){
		RGBCOLOUR col = source->GetRGBColor(0,y);
		int graylevel = isGreyScale ? col.R : (int)(col.R*0.3 + col.G*0.59+ col.B*0.11);
        rowsum_image[y][0] = graylevel;
        rowsum_sqimg[y][0] = graylevel*graylevel;
    }
    for(int x=1; x<image_width; x++){
        for(int y=0; y<image_height; y++){
			RGBCOLOUR col = source->GetRGBColor(x,y);
			int graylevel = isGreyScale ? col.R : (int)(col.R*0.3 + col.G*0.59+ col.B*0.11);
            rowsum_image[y][x] = rowsum_image[y][x-1] + graylevel;
            rowsum_sqimg[y][x] = rowsum_sqimg[y][x-1] + graylevel*graylevel;
        }
    }

	//Cancelled?
	if (stopSignalSource != NULL && stopSignalSource->HasStopSignal())
	{
		delete destImage;
		CleanUpForSauvola(integral_image, rowsum_image, integral_sqimg, rowsum_sqimg, image_height);
		return NULL;
	}

    for(int x=0; x<image_width; x++){
        integral_image[0][x] = rowsum_image[0][x];
        integral_sqimg[0][x] = rowsum_sqimg[0][x];
    }
    for(int x=0; x<image_width; x++){
        for(int y=1; y<image_height; y++){
            integral_image[y][x] = integral_image[y-1][x] + rowsum_image[y][x];
            integral_sqimg[y][x] = integral_sqimg[y-1][x] + rowsum_sqimg[y][x];
        }
    }

	//Cancelled?
	if (stopSignalSource != NULL && stopSignalSource->HasStopSignal())
	{
		delete destImage;
		CleanUpForSauvola(integral_image, rowsum_image, integral_sqimg, rowsum_sqimg, image_height);
		return NULL;
	}

    //Calculate the mean and standard deviation using the integral image
    for(int x=0; x<image_width; x++)
	{
        for(int y=0; y<image_height; y++){
            xmin = max(0,x-whalf);
            ymin = max(0,y-whalf);
            xmax = min(image_width-1,x+whalf);
            ymax = min(image_height-1,y+whalf);
            area = (xmax-xmin+1)*(ymax-ymin+1);
            // area can't be 0 here
            // proof (assuming whalf >= 0):
            // we'll prove that (xmax-xmin+1) > 0,
            // (ymax-ymin+1) is analogous
            // It's the same as to prove: xmax >= xmin
            // image_width - 1 >= 0         since image_width > i >= 0
            // i + whalf >= 0               since i >= 0, whalf >= 0
            // i + whalf >= i - whalf       since whalf >= 0
            // image_width - 1 >= i - whalf since image_width > i
            // --IM
            ASSERT(area);
            if(!xmin && !ymin){ // Point at origin
                diff   = (double)integral_image[ymax][xmax];
                sqdiff = (double)integral_sqimg[ymax][xmax];
            }
            else if(!xmin && ymin){ // first column
                diff   = (double)(integral_image[ymax][xmax] - integral_image[ymin-1][xmax]);
                sqdiff = (double)(integral_sqimg[ymax][xmax] - integral_sqimg[ymin-1][xmax]);
            }
            else if(xmin && !ymin){ // first row
                diff   = (double)(integral_image[ymax][xmax] - integral_image[ymax][xmin-1]);
                sqdiff = (double)(integral_sqimg[ymax][xmax] - integral_sqimg[ymax][xmin-1]);
            }
            else{ // rest of the image
                diagsum    = (double)(integral_image[ymax][xmax] + integral_image[ymin-1][xmin-1]);
                idiagsum   = (double)(integral_image[ymin-1][xmax] + integral_image[ymax][xmin-1]);
                diff       = diagsum - idiagsum;
                sqdiagsum  = (double)(integral_sqimg[ymax][xmax] + integral_sqimg[ymin-1][xmin-1]);
                sqidiagsum = (double)(integral_sqimg[ymin-1][xmax] + integral_sqimg[ymax][xmin-1]);
                sqdiff     = sqdiagsum - sqidiagsum;
            }

            mean = diff/area;
            std  = sqrt((sqdiff - diff*diff/area)/(area-1));

			//Thresholding
            threshold = mean*(1+k*((std/128)-1));
			RGBCOLOUR col = source->GetRGBColor(x,y);
			int graylevel = isGreyScale ? col.R : (int)(col.R*0.3 + col.G*0.59+ col.B*0.11);
            if(graylevel < threshold)
                destImage->SetBlack(x, y);
            else
                destImage->SetWhite(x, y);
        }

		//Cancelled?
		if (stopSignalSource != NULL && stopSignalSource->HasStopSignal())
		{
			delete destImage;
			CleanUpForSauvola(integral_image, rowsum_image, integral_sqimg, rowsum_sqimg, image_height);
			return NULL;
		}
    }

	CleanUpForSauvola(integral_image, rowsum_image, integral_sqimg, rowsum_sqimg, image_height);

	return destImage;
}

/*
 * Deletes arrays used by the Sauvola binarisation
 */
void CImageTransformer::CleanUpForSauvola(int64_t ** array1, int64_t ** array2, 
										  int64_t ** array3, int64_t ** array4, int count)
{
	for (int i = 0; i < count; i++)
  	{
  		delete [] array1[i];
  		delete [] array2[i];
  		delete [] array3[i];
  		delete [] array4[i];
  	}
	delete [] array1;
	delete [] array2;
	delete [] array3;
	delete [] array4;
}

/*
 * Converts the given image to grey scale format (creates new image). If the source image 
 * already is grey scale, it will be returned unchanged.
 */
COpenCvGreyScaleImage * CImageTransformer::ConvertToGreyScale(COpenCvImage * source)
{
	if (source == NULL)
		return NULL;
	if (typeid(*source) == typeid(COpenCvGreyScaleImage))
		return (COpenCvGreyScaleImage*)source;

	//Create empty image
	COpenCvGreyScaleImage * res = COpenCvImage::CreateG(source->GetWidth(), source->GetHeight(), RGBWHITE);

	//Convert
	// Bitonal
	if (typeid(*source) == typeid(COpenCvBiLevelImage))
	{
		COpenCvBiLevelImage * bilevelImage = (COpenCvBiLevelImage*)source;
		for (int x=0; x < source->GetWidth(); x++)
			for(int y=0; y < source->GetHeight(); y++)
				res->SetGreyLevel(x, y, bilevelImage->IsWhite(x, y) ? 255 : 0);
	}
	else //Colour
	{
		RGBCOLOUR cur;
		for (int x=0; x < source->GetWidth(); x++)
		{
			for(int y=0; y < source->GetHeight(); y++)
			{
				cur = source->GetRGBColor(x, y);
				res->SetGreyLevel(x, y, (int)max(0.0, min(255.0, 0.299*cur.R + 0.587*cur.G + 0.114*cur.B)));
			}
		}
	}

	//Some params
	res->CopyImageInfo(source->GetImageInfo());

	return res;
}

/*
 * Converts the given image to colour format (creates new image). If the source image 
 * already is in colour format, it will be returned unchanged.
 */
COpenCvColourImage * CImageTransformer::ConvertToColour(COpenCvImage * source)
{
	if (source == NULL)
		return NULL;
	if (typeid(*source) == typeid(COpenCvColourImage))
		return (COpenCvColourImage*)source;


	//Create empty image
	COpenCvColourImage * res = COpenCvImage::CreateC(source->GetWidth(), source->GetHeight(), RGBWHITE);

	//Convert
	cv::Mat colourMat;

	//Check number of channels in source (CopenCvBiLevel image can have 3 channels in some cases)
	if (source->GetData().type() == CV_8UC1)
	{
		colourMat.create(source->GetHeight(), source->GetWidth(), CV_8UC3);

		//if (typeid(*thumbnail) == typeid(COpenCvColourImage))
		//	cvtColor(thumbnail->GetData(), thumbMat, CV_BGR2BGRA); //RGB colour to RGB with alpha
		//else
		cvtColor(source->GetData(), colourMat, CV_GRAY2BGR); //BW or greyscale to RGB with alpha

		res->SetData(colourMat);
		//thumbMat.copyTo(colPixData(cv::Rect(x, y, thumbWidth, thumbHeight)));
	}
	else //Already colour
	{
		res->SetData(source->GetData());
	}
	//Some params
	res->CopyImageInfo(source->GetImageInfo());

	return res;
}



} //end namespace