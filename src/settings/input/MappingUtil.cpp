#include <array>
#include <fmt/format.h>

#include "MappingUtil.h"
#include "sdl_key_converter.h"

namespace MappingUtil
{

static const std::array<std::string, 4> s_hatDirs{ "Right", "Left", "Down", "Up" };
static const std::array<char, 2> s_axisDirs{ '+', '-' };

bool starts_with(std::string mainStr, std::string toMatch)
{
	// Convert mainStr to lower case
	std::transform(mainStr.begin(), mainStr.end(), mainStr.begin(), ::tolower);
	// Convert toMatch to lower case
	std::transform(toMatch.begin(), toMatch.end(), toMatch.begin(), ::tolower);
 
	if(mainStr.find(toMatch) == 0)
		return true;
	else
		return false;
}

bool hatDirFromStr(const std::string& str, JoyHatDir* v)
{
	if (starts_with(str, "Right")) *v = JoyHatDir::Right;
	else if (starts_with(str, "Left")) *v = JoyHatDir::Left;
	else if (starts_with(str, "Down")) *v = JoyHatDir::Down;
	else if (starts_with(str, "Up")) *v = JoyHatDir::Up;
	else return false;
	return true;
}

bool axisDirFromChar(char c, JoyAxisDir* v)
{
	if (c == '+') *v = JoyAxisDir::Positive;
	else if (c == '-') *v = JoyAxisDir::Negative;
	else return false;
	return true;
}

Map parseSingleMap(const std::string& str)
{
	const char* cstr = str.c_str();

	std::array<int, 2> i;
	char c;
	std::string buf(20, '\0');

	Map map{};

	/*
		Clear needed whitespace
		
	*/

	if (starts_with(str, "key") && std::sscanf(cstr, "key(%d)", &i[0]) == 1) {
		map.type = MapType::Key;
		map.key.scancode = static_cast<SDL_Scancode>(sdl_keysym2native(i[0]));
	}
	else if (starts_with(str, "button") && std::sscanf(cstr, "button(%d)", &i[0]) == 1) {
		map.type = MapType::JoyButton;
		map.jbutton.button = i[0];
	}
	else if (starts_with(str, "hat") && std::sscanf(cstr, "hat(%d %10s", &i[0], buf.data()) == 2) {
		if (hatDirFromStr(buf, &map.jhat.dir)) {
			map.type = MapType::JoyHat;
			map.jhat.hat = i[0];
		}
	}
	else if (starts_with(str, "axis")) {
		if (std::sscanf(cstr, "axis(%d%c,%d)", &i[0], &c, &i[1]) == 3) {
			if (axisDirFromChar(c, &map.jaxis.dir)) {
				map.jaxis.type = MapType::JoyAxis;
				map.jaxis.axis = i[0];
				map.jaxis.deadzone = deadzoneToPercent(i[1]);
			}
		}
		else if (std::sscanf(cstr, "axis(%d%c)", &i[0], &c) == 2) {
			if (axisDirFromChar(c, &map.jaxis.dir)) {
				map.jaxis.type = MapType::JoyAxis;
				map.jaxis.axis = i[0];
				map.jaxis.deadzone = 0.0f;
			}
		}
	}

	return map;
}

std::string formatSingleMap(const Map& map, bool present)
{
	if (!present) {
		switch (map.type) {
		default: case MapType::None: return "";
		case MapType::Key: return fmt::format("key({})", 
			sdl_native2keysym(map.key.scancode));
		case MapType::JoyButton: return fmt::format("button({})", 
			map.jbutton.button);
		case MapType::JoyHat: return fmt::format("hat({} {})", 
			map.jhat.hat, s_hatDirs[static_cast<std::size_t>(map.jhat.dir)]);
		case MapType::JoyAxis: return map.jaxis.deadzone >= 0.01f ?
			fmt::format("axis({}{},{})", 
				map.jaxis.axis, s_axisDirs[static_cast<std::size_t>(map.jaxis.dir)], deadzoneFromPercent(map.jaxis.deadzone)) :
			fmt::format("axis({}{})", 
				map.jaxis.axis, s_axisDirs[static_cast<std::size_t>(map.jaxis.dir)]);
		}
	}
	else {
		switch (map.type) {
		default: case MapType::None: return "None";
		case MapType::Key: return SDL_GetKeyName(SDL_GetKeyFromScancode(map.key.scancode));
		case MapType::JoyButton: return fmt::format("Button {}", 
			map.jbutton.button);
		case MapType::JoyHat: return fmt::format("Hat {},{}", 
			map.jhat.hat, s_hatDirs[static_cast<std::size_t>(map.jhat.dir)]);
		case MapType::JoyAxis: return fmt::format("Axis {}{}", 
			map.jaxis.axis, s_axisDirs[static_cast<std::size_t>(map.jaxis.dir)]);
		}
	}
}

std::pair<Map, Map> parseDoubleMap(const std::string& str)
{
	const char* cstr = str.c_str();

	std::array<int, 2> i;
	std::array<char, 2> j;
	std::array<std::string, 2> buf;
	buf[0].resize(20);
	buf[1].resize(20);

	std::pair<Map, Map> map{};
	
	if (starts_with(str, "key") && std::sscanf(cstr, "key(%d,%d)", &i[0], &i[1]) == 2) {
		map.first.type = map.second.type = MapType::Key;
		map.first.key.scancode = static_cast<SDL_Scancode>(sdl_keysym2native(i[0]));
		map.second.key.scancode = static_cast<SDL_Scancode>(sdl_keysym2native(i[1]));
	}
	else if (starts_with(str, "button") && std::sscanf(cstr, "button(%d,%d)", &i[0], &i[1]) == 2) {
		map.first.type = map.second.type = MapType::JoyButton;
		map.first.jbutton.button = i[0];
		map.second.jbutton.button = i[1];
	}
	else if (starts_with(str, "hat") && std::sscanf(cstr, "hat(%d %10s %10s", &i[0], buf[0].data(), buf[1].data()) == 3) {
		if (hatDirFromStr(buf[0], &map.first.jhat.dir) && hatDirFromStr(buf[1], &map.second.jhat.dir)) {
			map.first.type = map.second.type = MapType::JoyHat;
			map.first.jhat.hat = map.second.jhat.hat = i[0];
		}
	}
	else if (starts_with(str, "axis") && std::sscanf(cstr, "axis(%d%c,%d%c)", &i[0], &j[0], &i[1], &j[1]) == 4) {
		if (axisDirFromChar(j[0], &map.first.jaxis.dir) && axisDirFromChar(j[1], &map.second.jaxis.dir)) {
			map.first.type = map.second.type = MapType::JoyAxis;
			map.first.jaxis.axis = i[0];
			map.second.jaxis.axis = i[1];
		}
	}

	return map;
}

std::string formatDoubleMap(const std::pair<Map, Map>& map)
{
	switch (map.first.type) {
	default: case MapType::None: return "";
	case MapType::Key:
		return fmt::format("key({},{})", 
			sdl_native2keysym(map.first.key.scancode), sdl_native2keysym(map.second.key.scancode));
	case MapType::JoyButton:
		return fmt::format("button({},{})", 
			map.first.jbutton.button, map.second.jbutton.button);
	case MapType::JoyHat:
		return fmt::format("hat({} {} {})", 
			map.first.jhat.hat, s_hatDirs[static_cast<std::size_t>(map.first.jhat.dir)], s_hatDirs[static_cast<std::size_t>(map.second.jhat.dir)]);
	case MapType::JoyAxis:
		return fmt::format("axis({}{},{}{})", 
			map.first.jaxis.axis, s_axisDirs[static_cast<std::size_t>(map.first.jaxis.dir)], 
			map.second.jaxis.axis, s_axisDirs[static_cast<std::size_t>(map.second.jaxis.dir)]);
	}
}

}
