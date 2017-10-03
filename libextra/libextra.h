// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBEXTRA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBEXTRA_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LIBEXTRA_EXPORTS
#define LIBEXTRA_API __declspec(dllexport)
#else
#define LIBEXTRA_API __declspec(dllimport)
#endif

// This class is exported from the libextra.dll
class LIBEXTRA_API Clibextra {
public:
	Clibextra(void);
	// TODO: add your methods here.
};

extern LIBEXTRA_API int nlibextra;

LIBEXTRA_API int fnlibextra(void);
