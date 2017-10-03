#pragma once

#include "Algorithm.h"
#include "OpenCvImage.h"
#include "ExtraString.h"
#include "ConnCompCollection.h"
#include "Histogram.h"

namespace PRImA
{

/*
 * Class CAdaptiveBinariser
 *
 * After 'Adaptive degraded document image binarization' by Gatos et al.
 *
 * CC 13/09/2017 - created
 */
class CAdaptiveBinariser : public CAlgorithm
{
public:
	CAdaptiveBinariser(COpenCvImage * inputImage);
	~CAdaptiveBinariser();

	virtual void DoRun();

	COpenCvBiLevelImage * GetOutputImage();

private:
	void ProcessSourceImage();
	void PreprocessSourceImage();
	void EstimateForegroundRegions();
	void EstimateBackgroundSurface();
	void ExtractConnectedComponents();
	void Thresholding();
	void PostProcess();
	void Downsample();
	void SaveImage(COpenCvImage * img, CUniString filePath);

private:
	bool m_Debug;
	bool m_Upsample;
	COpenCvImage * m_InputImage;
	COpenCvBiLevelImage * m_T;
	COpenCvGreyScaleImage * m_Is; //Greyscale source image
	COpenCvGreyScaleImage * m_I; //Greyscale image after preprocessing
	COpenCvGreyScaleImage * m_Iu; //Upsampled greyscale image after preprocessing
	COpenCvBiLevelImage * m_S; //Binary image for foreground estimation
	COpenCvGreyScaleImage * m_B; //Greyscale image with estimated background
	bool m_DeleteGreyScaleSourceImage;
	CConnCompCollection * m_ConnComps;
	CHistogram * m_CompHeightHistogram;
	CHistogram * m_CompWidthHistogram;
	int m_AverageComponentWidth;
	int m_AverageComponentHeight;
};


} //end namespace