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

	void CheatStart(eCheatMode CheatMode, char *CheatNumList);

#endif