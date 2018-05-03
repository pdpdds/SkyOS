// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SAMPLEDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SAMPLEDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SAMPLEDLL_EXPORTS
#define SAMPLEDLL_API __declspec(dllexport)
#else
#define SAMPLEDLL_API __declspec(dllimport)
#endif

// This class is exported from the SampleDll.dll
class SAMPLEDLL_API CSampleDll {
public:
	CSampleDll(void);
	// TODO: add your methods here.
};

extern SAMPLEDLL_API int nSampleDll;

SAMPLEDLL_API int fnSampleDll(void);
