#pragma once

#ifndef HEXWINDOW_H
#define HEXWINDOW_H

#include <stdio.h>
#include <SDL/SDL_events.h>
#include "../../imgui/ImGuiContextWrapper.h"

class HexWindow final
{
public:
	HexWindow();
	void runOnce();
	void readData();

	struct{
		uint8_t* hexData;
		uint8_t* funcData;
	} data;

	struct
	{
		uint32_t hex = 0;
		uint32_t func = 0;
	} currentAddr;

	struct
	{
		uint32_t columns = 16;
		uint32_t rows = 16;
		uint32_t func = 512;
	} size;

	inline const uint32_t getSize()
	{
		return size.columns * size.rows;
	}
};

#endif



