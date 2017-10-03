#pragma once

#include "ConnectedComponent.h"
#include "OpenCvImage.h"
#include "Histogram.h"
#include "PointList.h"
#include <vector>
#include <algorithm>
#include "run.h"

using namespace std;

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif

#ifndef CCONNCOMPCOLLECTION_H
#define CCONNCOMPCOLLECTION_H

bool ComponentComparatorByX(PRImA::CConnectedComponent * comp1, PRImA::CConnectedComponent * comp2);

namespace PRImA 
{

/*
 * Class CConnCompCollection
 *
 * A collection of connected components.
 * Also provides functions to work with components.
 * 
 * Note:
 *    This class is a replacement for the old class CConnectedComponents.
 *
 * CC 02.09.2010 - Created
 */
class DllExport CConnCompCollection
{
public:
	CConnCompCollection(bool deleteComponentsOnDestruction = true);
	~CConnCompCollection(void);

public:
	static void DrawComponent(CConnectedComponent * comp, COpenCvBiLevelImage * image);
	static void DrawComponent(CConnectedComponent * comp, COpenCvBiLevelImage * image, int offsetX, int offsetY);

	//inline bool IsFourConnected() { return m_FourConnected; };

	void						AddComponent(CConnectedComponent * comp);
	void						AddComponent(CRun * run);
	CConnectedComponent		*	GetComponent(int index);
	int							GetSize();
	void						DeleteComponent(CConnectedComponent * component);
	void						DeleteComponent(int index);
	void						DeleteAll();


	CHistogram * CreateBetweenLineNNDistHistogram(double Angle,double Tolerance); 
	CHistogram * CreateComponentSizeNNHistogram();
	CHistogram * CreateNNAngleHist(double BinAngle);
	CHistogram * CreateWithinLineNNDistHistogram(double Angle,double Tolerance);
	CHistogram * CreateComponentHeightNNHistogram();
	CHistogram * CreateComponentWidthNNHistogram();

	bool ExtractComponentsFromImage(COpenCvBiLevelImage * image, bool fourConnected = true, bool lookForBlack = true);
	bool ExtractComponentsFromImage(COpenCvBiLevelImage * image, int x1, int y1, int x2, int y2, bool fourConnected = true, bool lookForBlack = true);

	CConnCompCollection * CreateSubSet(CPointList * outline, const int minArea = 0, 
										bool componentsMustBeCompletelyInside = true);
	CConnCompCollection * CreateSubSet(CConnectedComponent * component, const int offsetX, const int offsetY);

	bool FindNearestNeighbours(int k);

	double GetAverageArea();
	double GetAverageHeight();
	double GetAverageWidth();

	double GetAverageHeight(int noiseFilter = 0);
	double GetAverageWidth(int noiseFilter = 0);
	double GetAverageArea(int noiseFilter = 0);

	void SortComponents(bool (*comparatorFunction)(CConnectedComponent*,CConnectedComponent*));
	void SortComponentsByX();

	int GetMaxX();
	int GetMinX();
	int GetMaxY();
	int GetMinY();

	//inline int GetWidth() { return m_Width; };
	//inline int GetHeight() { return m_Height; };
	inline void SetDimensions(int width, int height) { m_Width = width; m_Height = height; };

private:
	bool AddRun(vector<vector<CRun*>> & runs, int y, int x1, int x2, bool fourConnected);

private:
	vector<CConnectedComponent*> m_Components;
	//bool m_FourConnected;
	bool m_DeleteComponentsOnDestruction;
	int m_Width;
	int m_Height;
};

}

#else
namespace PRImA
{
class CConnCompCollection;
}
#endif

