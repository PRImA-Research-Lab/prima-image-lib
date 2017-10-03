#ifndef COMPATIBILITY_H
#define COMPATIBILITY_H

#ifdef _MSC_VER

#include <math.h>

#pragma warning(disable : 4996)

// Removes a warning for converting fopen -> fopen_s which isn't standard
typedef unsigned __int8 uint8_t;
#ifndef BYTE
typedef uint8_t BYTE;
#endif

inline long lrint(double d)
{
	return (long) floor( d + 0.5 );
}


#endif

#endif // COMPATIBILITY_H

