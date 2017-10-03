#pragma once
#include "imagereader.h"
#include "Image.h"
#include "HiColorImage.h"
#include "LoColorImage.h"
#include "BiLevelImage.h"
#include "GreyScaleImage.h"

class CTiffImageReader : public CImageReader
{
public:
	CTiffImageReader(void);
	~CTiffImageReader(void);

	CImage * ReadImage(const char* fileName);

private:
	//bool ReadFromTIFF(CImage * pImage, TIFF * pTif, char * name);

	bool ReadFromTIFF(CHiColorImage * pImage, TIFF * pTif, char * name);

	bool ReadFromTIFF(CLoColorImage * pImage, TIFF * pTif, char * name);
	
	bool ReadFromTIFF(CBiLevelImage * pImage, TIFF * pTif, char * name);

	bool ReadFromTIFF(CGreyScaleImage * pImage, TIFF * pTif, char * name);

	void getImageSizeAndResolution(CImage* pImage, TIFF * pTif);
};
