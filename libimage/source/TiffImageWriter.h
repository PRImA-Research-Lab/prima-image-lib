#pragma once
#include "imagewriter.h"
#include "BiLevelImage.h"
#include "GreyscaleImage.h"
#include "LoColorImage.h"
#include "HiColorImage.h"
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
 * Class CTiffImageWriter
 *
 * Implementation of the CImageWriter class using libtiff.
 *
 * Deprecated: Use OpenCV image classes
 *
 * CC 09.10.2009 - created
 */
class DllExport CTiffImageWriter :
	public CImageWriter
{
public:
	CTiffImageWriter(void);
	~CTiffImageWriter(void);

	void WriteImage(PRImA::CImage* pImage, char* fileName);
	void WriteImage(CBiLevelImage* pImage, char* fileName);
	void WriteImage(CGreyScaleImage* pImage, char* fileName);
	void WriteImage(CLoColorImage* pImage, char* fileName);
	void WriteImage(CHiColorImage* pImage, char* fileName);

private:


	void SetCommonFields(PRImA::CImage* pImage, TIFF* pTif);
};

}