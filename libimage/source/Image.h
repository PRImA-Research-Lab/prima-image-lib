#ifndef IMAGE_H
#define IMAGE_H

#ifdef _MSC_VER
#include "afxwin.h"
#include "extrastring.h"
//#include <pstdint.h>
#include <cstdint>
#else
#include <stdint.h>
#endif

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif


typedef struct tagImageInfo
{
	short int Type;
	double RangeLeft;
	double RangeRight;
} ImageInfo;

struct RGBCOLOUR
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
	bool operator==(RGBCOLOUR & cmp)
	{
		return (R == cmp.R && G == cmp.G && B == cmp.B && A == cmp.A);
	};
};

const RGBCOLOUR RGBBLACK     = {  0,   0,   0, 255};
const RGBCOLOUR RGBWHITE     = {255, 255, 255, 255};
const RGBCOLOUR RGBRED       = {255,   0,   0, 255};
const RGBCOLOUR RGBGREEN     = {  0, 255,   0, 255};
const RGBCOLOUR RGBBLUE      = {  0,   0, 255, 255};
const RGBCOLOUR RGBYELLOW    = {255, 255,   0, 255};
const RGBCOLOUR RGBORANGE    = {255, 128,   0, 255};
const RGBCOLOUR RGBPINK      = {255, 128, 255, 255};
const RGBCOLOUR RGBGREY      = {128, 128, 128, 255};
const RGBCOLOUR RGBGREY32    = { 32,  32,  32, 255};
const RGBCOLOUR RGBGREY64    = { 64,  64,  64, 255};
const RGBCOLOUR RGBGREY96    = { 96,  96,  96, 255};
const RGBCOLOUR RGBGREY128   = {128, 128, 128, 255};
const RGBCOLOUR RGBGREY160   = {160, 160, 160, 255};
const RGBCOLOUR RGBGREY192   = {192, 192, 192, 255};
const RGBCOLOUR RGBGREY224   = {224, 224, 224, 255};
const RGBCOLOUR RGBTURQUOISE = {  0, 255, 192, 255};
const RGBCOLOUR RGBINDIGO    = { 75,   0, 130, 255};
const RGBCOLOUR RGBVIOLET    = {238, 130, 238, 255};
const RGBCOLOUR RGBCYAN      = {  0, 255, 255, 255};
const RGBCOLOUR RGBMAGENTA   = {255,   0, 255, 255};

namespace PRImA
{

/*
 * Class CImage
 *
 * Base class for PRImA image classes.
 *
 * Deprecated: Use OpenCV image classes.
 */
class DllExport CImage
{
public:
	static const int ERR_NONE			= 0;
	static const int ERR_UNKNOWN		= 1;
	static const int ERR_OUT_OF_MEMORY	= 2;

	// CONSTRUCTION
public:
	CImage();
	virtual ~CImage();

	// METHODS
public:
	void		 Init();
	virtual bool Create(int Width, int Height, RGBCOLOUR BackColour, const char * Name, bool reverse = false);
	CImage*		 CreateSubImage(int left, int top, int width, int height); //Creates an image of the same type copying the specified frame.
	virtual	CImage * CreateNewImage() = 0; //Factory method to create an empty image
	//virtual bool DetectBaselineDist(int NoStrips = 1);
	virtual void Frame(int FrameWidth, bool White);
	virtual int  GetBaselineDist();
	virtual CImage * Clone(bool reverse = false) = 0; //Creates a deep copy of this image

#ifdef _MSC_VER
	HBITMAP CreateBitmap();
	//inline CBitmap * GetBitmap() { if (m_ImageBitmap == NULL) return CreateBitmap(); else return m_ImageBitmap; };
#endif
	inline RGBCOLOUR GetBackColour() { return m_BackColour; };
	inline unsigned GetBitsPerPixel() { return m_BitsPerPixel; };
	static double	GetColourDistance(RGBCOLOUR a, RGBCOLOUR b);
	inline int		GetHeight() { return m_Height; };
	void			GetICCProfile(uint32_t * ICCLength, uint8_t ** ICCProfile);
	virtual RGBCOLOUR GetRGBColor(int x, int y)
	{
		return (x ==y ? RGBWHITE : RGBWHITE);
	};
	virtual void FillBitmapInfo()
	{
		return;
	};
	virtual void	SetRGBColor(int, int, RGBCOLOUR){return;};
	inline char *	GetName() { return m_sName; };
	void			SetName(const char * name);
	inline int		GetWidth() { return m_Width;};
	inline void		SetSize(unsigned width, unsigned height) 
	{
		m_Width = width;
		m_Height = height;
	}
	inline double	GetXRes() { return m_ResX; };
	inline double	GetYRes() { return m_ResY; };
	void inline		SetBitsPerPixel(unsigned bpp) { m_BitsPerPixel = bpp; };
	void			SetICCProfile(uint32_t ICCLength, uint8_t * ICCProfile);
	inline void		SetResolution(const double XRes, const double YRes) 
	{	
		m_ResX = XRes;
		m_ResY = YRes;
	};
	inline void SetResolutionUnit(int resUnit) { m_ResUnit = resUnit; };
	inline void		SetBytesBerLine(long bytes) { m_BytesPerLine = bytes; };
	void InitArrays();
	inline long			GetBytesPerLine() { return m_BytesPerLine; };
	inline uint8_t**	GetLineArray() { return m_pLineArray; };
	inline void			SetLineArray(uint8_t** lineArray) { m_pLineArray = lineArray; };
	inline uint8_t*		GetArray() { return m_pArray; };
	inline void			SetArray(uint8_t* pArray) { m_pArray = pArray;};
	inline uint32_t		GetIccLength() { return m_nICCLength; };
	inline void			SetIccLength(uint32_t iccLength) { m_nICCLength = iccLength; };
	//TODO CC: There already is a method called GetICCProfile! What does it do and where is it used? Maybe it should be renamed.
	inline uint8_t*		GetIccProfile() { return m_sICCProfile; };
	inline void			SetIccProfile(uint8_t* iccProfile) { m_sICCProfile = iccProfile; };

	inline HBITMAP		GetHBitmap() { if (m_ImageHBitmap==NULL) return CreateBitmap(); else return m_ImageHBitmap; };
	inline void			ResetHBitmap() { DeleteObject(m_ImageHBitmap); m_ImageHBitmap = 0; };
	inline CUniString	GetFilePath() { return m_FilePath; };
	inline void			SetFilePath(CUniString path) { m_FilePath = path; };

	virtual void		Clear(RGBCOLOUR colour) = 0;

	// DATA
private:
	ImageInfo  m_Info;
	long       m_NumberOfPixels;
	int        m_ResUnit;
protected:
	uint8_t  * m_pArray;
	RGBCOLOUR  m_BackColour;
	int        m_BaselineDist;
	uint16_t   m_BitsPerPixel;
	long       m_BytesPerLine;
	RGBCOLOUR  m_nForeColour;
	int		   m_Height;
	uint32_t   m_nICCLength;
	uint8_t  * m_sICCProfile;
	uint8_t ** m_pLineArray; //Helper for faster access of lines
	char     * m_sName;
	double     m_ResX;
	double     m_ResY;
	int		   m_Width;
	CUniString	m_FilePath;
#ifdef _MSC_VER
	//CBitmap *	 m_ImageBitmap;
	BITMAPINFO * m_pBitmapInfo;
	HBITMAP      m_ImageHBitmap;
#endif
};

} //end namespace PRImA

#endif
