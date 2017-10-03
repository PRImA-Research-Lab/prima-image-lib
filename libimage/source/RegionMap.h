#include <stdio.h>

#include "BiLevelImage.h"
#include "ConnectedComponents.h"
#include "Run.h"

namespace PRImA 
{

#ifndef REGIONMAP_H
#define REGIONMAP_H

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif


enum SmearDir
{
	SD_VERT,
	SD_HORZ,
	SD_UPRIGHT,
	SD_DOWNRIGHT
};

typedef unsigned char uint8_t;

/*
 * Class CLayoutObjectMap
 *
 * A bitonal bitmap with some operations such as smearing.
 *
 * CC 24/02/2016 - Renamed from CRegionMap
 */
class DllExport CLayoutObjectMap
{
	// CONSTRUCTION
public:
	CLayoutObjectMap();
	~CLayoutObjectMap();

	// METHODS
public:
	bool Create(const int Width, const int Height, const int OffsetX = 0, const int OffsetY = 0);
	COpenCvBiLevelImage * CreateImage();
	void Delete();
	//void ExportTIFF(char * FileName);
	int  GetHeight();
	bool GetValueDirect(const int x, const int y);
	bool GetValueOffset(const int x, const int y);
	int  GetWidth();
	void SetAll(const bool Value);
	void SetValueDirect(const int x, const int y, const bool Value);
	void SetValueOffset(const int x, const int y, const bool Value);
	void SetValueOffsetSave(const int x, const int y, const bool Value);
	void Smear(const int SmearVal, const SmearDir Dir, bool smearCol = true);
	void Smear(const int smearVal, CConnectedComponent * from, CConnectedComponent * to);

	void Dilate();

	inline int GetOffsetX() { return m_nOffsetX; };
	inline int GetOffsetY() { return m_nOffsetY; };

	CConnectedComponents * ExtractComponents();

	// DATA ITEMS
private:
	int m_nHeight;
	uint8_t ** m_pMap;
	int m_nWidth;
	int m_nOffsetX;
	int m_nOffsetY;
};

#else // REGIONMAP_H
class CLayoutObjectMap;
#endif // REGIONMAP_H

}