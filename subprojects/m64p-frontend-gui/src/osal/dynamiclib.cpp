#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
    #include <windows.h>
#else
    #include <dlfcn.h>
    #include <string.h>
#endif

#include "imports_osal.h"

m64p_error osal_dynlib_open(m64p_dynlib_handle *pLibHandle, const char *pccLibraryPath) {
    if (pLibHandle == NULL || pccLibraryPath == NULL)
        return M64ERR_INPUT_ASSERT;

#ifdef WIN32
    *pLibHandle = LoadLibrary(pccLibraryPath);
#else
    *pLibHandle = dlopen(pccLibraryPath, RTLD_NOW);
#endif

    if (*pLibHandle == NULL) {        
#ifdef WIN32
        char *pchErrMsg;
        DWORD dwErr = GetLastError(); 
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErr,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &pchErrMsg, 0, NULL);
        printf("LoadLibrary('%s') error: %s", pccLibraryPath, pchErrMsg);
        LocalFree(pchErrMsg);
#else
        if (strchr(pccLibraryPath, '/') != NULL)
            printf("dlopen('%s') failed: %s", pccLibraryPath, dlerror());
#endif
        
        return M64ERR_INPUT_NOT_FOUND;
    }

    return M64ERR_SUCCESS;
}

void* osal_dynlib_getproc(m64p_dynlib_handle LibHandle, const char *pccProcedureName) {
    if (pccProcedureName == NULL)
        return NULL;

#ifdef WIN32
    return GetProcAddress(LibHandle, pccProcedureName);
#else
    return dlsym(LibHandle, pccProcedureName);
#endif
}

m64p_error osal_dynlib_close(m64p_dynlib_handle LibHandle) {
#ifdef WIN32
    if (FreeLibrary(LibHandle) == 0)
    {
        char *pchErrMsg;
        DWORD dwErr = GetLastError(); 
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErr,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &pchErrMsg, 0, NULL);
        printf("FreeLibrary() error: %s", pchErrMsg);
        LocalFree(pchErrMsg);
        return M64ERR_INTERNAL;
    }
#else
    if (dlclose(LibHandle) != 0)
    {
        printf("dlclose() failed: %s", dlerror());
        return M64ERR_INTERNAL;
    }
#endif

    return M64ERR_SUCCESS;
}