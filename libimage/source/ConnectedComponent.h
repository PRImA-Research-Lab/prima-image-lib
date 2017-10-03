#ifndef CONNECTEDCOMPONENT_H
#define CONNECTEDCOMPONENT_H

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif


namespace PRImA 
{

class CRun;


/*
 * Class CConnectedComponent
 *
 * Represents a single connected component within a bitonal image.
 * The component is stored in a data structure containing pixel runs. 
 */
class DllExport CConnectedComponent
{
	// CONSTRUCTION
public:
	CConnectedComponent();
	~CConnectedComponent();

	// METHODS
public:
	bool AddRun(CRun * Run, bool updateBoundingBox = true);
	int  GetArea();
	bool     Create(CRun * Run);
	int      GetCentroidX();
	int      GetCentroidY();
	int		 GetCenterX();
	int		 GetCenterY();
	CConnectedComponent * GetNeighbour(int Index);
	int		 GetRunCount();
	CRun   * GetRun(int Index);
	bool     IsIn(int x, int y);
	bool	 IsInsideRect(int x1, int y1, int x2, int y2);
	bool	 IsTouchingRect(int x1, int y1, int x2, int y2);
	bool     Merge(CConnectedComponent * CC);
	int      GetHeight();
	int		 GetNeighbourCount();
	int      GetWidth();
	unsigned GetSize();
	int      GetX1();
	int      GetX2();
	int      GetY1();
	int      GetY2();
	bool     SetNeighbour(const int Index, CConnectedComponent * Neighbour);
	bool     SetNoNeighbours(const int NoNeighbours);
	bool     SetNeighbourCount(const int count);
	bool     SetNoRuns(const int number);
	bool     SetRunCount(const int count);
	
	CConnectedComponent * Clone();

	// DATA ITEMS
private:
	int m_RunCount;
	int m_RunsAlloc;
	CRun ** m_Runs;

	int     m_nNeighbours;
	CConnectedComponent ** m_pNeighbours;

	int m_X1;
	int m_X2;
	int m_Y1;
	int m_Y2;

	static const int RUN_INIT = 100;
	static const int RUN_INCR = 100;
};

}

#endif // CONNECTEDCOMPONENT_H
