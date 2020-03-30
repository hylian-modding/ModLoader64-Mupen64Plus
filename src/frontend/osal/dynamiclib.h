#ifndef FRONTEND_OSAL_DYNAMICLIB_H
#define FRONTEND_OSAL_DYNAMICLIB_H

    #include <m64p_types.h>

    m64p_error osal_dynlib_open(m64p_dynlib_handle *pLibHandle, const char *pccLibraryPath);
    void* osal_dynlib_getproc(m64p_dynlib_handle LibHandle, const char *pccProcedureName);
    m64p_error osal_dynlib_close(m64p_dynlib_handle LibHandle);

#endif