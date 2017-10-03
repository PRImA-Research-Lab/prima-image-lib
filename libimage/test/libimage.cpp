// libimage.cpp : Defines the entry point for the console application.
//
// CC 19.05.2011 - Made this a proper PRImA tool for image operations
//

//#include "stdafx.h"
#include <iostream>
#include <direct.h> // for getcwd
#include <stdlib.h> // for MAX_PATH#include "Image.h"
#include <ctime>
#include "opencvImageReader.h"
#include "opencvImageWriter.h"
#include <stdio.h>
#include "ImageTransformer.h"
#include "ExtraString.h"
#include "ExtraFileHelper.h"
#include "ProjectionProfile.h"
#include "AdaptiveBinariser.h"

using namespace std;
using namespace PRImA;

//Functions
void ShowUsage();
COpenCvImage * ThresholdBinarization(COpenCvImage * inputImage, int argc, char * argv[], bool forceOutput);
COpenCvImage * OtsuBinarization(COpenCvImage * inputImage, bool forceOutput);
COpenCvImage * SauvolaBinarization(COpenCvImage * inputImage, int argc, char * argv[], bool forceOutput);
COpenCvImage * AdaptiveBinarization(COpenCvImage * inputImage, bool forceOutput);
COpenCvImage * Erode(COpenCvImage * inputImage, int argc, char * argv[], bool forceOutput);
COpenCvImage * Dilate(COpenCvImage * inputImage, int argc, char * argv[], bool forceOutput);
COpenCvBiLevelImage * ProjectionProfile(COpenCvImage * inputImage, bool vertical, int argc, char * argv[], bool forceOutput);

//void loadAndSaveImage(char* loadFileName, char* saveFileName);
//void resizeImage(char* loadFileName, char* saveFileName);

int main(int argc, char * argv[])
{
	// Check required number of command-line arguments have been specified
	if(argc < 5)
	{
		cout << "Missing arguments!\n";
		ShowUsage();
		return EXIT_FAILURE;
	}

	//Check input file
	CUniString inputFile(argv[1]);
	if (!CExtraFileHelper::FileExists(inputFile))
	{
		cout << "Input file not found: " << inputFile.ToC_Str() << "\n";
		return EXIT_FAILURE;
	}
	//Load input image
	COpenCvImageReader reader;
	COpenCvImage * inputImage = reader.Read(inputFile);
	if (inputImage == NULL)
	{
		cout << "Could not load the input image!\n";
		return EXIT_FAILURE;
	}
	cout << inputFile.ToC_Str(); //CSV output

	//Output image file
	CUniString outputFile(argv[2]);
	cout << "," << outputFile.ToC_Str(); //CSV output

	//Options
	CUniString options(argv[3]);
	bool overwrite = options.Find(_T("O")) >= 0 || options.Find(_T("o")) >= 0;
	bool forceOutput = options.Find(_T("F")) >= 0 || options.Find(_T("f")) >= 0;

	//Operation switch
	CUniString operation(argv[4]);
	cout << "," << operation.ToC_Str(); //CSV output

	COpenCvImage * outputImage = NULL;

	if (operation == CUniString(_T("ThresholdBin")) || operation == CUniString(_T("thresholdbin")))
		outputImage = ThresholdBinarization(inputImage, argc, argv, forceOutput);
	else if (operation == CUniString(_T("OtsuBin")) || operation == CUniString(_T("otsubin")))
		outputImage = OtsuBinarization(inputImage, forceOutput);
	else if (operation == CUniString(_T("SauvolaBin")) || operation == CUniString(_T("sauvolabin")))
		outputImage = SauvolaBinarization(inputImage, argc, argv, forceOutput);
	else if (operation == CUniString(_T("AdaptiveBin")) || operation == CUniString(_T("adaptivebin")))
		outputImage = AdaptiveBinarization(inputImage, forceOutput);
	else if (operation == CUniString(_T("Erode")) || operation == CUniString(_T("erode")))
		outputImage = Erode(inputImage, argc, argv, forceOutput);
	else if (operation == CUniString(_T("Dilate")) || operation == CUniString(_T("dilate")))
		outputImage = Dilate(inputImage, argc, argv, forceOutput);
	else if (operation == CUniString(_T("HProfile")) || operation == CUniString(_T("hprofile")))
		outputImage = ProjectionProfile(inputImage, false, argc, argv, forceOutput);
	else if (operation == CUniString(_T("VProfile")) || operation == CUniString(_T("vprofile")))
		outputImage = ProjectionProfile(inputImage, true, argc, argv, forceOutput);
	else
	{
		cout << ",ERROR,Unknown operation"; //CSV output
	}

	//Save output image
	if (outputImage != NULL)
	{
		if (CExtraFileHelper::FileExists(outputFile) && !overwrite)
		{
			cout << ",Target file already exists; operation result not saved"; //CSV output
		}
		else
		{
			COpenCvImageWriter writer;
			writer.Write(outputImage, outputFile);
			cout << ",Result image saved"; //CSV output
		}
	}

	cout << "\n"; //CSV output

	if (inputImage != outputImage) //(if forceOutput is used, the input image may be the same as the output image)
		delete inputImage;
	delete outputImage;

	//Old test code:
	/*cout<<"Start\n";

	// _MAX_PATH is the maximum length allowed for a path
	char CurrentPath[_MAX_PATH];
	// use the function to get the path
	getcwd(CurrentPath, _MAX_PATH);

	// display the path for demo purposes only
	char temp[_MAX_PATH];
	cout << CurrentPath << endl;

	loadAndSaveImage("misc\\example\\bilevel.tif", "temp_bilevel.tif");

	loadAndSaveImage("misc\\example\\greyscale.tif", "temp_greyscale.tif");

	loadAndSaveImage("misc\\example\\highcolor.tif", "temp_highcolor.tif");

	loadAndSaveImage("misc\\example\\lowcolor.tif", "temp_lowcolor.tif");

	loadAndSaveImage("misc\\example\\lowcolor_compressed.tif", NULL);

	resizeImage("misc\\example\\greyscale.tif", "temp_greyscale_resized.tif");
	resizeImage("misc\\example\\highcolor.tif", "temp_highcolor_resized.tif");
	resizeImage("misc\\example\\bilevel.tif", "temp_bilevel_resized.tif");
	resizeImage("misc\\example\\colours.tif", "temp_colours_resized.tif");
	resizeImage("misc\\example\\glyph.tif", "temp_glyph_resized.tif");

	cout<<"Press any key to delete the created files and end the program\n";
	cin.getline(temp,_MAX_PATH);

	remove("temp_bilevel.tif");
	remove("temp_greyscale.tif");
	remove("temp_highcolor.tif");
	remove("temp_lowcolor.tif");
	remove("temp_greyscale_resized.tif");
	remove("temp_highcolor_resized.tif");
	remove("temp_bilevel_resized.tif");
	remove("temp_colours_resized.tif");
	remove("temp_glyph_resized.tif");
	*/

	return 0;
}

void ShowUsage()
{
	printf("ImageTool Usage:\n\n");
	printf("  ImageTool <inputimage> <outputimage> <options> <operation> [param1] [param2]\n\n");
	printf("  Where:\n");
	printf("    <inputimage>  File path to source image (tif, png or jpg)\n");
	printf("    <outputimage> File path to output image (tif, png or jpg)\n");
	printf("    <options>     Combination of following characters:  \n");
	printf("                  Note: No spaces between options!\n");
	printf("           'O' - to overwrite existing files\n");
	printf("           'F' - forces saving an output image of a binarization operation\n");
	printf("                 even if input already is a binary image.\n");
	printf("           '-' - as placeholder if no other option is specified\n");
	printf("    <operation>   The image operation to be carried out; one of:\n");
	printf("           ThresholdBin - Binarisation using a threshold\n");
	printf("                              param1: threshold; 0...255\n");
	printf("           OtsuBin      - Binarisation using otsu approach\n");
	printf("           SauvolaBin   - Binarisation using sauvola approach\n");
	printf("                              param1: window size; 10...200 (recommendation: 50)\n");
	printf("                              param2: weight; 0.05...0.95 (recommendation: 0.4)\n");
	printf("           AdaptiveBin  - Adaptive binarisation based on Gatos et al. 2005\n");
	printf("           Erode - Morphological operation (thinning) (for bitonal or greyscale)\n");
	printf("           Dilate - Morphological operation (growing) (for bitonal or greyscale)\n");
	printf("           HProfile - Horizontal projection profile\n");
	printf("           VProfile - Vertical projection profile\n");
	printf("\n");
}

COpenCvImage * ThresholdBinarization(COpenCvImage * inputImage, int argc, char * argv[], bool forceOutput)
{
	if(argc < 6)
	{
		cout << ",ERROR,Missing parameter for operation!"; //CSV output
		return NULL;
	}

	if (typeid(*inputImage) == typeid(COpenCvBiLevelImage))
	{
		cout << ",INFO,Input image already in binary format"; //CSV output
		return forceOutput ? inputImage : NULL;
	}

	int threshold = _tstoi(CUniString(argv[5]).GetBuffer());

	COpenCvBiLevelImage * res = CImageTransformer::Binarize(inputImage, threshold);

	if (res != NULL)
		cout << ",SUCCESS,Operation finished"; //CSV output
	else
		cout << ",ERROR,internal binarisation error"; //CSV output
	return res;
}

COpenCvImage * OtsuBinarization(COpenCvImage * inputImage, bool forceOutput)
{
	if (typeid(*inputImage) == typeid(COpenCvBiLevelImage))
	{
		cout << ",INFO,Input image already in binary format"; //CSV output
		return forceOutput ? inputImage : NULL;
	}

	COpenCvBiLevelImage * res = CImageTransformer::OtsuBinarization(inputImage);

	if (res != NULL)
		cout << ",SUCCESS,Operation finished"; //CSV output
	else
		cout << ",ERROR,internal binarisation error"; //CSV output
	return res;
}

COpenCvImage * SauvolaBinarization(COpenCvImage * inputImage, int argc, char * argv[], bool forceOutput)
{
	if(argc < 7)
	{
		cout << ",ERROR,Missing parameter for operation!"; //CSV output
		return NULL;
	}

	if (typeid(*inputImage) == typeid(COpenCvBiLevelImage))
	{
		cout << ",INFO,Input image already in binary format"; //CSV output
		return forceOutput ? inputImage : NULL;
	}

	int windowSize = _tstoi(CUniString(argv[5]).GetBuffer());
	double weight = _tstof(CUniString(argv[6]).GetBuffer());

	COpenCvBiLevelImage * res = CImageTransformer::SauvolaBinarization(inputImage, weight, windowSize);

	if (res != NULL)
		cout << ",SUCCESS,Operation finished"; //CSV output
	else
		cout << ",ERROR,internal binarisation error"; //CSV output
	return res;
}

/*
 * After 'Adaptive degraded document image binarization' by Gatos et al.
 */
COpenCvImage * AdaptiveBinarization(COpenCvImage * inputImage, bool forceOutput)
{
	if (typeid(*inputImage) == typeid(COpenCvBiLevelImage))
	{
		cout << ",INFO,Input image already in binary format"; //CSV output
		return forceOutput ? inputImage : NULL;
	}

	CAdaptiveBinariser binariser(inputImage);
	binariser.Run();

	COpenCvBiLevelImage * res = binariser.GetOutputImage();

	if (res != NULL)
		cout << ",SUCCESS,Operation finished"; //CSV output
	else
		cout << ",ERROR,internal binarisation error"; //CSV output
	return res;
}

/*
 * Thinning operation (if colour image it will be converted to greyscale first)
 */
COpenCvImage * Erode(COpenCvImage * inputImage, int argc, char * argv[], bool forceOutput)
{

	if (typeid(*inputImage) == typeid(COpenCvColourImage))
	{
		inputImage = CImageTransformer::ConvertToGreyScale(inputImage);
		if (inputImage == NULL)
		{
			cout << ",ERROR,could not convert to greyscale"; //CSV output
			return NULL;
		}
	}

	COpenCvImage * res = CImageTransformer::Erode(inputImage);

	if (res != NULL)
		cout << ",SUCCESS,Operation finished"; //CSV output
	else
		cout << ",ERROR,internal error in erode"; //CSV output
	return res;
}

/*
 * Thickening operation (if colour image it will be converted to greyscale first)
 */
COpenCvImage * Dilate(COpenCvImage * inputImage, int argc, char * argv[], bool forceOutput)
{
	if (typeid(*inputImage) == typeid(COpenCvColourImage))
	{
		inputImage = CImageTransformer::ConvertToGreyScale(inputImage);
		if (inputImage == NULL)
		{
			cout << ",ERROR,could not convert to greyscale"; //CSV output
			return NULL;
		}
	}

	COpenCvImage * res = CImageTransformer::Dilate(inputImage);

	if (res != NULL)
		cout << ",SUCCESS,Operation finished"; //CSV output
	else
		cout << ",ERROR,internal error in dilate"; //CSV output
	return res;
}

/*
 * Rendered vertical or horizontal projection profile
 */
COpenCvBiLevelImage * ProjectionProfile(COpenCvImage * inputImage, bool vertical, int argc, char * argv[], bool forceOutput)
{
	CProjectionProfile projProf;
	if (vertical)
		return projProf.CalculateVerticalProjectionProfile(inputImage);
	else //horizontal
		return projProf.CalculateHorizontalProjectionProfile(inputImage);
}

//Old code for testing:
/*void loadAndSaveImage(char* loadFileName, char* saveFileName) 
{
	cout<<"Load image: "<<loadFileName<<endl;

	CImageReader* pReader = new CTiffImageReader();

	clock_t start(clock());
	CImage* pImg = pReader->ReadImage(loadFileName);
	clock_t finish(clock());

	cout<<"Imgage size: "<<(pImg->GetWidth())<<"*"<<(pImg->GetHeight())<<endl;
	cout<<"Bytes per pixel: "<<(pImg->GetBitsPerPixel())<<endl;
	cout<<"x-res: "<<(pImg->GetXRes())<<endl;
	cout<<"RGB at 100,100: "<<pImg->GetRGBColor(100,100).R<<","<<pImg->GetRGBColor(100,100).G<<","<<pImg->GetRGBColor(100,100).B<<endl;
	cout<<"LineArray [100][100]: "<<((int)pImg->GetLineArray()[100][100])<<endl;
	cout<<"LineArray [200][200]: "<<((int)pImg->GetLineArray()[200][200])<<endl;
	cout<<"Time to load image: "<<(finish-start)<<"ms\n";
	cout<<endl;
	delete pReader;

	if (saveFileName != NULL)
	{
		cout<<"Save image: "<<saveFileName<<endl;
		CImageWriter* pWriter = new CTiffImageWriter();

		clock_t start2(clock());
		pWriter->WriteImage(pImg, saveFileName);
		clock_t finish2(clock());

		cout<<"Time to save image: "<<(finish2-start2)<<"ms\n";
		cout<<endl;

		delete pWriter;
	}
	delete pImg;
}

void resizeImage(char* loadFileName, char* saveFileName)
{
	cout<<"Load image: "<<loadFileName<<endl;

	CImageReader* pReader = new CTiffImageReader();

	clock_t start(clock());
	CImage* pImg = pReader->ReadImage(loadFileName);
	clock_t finish(clock());

	delete pReader;

	CImage * newImage = CImageTransformer::Resize(pImg, 500, 500);

	if (saveFileName != NULL)
	{
		cout<<"Save image: "<<saveFileName<<endl;
		CImageWriter* pWriter = new CTiffImageWriter();

		clock_t start2(clock());
		pWriter->WriteImage(newImage, saveFileName);
		clock_t finish2(clock());

		cout<<"Time to save image: "<<(finish2-start2)<<"ms\n";
		cout<<endl;

		delete pWriter;
	}
	delete pImg;
	delete newImage;
}*/