#pragma once
#include "imagereader.h"
#include "Image.h"
#include "HiColorImage.h"
#include "LoColorImage.h"
#include "BiLevelImage.h"
#include "GreyScaleImage.h"
#include "tiffio.h"

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif


namespace PRImA 
{

/*
 * Class CTiffImageReader
 * 
 * Implementation of the CImageReader class using libtiff.
 *
 * Deprecated: Use OpenCV image classes
 *
 * CC 06.10.2009 - created
 */
class DllExport CTiffImageReader : public CImageReader
{
public:
	CTiffImageReader(void);
	~CTiffImageReader(void);

	PRImA::CImage * ReadImage(const char* fileName);

private:
	bool ReadFromTIFF(PRImA::CImage * pImage, TIFF * pTif, char * name);

	bool ReadFromTIFF(CHiColorImage * pImage, TIFF * pTif, char * name);

	bool ReadFromTIFF(CLoColorImage * pImage, TIFF * pTif, char * name);
	
	bool ReadFromTIFF(CBiLevelImage * pImage, TIFF * pTif, char * name);

	bool ReadFromTIFF(CGreyScaleImage * pImage, TIFF * pTif, char * name);

	void getImageSizeAndResolution(PRImA::CImage* pImage, TIFF * pTif);

	void getColorProfile(PRImA::CImage* pImage, TIFF * pTif);

};

}