/*

AJF - 16/09/10

1. Removed the following from each of the projection profile methods as using a local variable and user may not want the maxima calculating:

int MaxVal = 0;

for(y = 0; y < m_ValueCount; y++)
{
	if(m_Histogram[y] > MaxVal)
	{
		MaxVal = m_Histogram[y];
	}
}

2. Updated loops of projection profiles to '< width' from '<= width'

3. Added overloaded methods to calc projection profiles for bounding boxes

4. Added GetMaxVal which returns the value of the largest bin (index)
*/
#include "Histogram.h"
#include "ConnectedComponent.h"
#include <vector>
#include <iostream>

using namespace PRImA;

// CONSTRUCTION

CHistogram::CHistogram()
{
	m_BaseLines = (int *) NULL;
	m_ValueCount = 0;
	m_Histogram = NULL;
	m_BaseLineCount = 0;
	m_PeakCount = 0;
	m_ValleyCount = 0;
	m_MetaPeakCount = 0;
	m_MetaValleyCount = 0;
	m_Peaks = (Peak *) NULL;
	m_Valleys = (Valley *) NULL;
	m_MetaPeaks = NULL;
	m_MetaValleys = NULL;
	m_MaxVal=0;
	m_Gradients = NULL;
	m_SmoothedGradients = NULL;
}

CHistogram::CHistogram(int NoValues)
{
	m_BaseLines = NULL;
	m_ValueCount = NoValues;
	m_Histogram = new int[NoValues];
	memset(m_Histogram,0,sizeof(int) * NoValues);
	m_BaseLineCount = 0;
	m_PeakCount = 0;
	m_ValleyCount = 0;
	m_MetaPeakCount = 0;
	m_MetaValleyCount = 0;
	m_Peaks = NULL;
	m_Valleys = NULL;
	m_MetaPeaks = NULL;
	m_MetaValleys = NULL;
	m_Gradients = NULL;
	m_SmoothedGradients = NULL;
}

CHistogram::~CHistogram()
{
	delete [] m_BaseLines;
	delete [] m_Histogram;
	delete [] m_Peaks;
	delete [] m_Valleys;
	delete [] m_MetaPeaks;
	delete [] m_MetaValleys;

	if(m_Gradients!=NULL)
		delete m_Gradients;
	
	if(m_SmoothedGradients!=NULL)
		delete m_SmoothedGradients;

}

// METHODS

bool CHistogram::Add(CHistogram * Other)
{
	int i;
	
	for(i = 0; i < m_ValueCount; i++)
		m_Histogram[i] += Other->GetValue(i);

	return true;
}

void CHistogram::BreakProfile()
{
	int i;

	for(i=0;i<m_MetaValleyCount - 1;i++)
	{
		if(m_MetaValleys[i].RMax < m_ValleyCount)
		{
			if
			(
				double(m_Histogram[m_Valleys[m_MetaValleys[i].Min].Min]) / double(m_Histogram[m_Valleys[m_MetaValleys[i].LMax].Min]) > MVHEIGHT &&
				double(m_Histogram[m_Valleys[m_MetaValleys[i].Min].Min]) / double(m_Histogram[m_Valleys[m_MetaValleys[i].RMax].Min]) > MVHEIGHT
			)
			{
				m_Histogram[m_Valleys[m_MetaValleys[i].Min].Min] = 0;
			}
		}
	}

}

bool CHistogram::CreateHorzProjProf(COpenCvBiLevelImage * Image)
{
	int x, y;

	delete [] m_Histogram;

	m_ValueCount = Image->GetHeight();
	m_Histogram = new int[m_ValueCount];
	if(m_Histogram==NULL)
	{
		return false;
	}
	memset(m_Histogram,0,sizeof(int) * m_ValueCount);

	for(y = 0; y < Image->GetHeight(); y++)
	{
		for(x = 0; x < Image->GetWidth(); x++)
		{
			if(Image->IsBlack(x,y))
				m_Histogram[y]++;
		}
	}

	return true;
}

bool CHistogram::CreateHorzProjProfVertStrip(COpenCvBiLevelImage * Image, int StartX, int EndX)
{
	int x, y;

	if(StartX >= Image->GetWidth() || EndX>=Image->GetWidth() || StartX > EndX)
		return false;

	delete m_Histogram;

	m_ValueCount = Image->GetHeight();
	m_Histogram = new int[m_ValueCount];

	if(m_Histogram==NULL)
		return false;

	memset(m_Histogram, 0, sizeof(int) * m_ValueCount);

	for(y = 0; y < m_ValueCount; y++)
	{
		for(x = StartX; x <= EndX; x++)
		{
			if(Image->IsBlack(x,y))
				m_Histogram[y]++;
		}
	}

	return true;
}

bool CHistogram::CreateVertProjProf(COpenCvBiLevelImage * Image)
{
	int x, y;

	delete m_Histogram;

	m_ValueCount = Image->GetWidth();
	m_Histogram = new int[m_ValueCount];
	if(m_Histogram==NULL)
	{
		return false;
	}
	memset(m_Histogram,0,sizeof(int) * m_ValueCount);

	for(y = 0; y < Image->GetHeight(); y++)
	{
		for(x = 0; x < Image->GetWidth(); x++)
		{
			if(Image->IsBlack(x,y))
				m_Histogram[x]++;
		}
	}

	return true;
}

/*
*Creates vertical projection profile for a bounding box 
*/
bool CHistogram::CreateVertProjProf(COpenCvBiLevelImage * Image, int left, int top, int right, int bottom)
{
	delete m_Histogram;

	m_ValueCount =	right - left + 1; //Total number of bins
	m_Histogram = new int[m_ValueCount];
	
	if(m_Histogram==NULL)
	{
		return false;
	}
	
	memset(m_Histogram,0,sizeof(int) * m_ValueCount);

	int binIndex=0;
	for(int x = left; x <= right; x++)
	{
		for(int y = top; y <= bottom; y++)
		{
			if(Image->IsBlack(x,y))
				m_Histogram[binIndex]++;
		}
		binIndex++;
	}

	return true;
}

/*
*Creates horizontal projection profile for a bounding box 
*/
bool CHistogram::CreateHorzProjProf(COpenCvBiLevelImage * Image, int left, int top, int right, int bottom)
{
	delete m_Histogram;

	m_ValueCount =	top - bottom + 1; //Total number of bins
	m_Histogram = new int[m_ValueCount];
	
	if(m_Histogram==NULL)
	{
		return false;
	}
	
	memset(m_Histogram,0,sizeof(int) * m_ValueCount);

	int binIndex=0;
	for(int y = top; y <= bottom; y++)
	{
		for(int x = left; x <= right; x++)
		{
			if(Image->IsBlack(x,y))
				m_Histogram[binIndex]++;
		}
		binIndex++;
	}

	return true;
}

bool CHistogram::CreateCCHeights(CConnectedComponents * CCs)
{
	//int x, y;

	delete m_Histogram;

	//int temp_ValueCount = 1 ;

    vector<int> temp_Value  ;
	vector<int> temp_Value_Histogram ; 

	temp_Value.push_back(CCs->GetCC(0)->GetY2()- CCs->GetCC(0)->GetY1());  // initilization 
	temp_Value_Histogram.push_back(1) ;

	for(int i = 0 ; i < CCs->GetCcCount(); i++)
	{
	   CConnectedComponent * CC1 = CCs->GetCC(i);

	   int temp_H = CC1->GetY2() - CC1->GetY1();
	   
	   bool flag = true ; 

       if(temp_Value_Histogram.size()==temp_Value.size())
	   {
	      for(unsigned int j = 0 ; j < temp_Value.size(); j++)
	      {
	       if(temp_H == temp_Value.at(j))
		   {
               temp_Value_Histogram.at(j) ++ ;

			   flag = false ;

			   break ;
		   }
	   
	      }
	   }


	   if(flag == true)
	   {
		  for(unsigned int j = 0 ; j < temp_Value.size(); j++)
	      {
	        if( j != 0 && j != temp_Value.size() - 1)
		    {
				
				if (temp_H > temp_Value.at(j) && temp_H < temp_Value.at(j+1))
				{
					temp_Value.insert(temp_Value.begin()+j+1, temp_H);

		            temp_Value_Histogram.insert(temp_Value_Histogram.begin()+j+1, 1) ;
				
				}
		    }
			else if (j == 0 )
			{
			    if(temp_H < temp_Value.at(j))
				{
				 
					temp_Value.insert(temp_Value.begin(), temp_H);

		        
					temp_Value_Histogram.insert(temp_Value_Histogram.begin(), 1) ;
				}
			}
			else if (j == temp_Value.size() - 1 )
			{
                if(temp_H > temp_Value.at(j))			   
				{
					temp_Value.insert(temp_Value.end(), temp_H);

		            temp_Value_Histogram.insert(temp_Value_Histogram.end(), 1) ;
				}
			}
	   
	      }
		      
	   }
	
	
	}

	// To store the temp value in Histogram

	m_ValueCount = temp_Value.at(temp_Value.size() - 1) ;
	m_Histogram = new int[m_ValueCount];
	if(m_Histogram==NULL)
	{
		return false;
	}

	memset(m_Histogram,0,sizeof(int) * m_ValueCount);

	for(int i = 0; i < m_ValueCount ; i++) ////  not right !!!
	{

		for(unsigned int j = 0 ; j < temp_Value.size(); j++)
		{
           if(i == temp_Value.at(j))
		   {
			   m_Histogram[i] = temp_Value_Histogram.at(j) ;
		       break ;
		   }

		}

	}

	return true;
}


void CHistogram::SmoothHistogram()
{
    for(int x = 1 ; x < m_ValueCount; x++)
	{
	     if(m_Histogram[x] == 0)
		 {
			 int i = 1 ;
		     while(m_Histogram[x-i]==0 && (x-i) > 0)
			 {
			     i ++ ;		 
			 }

             m_Histogram[x] = m_Histogram[x-i];
		 }
	
	}
 
}

/*
*AJF - 20/09/10
*SmoothHistogram(int) - walks along histogram, computes the average value for a neighbourhood of bins/indexes then writes this to the centre value of the neighbourhood
*Doesn't smooth neighbour size /2 at begining and end
*TODO: Improve this ^^^!
*/
void CHistogram::SmoothHistogram(int neighbourSize /* should be an odd number */)
{
	int cen=(int)(neighbourSize/2);

	int *histogramSmoothed = new int[m_ValueCount];
	memset(histogramSmoothed,0,sizeof(int) * m_ValueCount);

	for(int i=0; i < m_ValueCount-neighbourSize; i++){
		long h=0;
		for(int j=i; j<i+neighbourSize; j++){
			h += (long)m_Histogram[j];
		}
		histogramSmoothed[i+cen]=(int)(h/neighbourSize);
	}

	memcpy(m_Histogram, histogramSmoothed, sizeof(int) * m_ValueCount);

	delete histogramSmoothed;
}


void CHistogram::CreateBaselines(CHistogram * Histogram)
{
	int x;

	delete m_Histogram;

	m_ValueCount = Histogram->GetValueCount();

	m_Histogram = new int[m_ValueCount];
	memset(m_Histogram,0,sizeof(int) * m_ValueCount);

	for(x = 0; x < Histogram->GetNoBaselines()-1; x++)
	{
		m_Histogram[Histogram->GetBaseline(x+1) - Histogram->GetBaseline(x)]++;
	}
}

void CHistogram::FindBaseLines()
{
	int i;

	delete [] m_BaseLines;
	m_BaseLines = new int[m_ValleyCount];
	m_BaseLineCount = 0;

	for(i=0;i<m_ValleyCount;i++)
	{
		if(m_Histogram[m_Valleys[i].Min]==0)
		{
			if(i>0)
			{
				if(double(m_Histogram[m_Valleys[i-1].RMax]) / double(m_Histogram[m_Valleys[i-1].LMax]) < PEAKRATIO)
				{
					m_BaseLines[m_BaseLineCount] = m_Valleys[i-1].LMax;
				}
				else
				{
					m_BaseLines[m_BaseLineCount] = m_Valleys[i-1].RMax;
				}
				m_BaseLineCount++;
			}
		}
	}
}

bool CHistogram::FindMetaPeaks()
{
	bool LastWasMin = false;
	int i;

	// Delete Previous MetaPeaks
	delete [] m_MetaPeaks;
	m_MetaPeaks = NULL;
	m_MetaPeaks = 0;
	
	if(m_Peaks == 0)
		return true;
	
	for(i = 0; i < m_PeakCount; i++)
	{
		if(IsMetaMinPeaks(i) && LastWasMin==false)
		{
			if(m_MetaPeakCount > 0)
				m_MetaPeaks[m_MetaPeakCount-1].RMin = i;
			
			m_MetaPeakCount++;
			Peak * temp = new Peak[m_MetaPeakCount];
			if(temp==NULL)
			{
				delete [] m_MetaPeaks;
				m_MetaPeaks = 0;
				return false;
			}
			memcpy(temp,m_MetaPeaks,sizeof(Peak) * (m_MetaPeakCount - 1));
			temp[m_MetaPeakCount-1].LMin =  i;
			temp[m_MetaPeakCount-1].Max  = -1;
			temp[m_MetaPeakCount-1].RMin = -1;
			delete [] m_MetaPeaks;
			m_MetaPeaks = temp;
			LastWasMin = true;
		}
		else if(IsMetaMaxPeaks(i) && LastWasMin==true && m_MetaPeakCount > 0)
		{
			m_MetaPeaks[m_MetaPeakCount-1].Max = i;
			LastWasMin = false;
		}
	}

	if(m_MetaPeakCount > 0)
	{
		if
		(
			m_MetaPeaks[m_MetaPeakCount-1].LMin == -1 ||
			m_MetaPeaks[m_MetaPeakCount-1].Max  == -1 ||
			m_MetaPeaks[m_MetaPeakCount-1].RMin == -1
		)
		{
			m_MetaPeakCount--;
		}
	}
	
	return true;
}

bool CHistogram::FindMetaValleys()
{
	bool LastWasMax = false;
	int i;

	// Delete Previous MetaValleys
	delete [] m_MetaValleys;
	m_MetaValleys = NULL;
	m_MetaValleys = 0;
	
	if(m_Valleys == 0)
		return true;
	
	for(i = 0; i < m_ValleyCount; i++)
	{
		if(IsMetaMaxValleys(i) && LastWasMax==false)
		{
			if(m_MetaValleyCount > 0)
				m_MetaValleys[m_MetaValleyCount-1].RMax = i;
			
			//m_MetaValleys++;		//CC 14.10.2010
			m_MetaValleyCount++;	//CC 14.10.2010
			Valley * temp = new Valley[m_MetaValleyCount];
			if(temp==NULL)
			{
				delete [] m_MetaValleys;
				m_MetaValleyCount = 0;
				return false;
			}
			memcpy(temp,m_MetaValleys,sizeof(Valley) * (m_MetaValleyCount - 1));
			temp[m_MetaValleyCount-1].LMax =  i;
			temp[m_MetaValleyCount-1].Min  = -1;
			temp[m_MetaValleyCount-1].RMax = -1;
			delete [] m_MetaValleys;
			m_MetaValleys = temp;
			LastWasMax = true;
		}
		else if(IsMetaMinValleys(i) && LastWasMax==true && m_MetaValleyCount > 0)
		{
			m_MetaValleys[m_MetaValleyCount-1].Min = i;
			LastWasMax = false;
		}
	}

	if( m_MetaValleys != NULL &&
		(m_MetaValleys[m_MetaValleyCount-1].LMax == -1 ||
		 m_MetaValleys[m_MetaValleyCount-1].Min  == -1 ||
		 m_MetaValleys[m_MetaValleyCount-1].RMax == -1) )
	{
		m_MetaValleyCount--;
	}
	
	return true;
}

bool CHistogram::FindPeaks()
{
	bool LastWasMin = false;
	int i;

	// Delete Previous Peaks
	delete [] m_Peaks;
	m_Peaks = NULL;
	m_Peaks = 0;
	
	for(i = 0; i < m_ValueCount; i++)
	{
		if(IsMin(i) && LastWasMin==false)
		{
			if(m_PeakCount > 0)
				m_Peaks[m_PeakCount-1].RMin = i;
			
			m_Peaks++;
			Peak * temp = new Peak[m_PeakCount];
			if(temp==NULL)
			{
				delete [] m_Peaks;
				m_PeakCount = 0;
				return false;
			}
			memcpy(temp,m_Peaks,sizeof(Peak) * (m_PeakCount - 1));
			temp[m_PeakCount-1].LMin =  i;
			temp[m_PeakCount-1].Max  = -1;
			temp[m_PeakCount-1].RMin = -1;
			delete [] m_Peaks;
			m_Peaks = temp;
			LastWasMin = true;
		}
		else if(IsMax(i) && LastWasMin==true && m_PeakCount > 0)
		{
			m_Peaks[m_PeakCount-1].Max = i;
			LastWasMin = false;
		}
	}

	if(m_PeakCount > 0)
	{
		if
		(
			m_Peaks[m_PeakCount-1].LMin == -1 ||
			m_Peaks[m_PeakCount-1].Max  == -1 ||
			m_Peaks[m_PeakCount-1].RMin == -1
		)
		{
			m_PeakCount--;
		}
	}
	
	return true;
}

//Add cast function

/*
*AJF - 21/09/10
*Alternative to FindPeaks/FindValleys
*Walks along histogram and its gradients and looks for continous increases/decreases within the window resolution
*03/02/2010 - removed confusing first 
*/
void CHistogram::LocatePeaksValleys(int comparisonWindowSize)
{
	//TODO: if gradients havent been found with specific values call with deafult

	//int p=smoothingWindowSize/2;
	//double gradientAvg = 0, previousGradientAvg = 0;

	////Smooth @ smoothingWindowSize //TODO: Place in its own method

	//m_SmoothedGradients = new double[m_ValueCount];

	//memset(m_SmoothedGradients, 0, sizeof(double) * m_ValueCount);

	//for(int i=p; i<m_ValueCount-p; i++)
	//{
	//	gradientAvg=0;
	//	//Inspect window
	//	for(int j = i - p; j < (i + p); j++)
	//	{
	//		gradientAvg += m_Gradients[j];
	//	}
	//	gradientAvg /= smoothingWindowSize;

	//	m_SmoothedGradients[i] = gradientAvg;
	//}

	//Standardise to -1, 0, 1 TODO: place in own method or integrate elsewhere for efficiency

	int *m_StandardisedGradients = new int[m_ValueCount];

	memset(m_StandardisedGradients, 0, sizeof(int) * m_ValueCount);

	

	for(int i=0; i<m_ValueCount; i++)
	{
		if(m_Gradients[i] < 0)
			m_StandardisedGradients[i] = -1;
		else if (m_Gradients[i] > 0)
			m_StandardisedGradients[i] = 1;
		else
			m_StandardisedGradients[i] = 0;
	}

	int ascentCount=0, descentCount=0, uncertainCount=0;

	

	m_ValleyList.clear();
	m_PeakList.clear();
	
	//list <int> ascentList;
	//list <int> descentList;

	

	//Iterate though the standardised gradients
	for(int i=0; i < m_ValueCount; i+=comparisonWindowSize)
	{

		if((i+comparisonWindowSize) > m_ValueCount)
			break;

		int windowSum=0;
		//Inspect the window
		for(int w=i; w<i+comparisonWindowSize; w++)
		{
			windowSum += m_StandardisedGradients[w];
		}

		if(windowSum == comparisonWindowSize) //Clear ascent
		{
			//Add the start point
			//ascentList.push_back(i);

			if(descentCount > 0)
			{
				//We have just observed an ascent after a descent - indicating a valley - TODO: exactly where on the index???
				descentCount=0;
				m_ValleyList.push_back( i - ((comparisonWindowSize*uncertainCount)/2) );
			}
			
			uncertainCount=0;
			ascentCount++;
		}
		else if (windowSum == (comparisonWindowSize * -1)) //Clear descent
		{
			//descentList.push_back(i);

			if(ascentCount > 0)
			{
				//We have just observed a peak
				ascentCount=0;
				m_PeakList.push_back( i - ((comparisonWindowSize*uncertainCount)/2) );
			}
			
			uncertainCount=0;
			descentCount++;
		}
		else
		{
			////Horizontal or not resolvable
			//if(ascentCount>0)
			//{

			//}
			//else if(descentCount>0)
			//{

			//}

			//ascentCount=0;
			//descentCount=0;
			uncertainCount++;
		}
	}
}

list<int>* CHistogram::GetPeakList()
{
	return &m_PeakList;
}

list<int>* CHistogram::GetValleyList()
{
	return &m_ValleyList;
}

double* CHistogram::GetSmoothedGradients()
{
	//if(m_Gradients == NULL)
	//	CalculateGradients(); //If gradients haven't been created then create with default resolution

	return m_SmoothedGradients;
}

//Idea: locate closest peak to left/right of index or similar function


/*
*AJF - 20/09/10
*Calculates the gradient between two points
*Note: the indexes should be quiet close together otherwise information may be lost
*Note: generally we assume walking from left to right (simply invert output data values if you are coming the other way)
*Note: maybe make private?
*/

double CHistogram::CalculateGradient(int indexFrom, int indexTo)
{
	return (double)((m_Histogram[indexTo] - m_Histogram[indexFrom]) / (indexTo - indexFrom));
}

//double CHistogram::CalculateGradient(int index, int sampleResolution)
//{
//	
//}

/*
AJF - 20/09/10
*Calculates gradients for the whole histogram with the desired sample resolution
*Note: in its current state will not calc for bottom and top bounds of histogram ( indexes < sampleResolution/2 || indexes > histogram size - sampleResolution/2)
*/
void CHistogram::CalculateGradients(int sampleResolution)
{
	m_Gradients = new double[m_ValueCount];

	memset(m_Gradients, 0, sizeof(double) * m_ValueCount);

	int j = sampleResolution/2;

	for(int i=j; i<m_ValueCount - j; i++)
	{
		m_Gradients[i] = CalculateGradient(i - j, i+j);
	}
}

double* CHistogram::GetGradients()
{
	if(m_Gradients == NULL)
		CalculateGradients(); //If gradients haven't been created then create with default resolution

	return m_Gradients;
}

bool CHistogram::FindValleys()
{
	bool LastWasMax = false;
	int i;

	// Delete Previous Valleys
	delete [] m_Valleys;
	m_Valleys = NULL;
	m_Valleys = 0;
	
	for(i = 0; i < m_ValueCount; i++)
	{
		if(IsMax(i) && LastWasMax==false)
		{
			if(m_ValleyCount > 0)
				m_Valleys[m_ValleyCount-1].RMax = i;
			
			m_ValleyCount++;
			Valley * temp = new Valley[m_ValleyCount];
			if(temp==NULL)
			{
				delete [] m_Valleys;
				m_ValleyCount = 0;
				return false;
			}
			memcpy(temp,m_Valleys,sizeof(Valley) * (m_ValleyCount - 1));
			temp[m_ValleyCount-1].LMax =  i;
			temp[m_ValleyCount-1].Min  = -1;
			temp[m_ValleyCount-1].RMax = -1;
			delete [] m_Valleys;
			m_Valleys = temp;
			LastWasMax = true;
		}
		else if(IsMin(i) && LastWasMax==true && m_ValleyCount > 0)
		{
			m_Valleys[m_ValleyCount-1].Min = i;
			LastWasMax = false;
		}
	}

	if
	(
		m_Valleys[m_ValleyCount-1].LMax == -1 ||
		m_Valleys[m_ValleyCount-1].Min  == -1 ||
		m_Valleys[m_ValleyCount-1].RMax == -1
	)
	{
		m_ValleyCount--;
	}
	
	return true;
}

long CHistogram::GetAverageValue()
{
	int i;

	long Total = 0;

	for(i = 0; i < GetValueCount(); i++)
		Total += GetValue(i);

	return Total / i;
}

int CHistogram::GetBaseline(int Index)
{
	return m_BaseLines[Index];
}

int CHistogram::GetMaxIndex()
{
	int i;
	int MaxIndex = 0;
	int MaxValue = (m_ValueCount > 0 ? m_Histogram[0] : 0);

	for(i = 1; i < m_ValueCount; i++)
	{
		if(m_Histogram[i] > MaxValue)
		{
			MaxValue = m_Histogram[i];
			MaxIndex = i;
		}		
	}

	return MaxIndex;
}

int CHistogram::GetMaxVal()
{
	m_MaxVal = 0;

	for(int i = 0; i < m_ValueCount; i++)
	{
		if(m_Histogram[i] > m_MaxVal)
		{
			m_MaxVal = m_Histogram[i];
		}
	}

	return m_MaxVal;
}

CHistogram::Peak * CHistogram::GetMetaPeak(int Index)
{
	if(Index < 0 || Index >= m_MetaPeakCount)
	{
		return NULL;
	}
	
	return &(m_MetaPeaks[Index]);
}

int CHistogram::GetNoBaselines()
{
	return m_BaseLineCount;
}

int CHistogram::GetNoPeaks()
{
	return m_PeakCount;
}

int CHistogram::GetNoMetaPeaks()
{
	return m_MetaPeakCount;
}

int CHistogram::GetValueCount()
{
	return m_ValueCount;
}

CHistogram::Peak * CHistogram::GetPeak(int Index)
{
	if(Index < 0 || Index >= m_PeakCount)
	{
		return NULL;
	}
	
	return &(m_Peaks[Index]);
}

int CHistogram::GetValue(int Index)
{
	if(Index < m_ValueCount)
		return m_Histogram[Index];
	else
		return UINT_MAX;
}

bool CHistogram::Increment(int Index)
{
	if(Index >= m_ValueCount)
	{
		return false;
	}
	else
	{
		m_Histogram[Index] ++;
		return true;
	}
}

bool CHistogram::IsMax(const int Index)
{
	int i;
	
	i = Index;

	while(i > 0 && m_Histogram[i] == m_Histogram[Index])
		i--;

	if(m_Histogram[i] > m_Histogram[Index])
		return false;
	
	i = Index;

	while(i < m_ValueCount - 1 && m_Histogram[i] == m_Histogram[Index])
		i++;

	if(m_Histogram[i] > m_Histogram[Index])
		return false;
	
	return true;
}

bool CHistogram::IsMetaMaxPeaks(const int Index)
{
	int i;
	
	i = Index;
	
	while(i > 0 && m_Histogram[m_Peaks[i].Max] == m_Histogram[m_Peaks[Index].Max])
		i--;

	if(m_Histogram[m_Peaks[i].Max] > m_Histogram[m_Peaks[Index].Max])
		return false;
	
	i = Index;

	while(i < m_PeakCount - 1 && m_Histogram[m_Peaks[i].Max] == m_Histogram[m_Peaks[Index].Max])
		i++;

	if(m_Histogram[m_Peaks[i].Max] > m_Histogram[m_Peaks[Index].Max])
		return false;
	
	return true;
}

bool CHistogram::IsMetaMaxValleys(const int Index)
{
	int i;
	
	i = Index;
	
	while(i > 0 && m_Histogram[m_Valleys[i].Min] == m_Histogram[m_Valleys[Index].Min])
		i--;

	if(m_Histogram[m_Valleys[i].Min] > m_Histogram[m_Valleys[Index].Min])
		return false;
	
	i = Index;

	while(i < m_ValleyCount - 1 && m_Histogram[m_Valleys[i].Min] == m_Histogram[m_Valleys[Index].Min])
		i++;

	if(m_Histogram[m_Valleys[i].Min] > m_Histogram[m_Valleys[Index].Min])
		return false;
	
	return true;
}

bool CHistogram::IsMetaMinValleys(const int Index)
{
	int i;
	
	i = Index;

	while(i > 0 && m_Histogram[m_Valleys[i].Min] == m_Histogram[m_Valleys[Index].Min])
		i--;

	if(m_Histogram[m_Valleys[i].Min] < m_Histogram[m_Valleys[Index].Min])
		return false;
	
	i = Index;

	while(i < m_ValleyCount - 1 && m_Histogram[m_Valleys[i].Min] == m_Histogram[m_Valleys[Index].Min])
		i++;

	if(m_Histogram[m_Valleys[i].Min] < m_Histogram[m_Valleys[Index].Min])
		return false;
	
	return true;
}

bool CHistogram::IsMetaMinPeaks(const int Index)
{
	int i;
	
	i = Index;

	while(i > 0 && m_Histogram[m_Peaks[i].Max] == m_Histogram[m_Peaks[Index].Max])
		i--;

	if(m_Histogram[m_Peaks[i].Max] < m_Histogram[m_Peaks[Index].Max])
		return false;
	
	i = Index;

	while(i < m_PeakCount - 1 && m_Histogram[m_Peaks[i].Max] == m_Histogram[m_Peaks[Index].Max])
		i++;

	if(m_Histogram[m_Peaks[i].Max] < m_Histogram[m_Peaks[Index].Max])
		return false;
	
	return true;
}

bool CHistogram::IsMin(const int Index)
{
	int i;
	
	i = Index;

	while(i > 0 && m_Histogram[i] == m_Histogram[Index])
		i--;

	if(m_Histogram[i] < m_Histogram[Index])
		return false;
	
	i = Index;

	while(i < m_ValueCount - 1 && m_Histogram[i] == m_Histogram[Index])
		i++;

	if(m_Histogram[i] < m_Histogram[Index])
		return false;
	
	return true;
}

bool CHistogram::Resize(int NewNoValues)
{
	int * temp = new int[NewNoValues];
	if(temp == NULL)
		return false;

	memset(temp, 0, sizeof(int) * NewNoValues);
	
	if(NewNoValues >= m_ValueCount)
	{
		// New Histogram no smaller than previous so can copy all values
		memcpy(temp, m_Histogram, sizeof(int) * m_ValueCount);
	}
	else
	{
		// New Histogram is smaller than previous so can only copy enough 
		// values to fill new Histogram
		memcpy(temp, m_Histogram, sizeof(int) * NewNoValues);
	}
	m_ValueCount = NewNoValues;
	delete [] m_Histogram;
	m_Histogram = temp;
	m_ValueCount = NewNoValues;
	return true;
}

bool CHistogram::SetValue(int Index, int Value)
{
	if(Index < 0)
	{
		return false;
	}
	else if(Index >= m_ValueCount)
	{
		return false;
	}
	
	m_Histogram[Index] = Value;
	
	return true;
}

/*
*Class LocateRuns - deprecated - remove when confident with new implementation
*
*Finds consistant runs of values that are below the specified threshold in the histogram
*TODO: improve to allow additional thresholding <= > >= etc.
*AJF 30/09/10 - created
*/

//void CHistogram::LocateRuns(list<CValueRun>* runList, int valueThreshold/*Value to threshold below*/, int minRun/*Min length of a run*/, int maxRun/*Max length of a run*/, int maxNoiseDuration/*Max continous noise we can tolerate in a run*/)
//{
//		int runCounter=0, runStart=0;
//		int noiseCounter=0, noiseInRunCounter=0;
//		CValueRun run;
//
//		for(int i=0; i<m_ValueCount; i++) //Redo and functionise later
//		{
//			if(GetValue(i) <= valueThreshold)
//			{
//				if(runCounter < 1)
//				{
//					runStart = i;//Mark start of the run
//					noiseCounter=0;//Reset noise counters
//					noiseInRunCounter=0;
//				}
//				//TODO: checks we not at end
//				runCounter++;
//			}
//			else // > valueThreshold
//			{
//				if(runCounter > 0) //We are in a run and have some noise occuring
//				{
//					if(noiseCounter < maxNoiseDuration)
//					{
//						//We can tolerate the noise
//						noiseInRunCounter++;
//					}
//					else
//					{
//						//We can't tolerate the level of noise and must conclude the run
//						runCounter+=noiseInRunCounter;
//						if(runCounter < maxRun && runCounter > minRun)
//						{
//							//Run satisfies criteria
//							//Record run
//							run.SetStart(runStart);
//							run.SetFinish(runStart+runCounter);
//							runList->push_back(run);
//						}
//						//Reset counters
//						runCounter=0;
//						noiseCounter=-1; //The following increment will reset to zero
//						noiseInRunCounter=0;
//					}
//					noiseCounter++;
//				}
//			}
//
//			//End of loop catch
//			if(i == m_ValueCount-1)
//			{
//				//We have reached the end of histogram
//				//Are we in a run?
//				if(runCounter > 0)
//				{
//					if(runCounter < maxRun && runCounter > minRun)
//					{
//						//Run satisfies criteria
//						//Record run
//						run.SetStart(runStart);
//						run.SetFinish(runStart+runCounter);
//						runList->push_back(run);
//					}
//				}
//			}
//		}
//}

/*
*LocateRuns
*
*Finds consistant runs of values that are below or above the specified threshold in the histogram between a start and end index
*
*thresholdDirection: True - below or equal to threshold, False - above or equal to threshold
*
*Will walk from left to right if startIndex < endIndex and right to left if startIndex > endIndex
*
*AJF 30/09/10 - created
*AJF 06/10/10 - added new directional functionality
*/

void CHistogram::LocateRuns(list<CValueRun>* runList, int thresholdValue, bool thresholdDirection, int startIndex, int endIndex, int minRun, int maxRun, int maxNoiseDuration)
{
		int runCounter=0, runStart=0;
		int noiseCounter=0, noiseInRunCounter=0;
		CValueRun run;

		//TODO: Check for out of bounds
		if(endIndex==m_ValueCount)
			endIndex -= 1;

		if(startIndex==m_ValueCount)
			startIndex -= 1;

		if(startIndex < endIndex)
		{
			//Walking from left to right
			for(int i=startIndex; i<=endIndex; i++)
			{
				LocateRuns(i, thresholdValue, thresholdDirection, true, runList, run, runCounter, runStart, minRun, maxRun, noiseCounter, noiseInRunCounter, maxNoiseDuration);
				//Check if we are at end of histogram
				if(i == endIndex)
				{
					//We have reached the end of histogram
					//Are we in a run?
					if(runCounter > 0)
					{
						if(runCounter <= maxRun && runCounter >= minRun)
						{
							//Run satisfies criteria
							//Record run
							run.SetStart(runStart);
							run.SetFinish(runStart+runCounter);
							runList->push_back(run);
						}
					}
				}
			}
		}
		else
		{
			//Walking from right to left
			for(int i=startIndex; i>=endIndex; i--)
			{
				LocateRuns(i, thresholdValue, thresholdDirection, false, runList, run, runCounter, runStart, minRun, maxRun, noiseCounter, noiseInRunCounter, maxNoiseDuration);
				//Check if we are at begining of histogram
				if(i == startIndex)
				{
					//We have reached the begining of histogram
					//Are we in a run?
					if(runCounter > 0)
					{
						if(runCounter <= maxRun && runCounter >= minRun)
						{
							//Run satisfies criteria
							//Record run
							run.SetStart(runStart);
							run.SetFinish(runStart-runCounter);
							runList->push_back(run);
						}
					}
				}
			}
		}		
}

/*Private helper function for use in LocateRuns
*walkDirection true - left to right, false right to left
*AJF 06/10/10 - created
*/

void CHistogram::LocateRuns(int testIndex, int thresholdValue, bool thresholdDirection, bool walkDirection, list<CValueRun>* runList, CValueRun &run, int &runCounter, int &runStart, int &minRun, int &maxRun, int &noiseCounter, int &noiseInRunCounter, int &maxNoiseDuration)
{
	bool thresholdSatisfied=false;
	if(thresholdDirection)
	{
		if(GetValue(testIndex) <= thresholdValue)
			thresholdSatisfied=true;
	}
	else
	{
		if(GetValue(testIndex) >= thresholdValue)
			thresholdSatisfied=true;
	}
		
	if(thresholdSatisfied)
	{
		if(runCounter < 1)
		{
			runStart = testIndex;//Mark start of the run
			noiseCounter=0;//Reset noise counters
			noiseInRunCounter=0;
		}
		//TODO: checks we not at end
		runCounter++;
	}
	else // < || > valueThreshold - depending on thresholdDirection
	{
		if(runCounter > 0) //We are in a run and have some noise occuring
		{
			if(noiseCounter < maxNoiseDuration)
			{
				//We can tolerate the noise
				noiseInRunCounter++;
			}
			else
			{
				//We can't tolerate the level of noise and must conclude the run
				int noiseToCancel=0;

				//How much noise is at the end of the run?
				if(walkDirection)
				{
					//if(GetValue(testIndex) <= thresholdValue)
					//	thresholdSatisfied=true;
					
					//So we can 

					for(int i=testIndex; i>=testIndex-noiseInRunCounter; i--)
					{
						thresholdSatisfied=false;
						if(thresholdDirection)
						{
							if(GetValue(i) >= thresholdValue)
								thresholdSatisfied=true;
						}
						else
						{
							if(GetValue(i) <= thresholdValue)
								thresholdSatisfied=true;
						}

						if(thresholdSatisfied)
							noiseToCancel++;
					}
				}
				else
				{
					//if(GetValue(testIndex) > thresholdValue)
					//	thresholdSatisfied=true;
					for(int i=testIndex; i<=testIndex+noiseInRunCounter; i++)
					{
						thresholdSatisfied=false;
						if(thresholdDirection)
						{
							if(GetValue(i) >= thresholdValue)
								thresholdSatisfied=true;
						}
						else
						{
							if(GetValue(i) <= thresholdValue)
								thresholdSatisfied=true;
						}

						if(thresholdSatisfied)
							noiseToCancel++;
					}
				}

				//We should deduct this from the total noise

				noiseInRunCounter-=noiseToCancel;

				runCounter+=noiseInRunCounter;
				
				if(runCounter <= maxRun && runCounter >= minRun)
				{
					//Run satisfies criteria
					//Record run
					run.SetStart(runStart);
					
					if(!walkDirection)
						runCounter *= -1;

					run.SetFinish(runStart+runCounter);
					runList->push_back(run);
				}
				//Reset counters
				runCounter=0;
				noiseCounter=-1; //The following increment will reset to zero
				noiseInRunCounter=0;
			}
			noiseCounter++;
		}
	}
}