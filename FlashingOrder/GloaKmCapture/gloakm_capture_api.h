#ifndef GLOAKM_CAPTURE_API_SDSF9045_H_
#define GLOAKM_CAPTURE_API_SDSF9045_H_

#include <windows.h>

#define DllExport __declspec(dllexport)

typedef int (*ReCallFunc)(void);

extern "C" DllExport BOOL WINAPI InstallLaunchEv(ReCallFunc func, char *tag_str);
extern "C" DllExport void WINAPI UnInstallLaunchEv();


#endif //GLOAKM_CAPTURE_API_SDSF9045_H_