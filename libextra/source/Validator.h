#pragma once
#include "ExtraString.h"

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
 * Class CValidator
 *
 * Interface for validators.
 * 
 * CC 29.03.2010 - created
 */

class DllExport CValidator
{
public:
	virtual ~CValidator() = 0;

	virtual int validate(void * doc) = 0;
	virtual CUniString GetErrorMsg() = 0;
};

}