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
 * Class CImageReader
 *
 * Base class for readers for PRImA image classes
 *
 * Deprecated: Use OpenCV image classes
 */
class DllExport CImageReader
{
public:
	CImageReader(void);
	~CImageReader(void);

	virtual PRImA::CImage * ReadImage(const char* fileName) { return NULL; };

};

}