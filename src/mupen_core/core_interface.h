/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus-ui-console - core_interface.h                             *
 *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
 *   Copyright (C) 2009 Richard Goedeken                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if !defined(CORE_INTERFACE_H)
#define CORE_INTERFACE_H

#include <m64p_common.h>
#include <m64p_config.h>
#include <m64p_debugger.h>
#include <m64p_frontend.h>
#include <m64p_types.h>
#include <m64p_memaccess.h>

/* function declarations */
extern m64p_error AttachCoreLib(const char *CoreLibFilepath);
extern m64p_error DetachCoreLib(void);

/* global variables from core_interface.c */
extern int g_CoreCapabilities;
extern int g_CoreAPIVersion;

/* declarations of Core library handle and pointers to common functions */
extern m64p_dynlib_handle      CoreHandle;
extern ptr_CoreErrorMessage    CoreErrorMessage;

/* declarations of pointers to Core rdram functions */
extern ptr_read_rdram_buffer			RdRamReadBuffer;
extern ptr_read_rdram_32				RdRamRead32;
extern ptr_read_rdram_16				RdRamRead16;
extern ptr_read_rdram_8					RdRamRead8;

extern ptr_write_rdram_32				RdRamWrite32;
extern ptr_write_rdram_16				RdRamWrite16;
extern ptr_write_rdram_8				RdRamWrite8;

/* declarations of pointers to Core Rom functions */
extern ptr_read_rom_buffer			RomReadBuffer;
extern ptr_read_rom_32				RomRead32;
extern ptr_read_rom_16				RomRead16;
extern ptr_read_rom_8				RomRead8;

extern ptr_write_rom_32				RomWrite32;
extern ptr_write_rom_16				RomWrite16;
extern ptr_write_rom_8				RomWrite8;

/* declarations of pointers to Core Cart functions */
extern ptr_get_cart_clock_rate			CartGetClockRate;
extern ptr_get_cart_pc					CartGetPC;
extern ptr_get_cart_release		    	CartGetRelease;

extern ptr_get_cart_name				CartGetName;
extern ptr_get_cart_manufacturer_id 	CartGetManufacturerID;
extern ptr_get_cart_serial				CartGetSerial;
extern ptr_get_cart_country_code		CartGetCountryCode;

/* declarations of pointers to Core SaveState functions */
extern ptr_refresh_memory_cache		MemoryCacheRefresh;

/* declarations of pointers to Core front-end functions */
extern ptr_CoreStartup         CoreStartup;
extern ptr_CoreShutdown        CoreShutdown;
extern ptr_CoreAttachPlugin    CoreAttachPlugin;
extern ptr_CoreDetachPlugin    CoreDetachPlugin;
extern ptr_CoreDoCommand       CoreDoCommand;
extern ptr_CoreOverrideVidExt  CoreOverrideVidExt;
extern ptr_CoreSaveOverride    CoreSaveOverride;
extern ptr_IsMupenReady        IsMupenReady;

/* declarations of pointers to Core config functions */
extern ptr_ConfigListSections     ConfigListSections;
extern ptr_ConfigOpenSection      ConfigOpenSection;
extern ptr_ConfigDeleteSection    ConfigDeleteSection;
extern ptr_ConfigSaveSection      ConfigSaveSection;
extern ptr_ConfigListParameters   ConfigListParameters;
extern ptr_ConfigSaveFile         ConfigSaveFile;
extern ptr_ConfigSetParameter     ConfigSetParameter;
extern ptr_ConfigGetParameter     ConfigGetParameter;
extern ptr_ConfigGetParameterType ConfigGetParameterType;
extern ptr_ConfigGetParameterHelp ConfigGetParameterHelp;
extern ptr_ConfigSetDefaultInt    ConfigSetDefaultInt;
extern ptr_ConfigSetDefaultFloat  ConfigSetDefaultFloat;
extern ptr_ConfigSetDefaultBool   ConfigSetDefaultBool;
extern ptr_ConfigSetDefaultString ConfigSetDefaultString;
extern ptr_ConfigGetParamInt      ConfigGetParamInt;
extern ptr_ConfigGetParamFloat    ConfigGetParamFloat;
extern ptr_ConfigGetParamBool     ConfigGetParamBool;
extern ptr_ConfigGetParamString   ConfigGetParamString;

extern ptr_ConfigGetSharedDataFilepath ConfigGetSharedDataFilepath;
extern ptr_ConfigGetUserConfigPath     ConfigGetUserConfigPath;
extern ptr_ConfigGetUserDataPath       ConfigGetUserDataPath;
extern ptr_ConfigGetUserCachePath      ConfigGetUserCachePath;

/* declarations of pointers to Core debugger functions */
extern ptr_DebugSetCallbacks      DebugSetCallbacks;
extern ptr_DebugSetCoreCompare    DebugSetCoreCompare;
extern ptr_DebugSetRunState       DebugSetRunState;
extern ptr_DebugGetState          DebugGetState;
extern ptr_DebugStep              DebugStep;
extern ptr_DebugDecodeOp          DebugDecodeOp;
extern ptr_DebugMemGetRecompInfo  DebugMemGetRecompInfo;
extern ptr_DebugMemGetMemInfo     DebugMemGetMemInfo;
extern ptr_DebugMemGetPointer     DebugMemGetPointer;

extern ptr_DebugMemRead64         DebugMemRead64;
extern ptr_DebugMemRead32         DebugMemRead32;
extern ptr_DebugMemRead16         DebugMemRead16;
extern ptr_DebugMemRead8          DebugMemRead8;

extern ptr_DebugMemWrite64        DebugMemWrite64;
extern ptr_DebugMemWrite32        DebugMemWrite32;
extern ptr_DebugMemWrite16        DebugMemWrite16;
extern ptr_DebugMemWrite8         DebugMemWrite8;

extern ptr_DebugGetCPUDataPtr     DebugGetCPUDataPtr;
extern ptr_DebugBreakpointLookup  DebugBreakpointLookup;
extern ptr_DebugBreakpointCommand DebugBreakpointCommand;

#endif /* #define CORE_INTERFACE_H */
