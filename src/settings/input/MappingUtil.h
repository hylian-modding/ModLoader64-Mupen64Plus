#pragma once

#include <string>
#include <SDL/SDL_scancode.h>

namespace MappingUtil
{

enum class MapType
{
	None, Key, JoyButton, JoyHat, JoyAxis
};

enum class JoyHatDir
{
	Right, Left, Down, Up
};

enum class JoyAxisDir
{
	Positive, Negative
};

struct KeyMap
{
	MapType type;
	SDL_Scancode scancode;
};

struct JoyButtonMap
{
	MapType type;
	int button;
};

struct JoyHatMap
{
	MapType type;
	JoyHatDir dir;
	int hat;
};

struct JoyAxisMap
{
	MapType type;
	JoyAxisDir dir;
	int axis;
	float deadzone;
};

union Map
{
	MapType type;
	KeyMap key;
	JoyButtonMap jbutton;
	JoyHatMap jhat;
	JoyAxisMap jaxis;
};

inline int deadzoneFromPercent(float v)
{
	return static_cast<int>(v / 100.0f * 32768.0f);
}

inline float deadzoneToPercent(int v)
{
	return v / 32768.0f * 100.0f;
}

Map parseSingleMap(const std::string& str);
std::string formatSingleMap(const Map& map, bool present);
std::pair<Map, Map> parseDoubleMap(const std::string& str);
std::string formatDoubleMap(const std::pair<Map, Map>& map);

}