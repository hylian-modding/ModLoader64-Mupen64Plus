#ifndef FRONTEND_OSAL_PREPROC_H
#define FRONTEND_OSAL_PREPROC_H

	#if defined(WIN32)
		#include <windows.h>

		#define PATH_MAX 2048
		#define OSAL_DEFAULT_DYNLIB_FILENAME "mupen64plus.dll"
		#define OSAL_DIR_SEPARATOR           '\\'
		#define OSAL_CURRENT_DIR             ".\\"
		#define OSAL_EMULATOR_DIR             ".\\emulator"
		#define OSAL_DLL_EXTENSION           ".dll"
		#define osal_insensitive_strcmp(x, y) _stricmp(x, y)
	#elif defined(__APPLE__)
		#include <limits.h>

		#define OSAL_DEFAULT_DYNLIB_FILENAME "mupen64plus.dylib"
		#define OSAL_DIR_SEPARATOR           '/'
		#define OSAL_CURRENT_DIR             "./"
		#define OSAL_EMULATOR_DIR             "./emulator"
		#define OSAL_DLL_EXTENSION           ".dylib"
		#define osal_insensitive_strcmp(x, y) strcasecmp(x, y)
	#else
		#include <limits.h>
		
		#ifndef PATH_MAX
		#define PATH_MAX 4096
		#endif

		#if defined(ANDROID)
		#include <android/log.h>

		#define printf(...) __android_log_print(ANDROID_LOG_VERBOSE, "Binding", __VA_ARGS__)
		#endif

		#define OSAL_DEFAULT_DYNLIB_FILENAME "mupen64plus.so"
		#define OSAL_DIR_SEPARATOR           '/'
		#define OSAL_CURRENT_DIR             "./"
		#define OSAL_EMULATOR_DIR             "./emulator"
		#define OSAL_DLL_EXTENSION           ".so"
		#define osal_insensitive_strcmp(x, y) strcasecmp(x, y)
	#endif
  
#endif