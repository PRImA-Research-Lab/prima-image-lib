#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <limits.h>

#ifndef _MSC_VER
#include <stdint.h>
#else
//#include <pstdint.h>
#endif

#include "opencvimage.h"
#include "ConnectedComponents.h"
#include <list>

#define MVHEIGHT  0.6
#define PEAKRATIO 0.5

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif


namespace PRImA 
{

class CConnectedComponents;

class DllExport CHistogram
{
	// DATA TYPES
public:
	struct Peak
	{
		int LMin;
		int Max;
		int RMin;
	};
	
	struct Valley
	{
		int LMax;
		int Min;
		int RMax;
	};

	//AJF 30/09/10 - CRun - Small inline class to represent run data
	class CValueRun
	{
	public:
		int GetStart()
		{
			return m_Start;
		}
		int GetFinish()
		{
			return m_Finish;
		}
		void SetStart(int start)
		{
			m_Start=start;
		}
		void SetFinish(int finish)
		{
			m_Finish=finish;
		}
	private:
		int m_Start;
		int m_Finish;
	};
	
	// CONSTRUCTION
public:
	CHistogram();
	CHistogram(int NoValues);
	~CHistogram();

	// METHODS
public:
	bool Add(CHistogram * Other);
	void BreakProfile();
	void CreateBaselines(CHistogram * Histogram);
	bool CreateHorzProjProf(COpenCvBiLevelImage * Image);
	bool CreateHorzProjProfVertStrip(COpenCvBiLevelImage * Image, int StartX, int EndX);
	bool CreateHorzProjProf(COpenCvBiLevelImage * Image, int left, int top, int right, int bottom); //AJF 16/09/10

	bool CreateVertProjProf(COpenCvBiLevelImage * Image);
	bool CreateVertProjProf(COpenCvBiLevelImage * Image, int left, int top, int right, int bottom); //AJF 16/09/10

	bool CreateCCHeights(CConnectedComponents * CCs);   // added by PY 28/07/2010

	void SmoothHistogram();
	void SmoothHistogram(int neighbourSize=3); //AJF 20/09/10

	double CalculateGradient(int indexFrom, int indexTo); //AJF 20/09/10
	void CalculateGradients(int sampleResolution=3); //AJF 20/09/10
	double* GetGradients();
	double* GetSmoothedGradients();

	void LocatePeaksValleys(int comparisonWindowSize=3);//AJF 21/09/10

	//void LocateRuns(list<CValueRun>* runList, int valueThreshold/*Value to threshold below*/, int minRun/*Min length of a run*/, int maxRun/*Max length of a run*/, int maxNoiseDuration/*Max continous noise we can tolerate in a run*/); //AJF 30/09/10
	void LocateRuns(list<CValueRun>* runList, int thresholdValue, bool thresholdDirection, int startIndex, int endIndex, int minRun, int maxRun, int maxNoiseDuration);
	void FindBaseLines();
	bool FindMetaPeaks();
	bool FindMetaValleys();
	bool FindPeaks();
	bool FindValleys();
	long GetAverageValue();
	int GetMaxIndex();
	int GetMaxVal();
	int  GetNoPeaks();
	int  GetNoMetaPeaks();
	Peak * GetPeak(int Index);
	Peak * GetMetaPeak(int Index);
	int GetValueCount();
	int GetValue(int Index);
	bool Increment(int Index);
	bool Resize(int NewNoValues);
	bool SetValue(int Index, int Value);

	list<int>* GetPeakList();
	list<int>* GetValleyList();

private:
	int  GetBaseline(int Index);
	int  GetNoBaselines();
	bool IsMax(const int Index);
	bool IsMetaMaxPeaks(const int Index);
	bool IsMetaMaxValleys(const int Index);
	bool IsMetaMinPeaks(const int Index);
	bool IsMetaMinValleys(const int Index);
	bool IsMin(const int Index);

	void LocateRuns(int testIndex, int thresholdValue, bool thresholdDirection, bool walkDirection, list<CValueRun>* runList, CValueRun &run, int &runCounter, int &runStart, int &minRun, int &maxRun, int &noiseCounter, int &noiseInRunCounter, int &maxNoiseDuration);
	// DATA
private:
	int      * m_BaseLines;
	int		 * m_Histogram;
	Peak     * m_Peaks;
	Valley   * m_Valleys;
	Peak     * m_MetaPeaks;
	Valley   * m_MetaValleys;
	int        m_BaseLineCount;
	int		   m_ValueCount;
	int        m_PeakCount;
	int        m_ValleyCount;
	int        m_MetaPeakCount;
	int        m_MetaValleyCount;
	int        m_MaxVal;

	double *m_Gradients;
	double *m_SmoothedGradients;
	list<int> m_PeakList;
	list<int> m_ValleyList;
};

}

#endif // HISTOGRAM_H
