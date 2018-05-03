// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SAMPLEDLL2_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SAMPLEDLL2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SAMPLEDLL2_EXPORTS
#define SAMPLEDLL2_API __declspec(dllexport)
#else
#define SAMPLEDLL2_API __declspec(dllimport)
#endif

// This class is exported from the SampleDll2.dll
class SAMPLEDLL2_API CSampleDll2 {
public:
	CSampleDll2(void);
	// TODO: add your methods here.
};

extern SAMPLEDLL2_API int nSampleDll2;

extern "C" SAMPLEDLL2_API int AAA(void);
