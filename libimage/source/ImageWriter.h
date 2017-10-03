#pragma once

#include "Image.h"

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
 * Class CImageWriter
 *
 * Base class for writers for PRImA image classes
 *
 * Deprecated: Use OpenCV image classes
 */
class DllExport CImageWriter
{
public:
	CImageWriter(void);
	~CImageWriter(void);

	virtual void WriteImage(PRImA::CImage* pImage, char* fileName) {};
};

}