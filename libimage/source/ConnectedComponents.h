#ifndef CONNECTEDCOMPONENTS_H
#define CONNECTEDCOMPONENTS_H

#include "opencvImage.h"
#include "Histogram.h"
#include "PointList.h"
#include <set>
#include "algorithm.h"

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif

using namespace std;

namespace PRImA 
{

class CRun;
class CConnectedComponent;
class CHistogram ;

/*
 * Class CConnectedComponents
 *
 * A collection of connected components.
 * Also provides functions to work with components.
 * 
 * Deprecated:
 *    Replaced by CConnCompCollection
 */
class DllExport CConnectedComponents
{
	// CONSTRUCTION
public:
	CConnectedComponents();
	~CConnectedComponents();

	// METHODS
public:
	bool AddCC();
	bool AddCC(CConnectedComponent * CC);
	bool AddNewCC(CConnectedComponent * CC, COpenCvBiLevelImage * Image);  // To add CC from non components
	bool AddCC(CRun * Run);
	bool AddRun(int y, int x1, int x2, bool trackCCs = false);
	CHistogram * CreateBetweenLineNNDistHistogram(double Angle,double Tolerance);
	CHistogram * CreateBetweenLineNNDistHistogramWithNewAngle(double Angle, double Tolerance);
	CHistogram * CreateComponentSizeHistogram();
	bool CreateFromImage(COpenCvBiLevelImage * Image, bool FourConnected = true, bool LookForBlack = true, int minArea = 0, CAlgorithm * stopObject = NULL, CProgressMonitor * progressMonitor = NULL);
	CHistogram * CreateNNAngleHist(double BinAngle);
	CHistogram * CreateNNNewAngleHist(double BinAngle);
	bool CreateSubSet(CConnectedComponents * CC, const int LowerBound, const int UpperBound);
	bool CreateSubSet(CConnectedComponents * CC, CPointList * Reg, const int MinArea = 0, bool componentsMustBeCompletelyInside = true);
	bool CreateSubSet(CConnectedComponents * CCs, CConnectedComponent * CC, const int OffsetX, const int OffsetY);
	CHistogram * CreateWithinLineNNDistHistogram(double Angle,double Tolerance);
	void DeleteCC(CConnectedComponent * CC);
	void DeleteCC(int index);
	void DeleteRun(CRun * run);
	void DeleteArrays(bool deleteUnusedElementsOnly);
	bool FindNearestNeighbours(int K);
	CConnectedComponent * FindOverlappingComponent(CConnectedComponent * cc, bool bestMatch, 
													int xOffset = 0, int yOffset = 0);
	CConnectedComponent * FindComponent(int x, int y, int xOffset = 0, int yOffset = 0, int areaFilter = 0, bool fastSearch = true);
	double GetAverageArea(int noiseFilter = 0);
	double GetAverageAreaN();
	double GetAverageHeight(int noiseFilter = 0);
	double GetAverageWidth(int noiseFilter = 0);
	CConnectedComponent * GetCC(int Index);
	bool GetFourConnected() const;
	int  GetHeight();
	int  GetIndex(CConnectedComponent * CC);
	int  GetCcCount();
	int  GetRunCount(int Y);
	CRun * GetRun(int Y, int Index);
	int  GetWidth() const;
	void SetSize(const unsigned NewWidth, const unsigned NewHeight);
	void SortComponentsX();
	void SortComponentsSizeMedium();

	// To get the broad of textline regions by components size.  Modified by Po Yang
	int  GetCCMinX();
	int  GetCCMaxX();
	int  GetCCMinY();
	int  GetCCMaxY();

private:
	CConnectedComponent * FindComponentUsingRuns(int x, int y, int xOffset = 0, int yOffset = 0, int areaFilter = 0);


	// DATA ITEMS
private:
	// m_bFourConnected = true if 4-Connected, false if 8-Connected
	bool m_bFourConnected;
	int m_nHeight;
	CRun *** m_pRuns;
	int		*	m_RunCount;
	int		*   m_RunAlloc;
	CConnectedComponent ** m_pCCs;
	int		m_CcCount;
	int		m_CCsAlloc;
	int     m_nWidth;
	set<CConnectedComponent*> m_CompsOfCurrentScanline;
	set<CConnectedComponent*> m_CompsOfLastScanline;

	static const int RUN_INIT = 10;
	static const int RUN_INCR = 10;
	static const int CC_INIT = 10;
	static const int CC_INCR = 10;
};

}

#endif // CONNECTEDCOMPONENTS_H
