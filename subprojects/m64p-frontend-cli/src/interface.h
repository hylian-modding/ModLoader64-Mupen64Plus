#ifndef FRONTEND_INTERFACE_H
#define FRONTEND_INTERFACE_H
    
    #ifdef WIN32
        #ifdef M64P_FRONTEND
            #define expose __declspec(dllexport)
        #else
            #define  expose __declspec(dllimport)
        #endif
    #else
        #define expose
    #endif

    #include "imports_m64p.h"
    
    extern int g_CoreCapabilities;
    extern int g_CoreAPIVersion;

    extern m64p_error AttachCoreLib(const char *CoreLibFilepath);
    extern m64p_error DetachCoreLib(void);

    extern m64p_dynlib_handle      CoreHandle;

    expose extern ptr_CoreErrorMessage    CoreErrorMessage;

    expose extern ptr_CoreStartup         CoreStartup;
    expose extern ptr_CoreShutdown        CoreShutdown;
    expose extern ptr_CoreAttachPlugin    CoreAttachPlugin;
    expose extern ptr_CoreDetachPlugin    CoreDetachPlugin;
    expose extern ptr_CoreDoCommand       CoreDoCommand;
    expose extern ptr_CoreOverrideVidExt  CoreOverrideVidExt;
    expose extern ptr_CoreAddCheat        CoreAddCheat;
    expose extern ptr_CoreCheatEnabled    CoreCheatEnabled;

    expose extern ptr_ConfigListSections     ConfigListSections;
    expose extern ptr_ConfigOpenSection      ConfigOpenSection;
    expose extern ptr_ConfigDeleteSection    ConfigDeleteSection;
    expose extern ptr_ConfigSaveSection      ConfigSaveSection;
    expose extern ptr_ConfigListParameters   ConfigListParameters;
    expose extern ptr_ConfigSaveFile         ConfigSaveFile;
    expose extern ptr_ConfigSetParameter     ConfigSetParameter;
    expose extern ptr_ConfigGetParameter     ConfigGetParameter;
    expose extern ptr_ConfigGetParameterType ConfigGetParameterType;
    expose extern ptr_ConfigGetParameterHelp ConfigGetParameterHelp;
    expose extern ptr_ConfigSetDefaultInt    ConfigSetDefaultInt;
    expose extern ptr_ConfigSetDefaultFloat  ConfigSetDefaultFloat;
    expose extern ptr_ConfigSetDefaultBool   ConfigSetDefaultBool;
    expose extern ptr_ConfigSetDefaultString ConfigSetDefaultString;
    expose extern ptr_ConfigGetParamInt      ConfigGetParamInt;
    expose extern ptr_ConfigGetParamFloat    ConfigGetParamFloat;
    expose extern ptr_ConfigGetParamBool     ConfigGetParamBool;
    expose extern ptr_ConfigGetParamString   ConfigGetParamString;

    expose extern ptr_ConfigExternalOpen         ConfigExternalOpen;
    expose extern ptr_ConfigExternalClose        ConfigExternalClose;
    expose extern ptr_ConfigExternalGetParameter ConfigExternalGetParameter;
    expose extern ptr_ConfigHasUnsavedChanges    ConfigHasUnsavedChanges;

    expose extern ptr_ConfigGetSharedDataFilepath ConfigGetSharedDataFilepath;
    expose extern ptr_ConfigGetUserConfigPath     ConfigGetUserConfigPath;
    expose extern ptr_ConfigGetUserDataPath       ConfigGetUserDataPath;
    expose extern ptr_ConfigGetUserCachePath      ConfigGetUserCachePath;

    expose extern ptr_DebugSetCallbacks      DebugSetCallbacks;
    expose extern ptr_DebugSetCoreCompare    DebugSetCoreCompare;
    expose extern ptr_DebugSetRunState       DebugSetRunState;
    expose extern ptr_DebugGetState          DebugGetState;
    expose extern ptr_DebugStep              DebugStep;
    expose extern ptr_DebugDecodeOp          DebugDecodeOp;
    expose extern ptr_DebugMemGetRecompInfo  DebugMemGetRecompInfo;
    expose extern ptr_DebugMemGetMemInfo     DebugMemGetMemInfo;
    expose extern ptr_DebugMemGetPointer     DebugMemGetPointer;

    expose extern ptr_DebugMemRead64         DebugMemRead64;
    expose extern ptr_DebugMemRead32         DebugMemRead32;
    expose extern ptr_DebugMemRead16         DebugMemRead16;
    expose extern ptr_DebugMemRead8          DebugMemRead8;

    expose extern ptr_DebugMemWrite64        DebugMemWrite64;
    expose extern ptr_DebugMemWrite32        DebugMemWrite32;
    expose extern ptr_DebugMemWrite16        DebugMemWrite16;
    expose extern ptr_DebugMemWrite8         DebugMemWrite8;

    expose extern ptr_DebugGetCPUDataPtr     DebugGetCPUDataPtr;
    expose extern ptr_DebugBreakpointLookup  DebugBreakpointLookup;
    expose extern ptr_DebugBreakpointCommand DebugBreakpointCommand;

    expose extern ptr_DebugBreakpointTriggeredBy DebugBreakpointTriggeredBy;
    expose extern ptr_DebugVirtualToPhysical     DebugVirtualToPhysical;

    expose extern ptr_read_rdram_buffer		 RdRamReadBuffer;
    expose extern ptr_read_rdram_32			 RdRamRead32;
    expose extern ptr_read_rdram_16			 RdRamRead16;
    expose extern ptr_read_rdram_8				 RdRamRead8;

    expose extern ptr_write_rdram_32			 RdRamWrite32;
    expose extern ptr_write_rdram_16			 RdRamWrite16;
    expose extern ptr_write_rdram_8			 RdRamWrite8;

    expose extern ptr_read_rom_buffer			 RomReadBuffer;
    expose extern ptr_read_rom_32				 RomRead32;
    expose extern ptr_read_rom_16				 RomRead16;
    expose extern ptr_read_rom_8				 RomRead8;

    expose extern ptr_write_rom_32				 RomWrite32;
    expose extern ptr_write_rom_16				 RomWrite16;
    expose extern ptr_write_rom_8				 RomWrite8;

    expose extern ptr_get_cart_clock_rate		 CartGetClockRate;
    expose extern ptr_get_cart_pc				 CartGetPC;
    expose extern ptr_get_cart_release		     CartGetRelease;

    expose extern ptr_get_cart_name			 CartGetName;
    expose extern ptr_get_cart_manufacturer_id  CartGetManufacturerID;
    expose extern ptr_get_cart_serial			 CartGetSerial;
    expose extern ptr_get_cart_country_code	 CartGetCountryCode;

    expose extern ptr_refresh_memory_cache		 MemoryCacheRefresh;

    expose extern ptr_CoreSaveOverride          CoreSaveOverride;
    expose extern ptr_IsMupenReady              IsMupenReady;

#endif