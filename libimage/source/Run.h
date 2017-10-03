#ifndef RUN_H
#define RUN_H

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif


namespace PRImA 
{

class CConnectedComponent;

/*
 * Class CRun
 *
 * A simple horizontal run of pixels. used for connected components
 */
class DllExport CRun
{
	// CONSTRUCTION
public:
	CRun();
	~CRun();

	// METHODS
public:
	void Create(int y,int x1,int x2);
	int GetY();
	int GetX1();
	int GetX2();
	CConnectedComponent * GetCC();
	void SetCC(CConnectedComponent * CC);

	// DATA ITEMS
private:
	CConnectedComponent * m_pCC;
	int m_nY;
	int m_nX1;
	int m_nX2;
};

}

#endif // RUN_H
