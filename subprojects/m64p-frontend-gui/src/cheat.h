#ifndef FRONTEND_CHEAT_H
#define FRONTEND_CHEAT_H

	#include "imports_m64p.h"

	#ifdef M64P_BIG_ENDIAN
		#define sl(mot) mot
	#else
		#define sl(mot) (((mot & 0xFF) << 24) | ((mot & 0xFF00) <<  8) | ((mot & 0xFF0000) >>  8) | ((mot & 0xFF000000) >> 24))
	#endif

	typedef enum {
		CHEAT_DISABLE,
		CHEAT_LIST,
		CHEAT_ALL,
		CHEAT_SHOW_LIST
	} eCheatMode;

	typedef struct {
		int    address;
		int   *variables;
		char **variable_names;
		int    var_to_use;
		int    var_count;
	} cheat_code;

	typedef struct _sCheatInfo {
		int                 Number;
		int                 Count;
		int                 VariableLine;
		const char         *Name;
		const char         *Description;
		cheat_code         *Codes;
		struct _sCheatInfo *Next;
		bool                active;
	} sCheatInfo;

	void ReadCheats(char *RomSection);
	void CheatFreeAll(void);
	sCheatInfo *CheatFindCode(int Number);
	void CheatActivate(sCheatInfo *pCheat);

	extern int                l_CheatCodesFound;
	extern int                l_RomFound;
	extern char              *l_CheatGameName;
	extern sCheatInfo        *l_CheatList;

#endif