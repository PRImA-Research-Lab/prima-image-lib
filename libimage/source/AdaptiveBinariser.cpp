#include "AdaptiveBinariser.h"
#include "ImageTransformer.h"
#include "WienerFilter.h"
#include "OpenCvImageWriter.h"

namespace PRImA
{

/*
 * Class CAdaptiveBinariser
 *
 * After 'Adaptive degraded document image binarization' by Gatos et al.
 *
 * CC 13/09/2017 - created
 */

/*
 * Constructor
 */
CAdaptiveBinariser::CAdaptiveBinariser(COpenCvImage * inputImage) : CAlgorithm(L"AdaptiveBinariser", L"After 'Adaptive degraded document image binarization' by Gatos et al.")
{
	m_Debug = false;
	m_Upsample = true;
	m_InputImage = inputImage;
	m_T = NULL;
	m_Is = NULL;
	m_I = NULL;
	m_Iu = NULL;
	m_S = NULL;
	m_B = NULL;
	m_ConnComps = NULL;
	m_CompHeightHistogram = NULL;
	m_CompWidthHistogram = NULL;
	m_AverageComponentWidth = 3;
	m_AverageComponentHeight = 3;
	m_DeleteGreyScaleSourceImage = true;
}

/*
 * Destructor
 */
CAdaptiveBinariser::~CAdaptiveBinariser()
{
	if (m_DeleteGreyScaleSourceImage)
		delete m_Is;
	delete m_I;
	delete m_Iu;
	delete m_S;
	delete m_B;
	delete m_ConnComps;
	delete m_CompWidthHistogram;
	delete m_CompHeightHistogram;
}

/*
 * Run
 */
void CAdaptiveBinariser::DoRun()
{
	if (m_InputImage == NULL)
		return;

	//Prepare image
	// Already a B/W image?
	COpenCvBiLevelImage * bwImage = dynamic_cast<COpenCvBiLevelImage*>(m_InputImage);
	if (bwImage != NULL)
	{
		m_T = (COpenCvBiLevelImage*)m_InputImage->Clone();
		m_Success = true;
		return;
	}

	SetProgress(5);

	// Colour image?
	COpenCvColourImage * colourImage = dynamic_cast<COpenCvColourImage*>(m_InputImage);
	if (colourImage != NULL)
	{
		m_Is = CImageTransformer::ConvertToGreyScale(colourImage);
	}
	else //Already greyscale
	{
		m_Is = (COpenCvGreyScaleImage*)m_InputImage;
		m_DeleteGreyScaleSourceImage = false;
	}

	if (m_Debug)
		SaveImage(m_Is, L"c:\\temp\\adaptiveBinariser\\Is.png");

	SetProgress(10);

	//Run method
	ProcessSourceImage();

	m_Success = true;
}

/*
 * Binarise source image
 */
void CAdaptiveBinariser::ProcessSourceImage()
{
	//Preprocess using Wiener filter
	PreprocessSourceImage();
	SetProgress(20);

	//Rough estimation of foreground regions
	EstimateForegroundRegions();
	SetProgress(30);

	//Background surface estimation using interpolation
	EstimateBackgroundSurface();
	SetProgress(40);

	//Final thresholding
	Thresholding();
	SetProgress(70);

	//Post-processing
	PostProcess();
	SetProgress(90);

	//Down-sample
	if (m_Upsample)
		Downsample();
	SetProgress(100);
}

/*
 * Preprocess greyscale source image using Weiner filter
 */
void CAdaptiveBinariser::PreprocessSourceImage()
{
	cv::Mat1b dst33;
	//m_I = COpenCvImage::CreateG(m_Is->GetWidth(), m_Is->GetHeight(), RGBWHITE);

	double estimatedNoiseVariance;

	// Call to WienerFilter function with a 3x3 kernel and estimated noise variances
	estimatedNoiseVariance = WienerFilter(m_Is->GetData(), dst33, cv::Size(3, 3));

	m_I = (COpenCvGreyScaleImage*)COpenCvImage::Create(dst33, COpenCvImage::TYPE_GREYSCALE, false);

	if (m_Debug)
		SaveImage(m_I, L"c:\\temp\\adaptiveBinariser\\I.png");
}

/*
 * Rough estimation of foreground regions using Sauvola method
 */
void CAdaptiveBinariser::EstimateForegroundRegions()
{
	m_S = CImageTransformer::SauvolaBinarization(m_I, 0.2, 40);

	ExtractConnectedComponents();

	if (m_Debug)
		SaveImage(m_S, L"c:\\temp\\adaptiveBinariser\\S.tif");
}

/*
 * Get connected components for bitonal image S and calculate width and height histograms
 */
void CAdaptiveBinariser::ExtractConnectedComponents()
{
	//Extract
	m_ConnComps = new CConnCompCollection();
	m_ConnComps->ExtractComponentsFromImage(m_S);

	//Histograms
	m_CompHeightHistogram = m_ConnComps->CreateComponentHeightNNHistogram();
	m_CompWidthHistogram = m_ConnComps->CreateComponentWidthNNHistogram();

	m_AverageComponentWidth = max(10, (int)m_ConnComps->GetAverageHeight(3)); //m_CompWidthHistogram->GetMaxIndex());
	m_AverageComponentHeight = max(10, (int)m_ConnComps->GetAverageWidth(3)); //m_CompHeightHistogram->GetMaxIndex());
}

/*
 * Background surface estimation using interpolation
 */
void CAdaptiveBinariser::EstimateBackgroundSurface()
{
	m_B = (COpenCvGreyScaleImage*)m_I->Clone();

	int dx = m_AverageComponentHeight;
	int dy = m_AverageComponentHeight;
	
	for (int y = 0; y < m_I->GetHeight(); y++)
	{
		for (int x = 0; x < m_I->GetWidth(); x++)
		{
			//Foreground pixel?
			if (m_S->IsBlack(x, y))
			{
				//Interpolate
				int sum1 = 0;
				int sum2 = 0;
				//int val = 0;
				for (int iy = y - dy; iy <= y + dy; iy++)
				{
					for (int ix = x - dx; ix <= x + dx; ix++)
					{
						//val = m_S->IsBlack(ix, iy, false) ? 0 : 1;
						if (m_S->IsWhite(ix, iy, true))
						{
							sum1 += m_I->GetGreyLevel(ix, iy, 255);
							sum2++;
						}
					}
				}
				if (sum2 > 0)
					m_B->SetGreyLevel(x, y, (int)((double)sum1 / (double)sum2));
				else
					m_B->SetGreyLevel(x, y, 255);
			}
		}
	}

	if (m_Debug)
		SaveImage(m_B, L"c:\\temp\\adaptiveBinariser\\B.tif");

	//Smooth
	/*COpenCvGreyScaleImage * temp = COpenCvImage::CreateG(m_B->GetWidth(), m_B->GetHeight(), RGBWHITE);
	cv::blur(m_B->GetData(), temp->GetData(), cv::Size(9, 9));

	if (m_Debug)
		SaveImage(temp, L"c:\\temp\\adaptiveBinariser\\B2.tif");

	delete m_B;
	m_B = temp;*/

}

/*
 * Final thresholding
 */
void CAdaptiveBinariser::Thresholding()
{
	double d = 0.0;
	double q = 0.6;
	double p1 = 0.5;
	double p2 = 0.8;
	double delta = 0.0;
	double b = 0.0; //Average background value 

	//Calculate b
	int count = 0;
	int sum = 0;
	for (int y = 0; y < m_B->GetHeight(); y++)
	{
		for (int x = 0; x < m_B->GetWidth(); x++)
		{
			if (m_S->IsBlack(x, y))
			{
				count++;
				sum += m_B->GetGreyLevel(x, y);
			}
			//if (m_S->IsWhite(x, y))
			//{
			//	count++;
			//	sum += m_B->GetGreyLevel(x, y);
			//}
		}
	}
	if (count > 0)
		b = (double)sum / (double)count;

	//Calculate delta
	int sum1 = 0;
	int sum2 = 0;
	for (int y = 0; y < m_B->GetHeight(); y++)
	{
		for (int x = 0; x < m_B->GetWidth(); x++)
		{
			sum1 += m_B->GetGreyLevel(x, y) - m_I->GetGreyLevel(x, y);
			if (m_S->IsBlack(x, y))
				sum2 += 1;
		}
	}
	delta = (double)sum1 / (double)sum2;

	//Threshold
	if (m_Upsample)
	{
		int newWidth = m_I->GetWidth() * 2;
		int newHeight = m_I->GetHeight() * 2;
		cv::Mat resizedData(newHeight, newWidth, m_I->GetData().type());

		int method = cv::INTER_CUBIC;

		cv::resize(m_I->GetData(), resizedData, cv::Size(newWidth, newHeight), 0.0, 0.0, method);

		m_Iu = (COpenCvGreyScaleImage*)COpenCvImage::Create(resizedData, COpenCvImage::TYPE_GREYSCALE, false);

		m_T = COpenCvImage::CreateB(m_Iu->GetWidth(), m_Iu->GetHeight(), RGBWHITE);

		int x = 0, y = 0;
		for (int yu = 0; yu < m_T->GetHeight(); yu++)
		{
			y = yu / 2;
			for (int xu = 0; xu < m_T->GetWidth(); xu++)
			{
				x = xu / 2;
				d = q * delta * ((1.0 - p2) / (1.0 + exp((-4.0*m_B->GetGreyLevel(x, y) / (b * (1.0 - p1))) + 2.0*(1.0 + p1) / (1.0 - p1))) + p2);

				if (m_B->GetGreyLevel(x, y) - m_Iu->GetGreyLevel(xu, yu) > d)
					m_T->SetBlack(xu, yu);
			}
		}
	}
	else //Don't upsample
	{
		m_T = COpenCvImage::CreateB(m_I->GetWidth(), m_I->GetHeight(), RGBWHITE);

		for (int y = 0; y < m_T->GetHeight(); y++)
		{
			for (int x = 0; x < m_T->GetWidth(); x++)
			{
				d = q * delta * ((1.0 - p2) / (1.0 + exp((-4.0*m_B->GetGreyLevel(x, y) / (b * (1.0 - p1))) + 2.0*(1.0 + p1) / (1.0 - p1))) + p2);

				if (m_B->GetGreyLevel(x, y) - m_I->GetGreyLevel(x, y) > d)
					m_T->SetBlack(x, y);
			}
		}
	}

	if (m_Debug)
		SaveImage(m_T, L"c:\\temp\\adaptiveBinariser\\T.tif");
}

/*
 * Post-processing using growing and shrinking
 */
void CAdaptiveBinariser::PostProcess()
{
	int lh = 2 * m_AverageComponentHeight;
	int n = (int)(0.15 * (double)lh);
	int n2 = n / 2;
	int ksh = (int)(0.9 * (double)(n * n));
	int psh = 0;

	//Shrink
	COpenCvBiLevelImage * temp = (COpenCvBiLevelImage*)m_T->Clone();
	for (int y = 0; y < m_T->GetHeight(); y++)
	{
		for (int x = 0; x < m_T->GetWidth(); x++)
		{
			if (m_T->IsBlack(x, y))
			{
				psh = m_T->CountPixels(false, x - n2, y - n2, x + n2, y + n2, false);
				if (psh > ksh)
					temp->SetWhite(x, y);
			}
		}
	}
	if (m_Debug)
		SaveImage(temp, L"c:\\temp\\adaptiveBinariser\\T2.tif");
	delete m_T;
	m_T = temp;

	//Grow
	int ksw = (int)(0.05 * (double)(n * n));
	int psw = 0;
	int dx = (int)(0.25 * n);
	int dy = dx;
	temp = (COpenCvBiLevelImage*)m_T->Clone();
	for (int y = 0; y < m_T->GetHeight(); y++)
	{
		for (int x = 0; x < m_T->GetWidth(); x++)
		{
			if (m_T->IsWhite(x, y))
			{
				psw = m_T->CountPixels(false, x - n2, y - n2, x + n2, y + n2, true);

				int sumx = 0;
				int sumy = 0;
				if (m_Upsample)
				{
					for (int iy = y - dy; iy <= y + dy; iy++)
					{
						for (int ix = x - dx; ix <= x + dx; ix++)
						{
							if (m_S->IsBlack(ix/2, iy/2, true))
							{
								sumx += ix;
								sumy += iy;
							}
						}
					}
				}
				else //Not upsampled
				{
					for (int iy = y - dy; iy <= y + dy; iy++)
					{
						for (int ix = x - dx; ix <= x + dx; ix++)
						{
							if (m_S->IsBlack(ix, iy, true))
							{
								sumx += ix;
								sumy += iy;
							}
						}
					}
				}
				int xa = x;
				if (psw > 0)
					xa = (int)((double)sumx / (double)psw);
				int ya = y;
				if (psw > 0)
					ya = (int)((double)sumy / (double)psw);

				if (psw > ksw && abs(x - xa) < dx && abs(y - ya) < dy)
					temp->SetBlack(x, y);
			}
		}
	}
	if (m_Debug)
		SaveImage(temp, L"c:\\temp\\adaptiveBinariser\\T3.tif");
	delete m_T;
	m_T = temp;

	//Grow 2
	int ksw2 = (int)(0.35 * (double)(n * n));
	temp = (COpenCvBiLevelImage*)m_T->Clone();
	for (int y = 0; y < m_T->GetHeight(); y++)
	{
		for (int x = 0; x < m_T->GetWidth(); x++)
		{
			if (m_T->IsWhite(x, y))
			{
				psw = m_T->CountPixels(false, x - n2, y - n2, x + n2, y + n2, true);

				if (psw > ksw2)
					temp->SetBlack(x, y);
			}
		}
	}
	if (m_Debug)
		SaveImage(temp, L"c:\\temp\\adaptiveBinariser\\T4.tif");
	delete m_T;
	m_T = temp;
}

/* 
 * Donwsample thresholded image
 */
void CAdaptiveBinariser::Downsample()
{
	int newWidth = m_I->GetWidth();
	int newHeight = m_I->GetHeight();
	cv::Mat resizedData(newHeight, newWidth, m_T->GetData().type());

	int method = cv::INTER_CUBIC;

	cv::resize(m_T->GetData(), resizedData, cv::Size(newWidth, newHeight), 0.0, 0.0, method);

	delete m_T;
	m_T = (COpenCvBiLevelImage*)COpenCvImage::Create(resizedData, COpenCvImage::TYPE_BILEVEL, true);


}

//The binarised image
COpenCvBiLevelImage * CAdaptiveBinariser::GetOutputImage()
{
	return m_T;
}

//Save an image to disk
void CAdaptiveBinariser::SaveImage(COpenCvImage * img, CUniString filePath)
{
	COpenCvImageWriter writer;
	writer.Write(img, filePath);

}

} //end namespace