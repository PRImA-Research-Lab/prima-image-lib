#pragma once

#include "opencvimage.h"

namespace PRImA {

/*
 * Class COpenCvImageWriter
 *
 * Image file writer using OpenCV library.
 *
 * CC 01/11/2013 - created
 */

class COpenCvImageWriter
{
public:
	COpenCvImageWriter(void);
	~COpenCvImageWriter(void);

	bool Write(COpenCvImage * image, CUniString filePath);

private:
	void WriteImageInfo(CImageInfo * info, CUniString filePath);
};

} //end namespace