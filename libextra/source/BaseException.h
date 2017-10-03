#pragma once

#include "extrastring.h"

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif

namespace PRImA 
{

class DllExport CBaseException
{
public:
	CBaseException(CUniString msg);
	~CBaseException(void);
	inline CUniString GetMsg() { return m_Msg; };

private:
	CUniString m_Msg;
};

}