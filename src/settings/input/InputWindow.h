#pragma once

#include <array>
#include <functional>
#include <vector>
#include <SDL/SDL_events.h>
#include <SDL/SDL_joystick.h>

#include "MappingUtil.h"

namespace Mupen
{
class Core;
}

class InputWindow final
{
public:
	InputWindow(const Mupen::Core& core) :
		m_core{ core }
	{}

	void handleEvent(SDL_Event& e);
	void show();

private:
	const Mupen::Core& m_core;

	int m_tabIndex{ -1 };
	int m_activeButtonIndex{ -1 };
	int m_activeMappingIndex{ -1 };
	int m_activeMappingSectionIndex{ -1 };
	bool m_scrollToActiveButton{};
	bool m_isDirty{};
	bool m_disableAxisMotion{};
	Uint32 m_ticksBetweenInputs{};

	std::vector<std::string> m_devices;
	int m_deviceIndex{};
	std::unique_ptr<SDL_Joystick, void(*)(SDL_Joystick*)> m_joystick{ nullptr, SDL_JoystickClose };
	std::vector<int> m_joystickInitialAxisStates;

	bool m_plugged{};
	int m_pluginIndex{};
	std::array<float, 2> m_analogDeadzones{};
	std::array<float, 2> m_analogPeaks{};
	constexpr static int NumMappings = 20;
	constexpr static int NumMappingSections = 2;
	std::array<std::array<MappingUtil::Map, NumMappings>, NumMappingSections> m_mappings{};

	void showTabBar();
	void showAxisWarningText();
	void moveToNextMappingButton();
	void deactivateMapping();
	void updateDeviceList();
	void openSelectedDevice();
	bool showMappingButton(int mappingIndex, int mappingSectionIndex, bool isActive, bool isWarning, bool showAxisDeadzone, const std::string& tooltipStart);
	bool showMappingButtonGroup(int beg, int num, std::function<bool(int, int, int, int)> showAsWarning = [](int, int, int, int) { return false; }, bool showAxisDeadzone = true);
	void loadConfig();
	void saveConfig();
};