#ifndef FRONTEND_PLUGIN_H
#define FRONTEND_PLUGIN_H

  	#include <limits.h>

	#include "osal/imports_osal.h"
	#include "imports_m64p.h"

	extern m64p_error PluginSearchLoad(m64p_handle ConfigUI);
	extern m64p_error PluginUnload(void);

	extern const char *g_PluginDir;
	extern const char *g_GfxPlugin;
	extern const char *g_AudioPlugin;
	extern const char *g_InputPlugin;
	extern const char *g_RspPlugin;

	typedef struct {
		m64p_plugin_type    type;
		char                name[8];
		m64p_dynlib_handle  handle;
		char                filename[PATH_MAX];
		const char         *libname;
		int                 libversion;
	} plugin_map_node;
	extern plugin_map_node g_PluginMap[4];

#endif