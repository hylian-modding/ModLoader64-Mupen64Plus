#ifndef FRONTEND_OSAL_FILES_H
#define FRONTEND_OSAL_FILES_H
	
	#include "preproc.h"

	typedef struct _osal_lib_search {
		char filepath[PATH_MAX];
		char* filename;
		m64p_plugin_type plugin_type;
		struct _osal_lib_search* next;
	} osal_lib_search;
		
	extern const int osal_libsearchdirs;
	extern const char* osal_libsearchpath[];

	extern osal_lib_search *osal_library_search(const char *searchpath);
	extern void osal_free_lib_list(osal_lib_search *head);

#endif