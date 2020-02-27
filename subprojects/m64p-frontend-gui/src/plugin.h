#ifndef PLUGIN_H
#define PLUGIN_H

	#include <limits.h>
	
	#include "osal/imports_osal.h"
	#include "imports_m64p.h"

	m64p_error PluginSearchLoad(void);
	m64p_error PluginUnload(void);

	extern QString qtPluginDir;
	extern QString qtGfxPlugin;
	extern QString qtAudioPlugin;
	extern QString qtInputPlugin;
	extern QString qtRspPlugin;

	typedef struct {
		m64p_plugin_type    type;
		char                name[8];
		m64p_dynlib_handle  handle;
		std::string         filename;
		std::string         libname;
		int                 libversion;
	} plugin_map_node;
	extern plugin_map_node g_PluginMap[4];

#endif