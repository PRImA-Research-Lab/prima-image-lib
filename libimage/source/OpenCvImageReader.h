#pragma once

#include "opencvimage.h"

namespace PRImA {

/*
 * Class COpenCvImageReader
 *
 * Image file reader using OpenCV library.
 *
 * CC 01/11/2013 - created
 */
class COpenCvImageReader
{
public:

	COpenCvImageReader(void);
	~COpenCvImageReader(void);

	COpenCvImage * Read(CUniString filePath, int enforceType);
	COpenCvImage * Read(CUniString filePath);

	COpenCvImage * ReadMulti(CUniString filePath, int pageIndex, int enforceType);
	COpenCvImage * ReadMulti(CUniString filePath, int pageIndex);

	inline void setDebug(bool debug) { m_Debug = debug; };

private:
	COpenCvImage * Read(CUniString filePath, int pageIndex, int enforceType);

	CImageInfo * ReadImageInfo(CUniString filePath);
	bool m_Debug;
};

} //end namespace