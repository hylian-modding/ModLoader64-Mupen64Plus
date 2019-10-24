#include <sstream>
#include <fmt/format.h>
#include <SDL/SDL_timer.h>
#include <mystd_clamp.h>

#include "InputWindow.h"
#include "../../imgui/ImGuiUtil.h"
#include "Mupen.h"

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

std::vector<std::string> split(std::string str, std::string token){
    std::vector<std::string>result;
    while(str.size()){
        int index = str.find(token);
        if(index!=std::string::npos){
            result.push_back(str.substr(0,index));
            str = str.substr(index+token.size());
            if(str.size()==0)result.push_back(str);
        }else{
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

static std::array<std::string, 3> s_pluginNames{ "None", "Memory pak", "Rumble pak" };
static std::array<std::string, 20> s_mappingLabels{
	"X Axis Left", "X Axis Right",
	"Y Axis Up", "Y Axis Down",
	"DPad Left", "DPad Right", "DPad Up", "DPad Down",
	"Start", "Z Trigger", "B Button", "A Button",
	"C Button Left", "C Button Right", "C Button Up", "C Button Down",
	"L Trigger", "R Trigger", 
	"Mempak switch", "Rumblepak switch" 
};
static std::array<std::string, 16> s_mappingProperties{
	"DPad L", "DPad R", "DPad U", "DPad D",
	"Start", "Z Trig", "B Button", "A Button",
	"C Button L", "C Button R", "C Button U", "C Button D",
	"L Trig", "R Trig", 
	"Mempak switch", "Rumblepak switch"
};
static std::array<std::string, 2> s_mapping2Properties{ "X Axis", "Y Axis" };
static std::array<std::string, 2> s_mappingSectionLabels{ "Primary", "Secondary" };
static constexpr Uint32 s_numTicksBetweenInputs = 250;

void InputWindow::show()
{
	ImGui::SetNextWindowPos({});

	ImGui::Begin("##inputWindow", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground 
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::InvisibleButton("##inputWindowLine", { 459, 1 });

	ImGui::Checkbox("Disable device axis motion", &m_disableAxisMotion);
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Useful to map a button instead of an axis\nwhen both are triggered at the same time\n(e.g. ps4 or ngc controllers shoulders).\n\nThis has no effect on the emulated\ncontroller, it is only a helper.");

	showTabBar();

	if (m_tabIndex < 4) {
		if (ImGui::Checkbox("Plugged", &m_plugged)) {
			m_isDirty = true;
		}

		if (ImGuiUtil::Combo("Device", &m_deviceIndex, m_devices.begin(), m_devices.end(), 10)) {
			openSelectedDevice();
			m_isDirty = true;
		}
		if (ImGuiUtil::Combo("Plugin", &m_pluginIndex, s_pluginNames.begin(), s_pluginNames.end())) {
			m_isDirty = true;
		}

		ImGui::Separator();

		if (ImGui::InputFloat2("Analog deadzone", m_analogDeadzones.data(), "%.2f%%")) {
			for (auto& i : m_analogDeadzones) i = mystd::clamp(i, 0.0f, 100.0f);
			m_isDirty = true;
		}

		if (ImGui::InputFloat2("Analog peak", m_analogPeaks.data(), "%.2f%%")) {
			for (auto& i : m_analogPeaks) i = mystd::clamp(i, 0.0f, 100.0f);
			m_isDirty = true;
		}

		auto showAxisWarning = [this](int beg, int num, int i, int j)
		{
			if (i == beg || i == beg + 1) {
				const auto& map1 = m_mappings[j][i];
				const auto& map2 = m_mappings[j][i == beg ? i + 1 : i - 1];
				const bool sameType = map1.type == map2.type;
				return !sameType;
			}
			return false;
		};

		ImGui::Separator();
		if (showMappingButtonGroup(0, 2, showAxisWarning, false)) {
			showAxisWarningText();
		}
		ImGui::Separator();
		if (showMappingButtonGroup(2, 2, showAxisWarning, false)) {
			showAxisWarningText();
		}
		ImGui::Separator();
		showMappingButtonGroup(4, 14);
		ImGui::Separator();
		showMappingButtonGroup(18, 2);

		if (ImGui::GetIO().MouseClicked[0] && m_activeButtonIndex != -1) {
			deactivateMapping();
		}
	}

	ImGui::End();

	if (m_isDirty) {
		saveConfig();
		m_isDirty = false;
	}
}

void InputWindow::showTabBar()
{
	ImGui::BeginTabBar("##inputWindowTabBar", ImGuiTabBarFlags_NoTooltip);

	int prevTabIndex{ m_tabIndex };

	for (int i{}; i < 4; ++i) {
		if (ImGui::BeginTabItem(fmt::format("Pad #{}", i + 1).c_str())) {
			m_tabIndex = i;
			ImGui::EndTabItem();
		}
	}

	if (m_tabIndex != prevTabIndex) {
		loadConfig();
	}

	ImGui::EndTabBar();
}

void InputWindow::showAxisWarningText()
{
	ImGui::TextColored(ImGuiUtil::vec4FromRGBA(240, 240, 0, 255), "Axis components must have the same input type.");
}

void InputWindow::handleEvent(SDL_Event& e)
{
	bool update{};

	if (!m_devices.empty() && (e.type == SDL_JOYDEVICEADDED || e.type == SDL_JOYDEVICEREMOVED)) {
		std::string currentDeviceName = m_devices[m_deviceIndex];
		updateDeviceList();

		if (e.type == SDL_JOYDEVICEREMOVED && currentDeviceName != "Keyboard") {
			// select previous device if available
			for (int i{}; i < SDL_NumJoysticks(); ++i) {
				if (currentDeviceName == SDL_JoystickNameForIndex(i)) {
					m_deviceIndex = i + 1;
					openSelectedDevice();
				}
			}
		}
		else if (e.type == SDL_JOYDEVICEADDED) {
			// select new device
			m_deviceIndex = e.jdevice.which + 1;
			openSelectedDevice();
		}
	}
	else if (m_activeButtonIndex != -1) {
		auto& map = m_mappings[m_activeMappingSectionIndex][m_activeMappingIndex];

		if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
				map.type = MappingUtil::MapType::None;
			}
			else {
				map.type = MappingUtil::MapType::Key;
				map.key.scancode = e.key.keysym.scancode; 
			}
			update = true;
		}
		else if ((e.type == SDL_JOYBUTTONDOWN || e.type == SDL_JOYHATMOTION)
			&& SDL_GetTicks() - m_ticksBetweenInputs > s_numTicksBetweenInputs) {

			if (e.type == SDL_JOYBUTTONDOWN) {
				map.type = MappingUtil::MapType::JoyButton;
				map.jbutton.button = e.jbutton.button;
				update = true;
			}
			else if (e.type == SDL_JOYHATMOTION) {
				if (e.jhat.value == SDL_HAT_UP
					|| e.jhat.value == SDL_HAT_DOWN
					|| e.jhat.value == SDL_HAT_LEFT
					|| e.jhat.value == SDL_HAT_RIGHT) {

					switch (e.jhat.value) {
					case SDL_HAT_UP: map.jhat.dir = MappingUtil::JoyHatDir::Up; break;
					case SDL_HAT_DOWN: map.jhat.dir = MappingUtil::JoyHatDir::Down; break;
					case SDL_HAT_LEFT: map.jhat.dir = MappingUtil::JoyHatDir::Left; break;
					case SDL_HAT_RIGHT: map.jhat.dir = MappingUtil::JoyHatDir::Right; break;
					}

					map.type = MappingUtil::MapType::JoyHat;
					map.jhat.hat = e.jhat.hat;
					update = true;
				}
			}
		}
		else if (!m_disableAxisMotion && e.type == SDL_JOYAXISMOTION) {
			if (!m_joystickInitialAxisStates.empty() &&
				(e.jaxis.value < m_joystickInitialAxisStates[e.jaxis.axis] - 20000
					|| e.jaxis.value > m_joystickInitialAxisStates[e.jaxis.axis] + 20000)) {

				if (SDL_GetTicks() - m_ticksBetweenInputs > s_numTicksBetweenInputs) {
					m_ticksBetweenInputs = SDL_GetTicks();

					map.type = MappingUtil::MapType::JoyAxis;
					map.jaxis.axis = e.jaxis.axis;
					map.jaxis.dir = e.jaxis.value < m_joystickInitialAxisStates[e.jaxis.axis] - 20000
						? MappingUtil::JoyAxisDir::Negative : MappingUtil::JoyAxisDir::Positive;
					map.jaxis.deadzone = 0;
					update = true;
				}
			}
		}
		else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
			deactivateMapping();
		}
	}
	
	if (update) {
		moveToNextMappingButton();
		m_isDirty = true;
	}
}

void InputWindow::moveToNextMappingButton()
{
	++m_activeMappingIndex;
	if (m_activeMappingIndex >= NumMappings) {
		m_activeMappingIndex = 0;
		++m_activeMappingSectionIndex;
		if (m_activeMappingSectionIndex >= NumMappingSections) {
			m_activeMappingSectionIndex = 0;
		}
	}

	m_activeButtonIndex = m_activeMappingIndex + m_activeMappingSectionIndex * NumMappings;
	m_scrollToActiveButton = true;
}

void InputWindow::deactivateMapping()
{
	m_activeButtonIndex = -1;
	m_activeMappingIndex = -1;
	m_activeMappingSectionIndex = -1;
}

void InputWindow::updateDeviceList()
{
	SDL_JoystickUpdate();

	m_devices.clear();
	m_devices.emplace_back("Keyboard");

	for (int i{}; i < SDL_NumJoysticks(); ++i) {
		m_devices.emplace_back(SDL_JoystickNameForIndex(i));
	}

	m_deviceIndex = 0;
	openSelectedDevice();
}

void InputWindow::openSelectedDevice()
{
	if (m_deviceIndex == 0) {
		m_joystick.reset(nullptr);
	}
	else {
		m_joystick.reset(SDL_JoystickOpen(m_deviceIndex - 1));
		
		if (!m_joystick) {
			m_deviceIndex = 0;
			return;
		}

		// dump initial axis states
		int numAxes = SDL_JoystickNumAxes(m_joystick.get());
		m_joystickInitialAxisStates.resize(numAxes);

		for (int i{}; i < numAxes; ++i) {
			m_joystickInitialAxisStates[i] = SDL_JoystickGetAxis(m_joystick.get(), i);
		}
	}
}

bool InputWindow::showMappingButton(int mappingIndex, int mappingSectionIndex, bool isActive, bool isWarning, bool showAxisDeadzone, const std::string& tooltipStart)
{
	bool triggered{};
	auto& map = m_mappings[mappingSectionIndex][mappingIndex];

	if (isWarning) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImGuiUtil::vec4FromRGBA(180, 180, 40, 255));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGuiUtil::vec4FromRGBA(225, 225, 86, 200));
	}

	if (isActive) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImGuiUtil::vec4FromRGBA(70, 70, 70, 255));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGuiUtil::vec4FromRGBA(90, 90, 90, 200));
	}

	bool isAxis = showAxisDeadzone && map.type == MappingUtil::MapType::JoyAxis;

	if (isActive && m_scrollToActiveButton) {
		ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + ImGui::GetCursorPosY());
		m_scrollToActiveButton = false;
	}

	if (ImGui::Button(fmt::format("{}##b{}_{}", MappingUtil::formatSingleMap(map, true).c_str(), mappingIndex, mappingSectionIndex).c_str(), { isAxis ? 75.0f : 148.0f, 0 })) {
		triggered = true;
		m_scrollToActiveButton = true;
	}

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("%s mapping", tooltipStart.c_str());
	}

	if (isAxis) {
		ImGui::SameLine();
		ImGui::PushItemWidth(65);
		if (ImGui::InputFloat(fmt::format("##a{}_{}", mappingIndex, mappingSectionIndex).c_str(), &map.jaxis.deadzone, 0.0f, 0.0f, "%.2f%%")) {
			map.jaxis.deadzone = mystd::clamp(map.jaxis.deadzone, 0.0f, 100.0f);
			m_isDirty = true;
		}
		ImGui::PopItemWidth();

		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("%s axis deadzone", tooltipStart.c_str());
		}
	}

	if (isActive) {
		ImGui::PopStyleColor(2);
	}

	if (isWarning) {
		ImGui::PopStyleColor(2);
	}

	return triggered;
}

bool InputWindow::showMappingButtonGroup(int beg, int num, std::function<bool(int, int, int, int)> showAsWarning, bool showAxisDeadzone)
{
	bool shownAsWarning{};

	for (int i{ beg }; i < beg + num; ++i) {
		for (int j{}; j < NumMappingSections; ++j) {
			int buttonIndex = j * NumMappings + i;

			bool w = showAsWarning(beg, num, i, j);
			if (w) shownAsWarning = true;

			if (showMappingButton(i, j, m_activeButtonIndex == buttonIndex, w, showAxisDeadzone, fmt::format("{} `{}`", s_mappingSectionLabels[j], s_mappingLabels[i]))) {
				m_activeButtonIndex = buttonIndex;
				m_activeMappingIndex = i;
				m_activeMappingSectionIndex = j;
			}
			ImGui::SameLine();
		}
		ImGui::Text("%s", s_mappingLabels[i].c_str());
	}

	return shownAsWarning;
}

static std::pair<int, int> scan2i(const std::string& str)
{
	std::pair<int, int> v;
	if (std::sscanf(str.c_str(), "%d,%d", &v.first, &v.second) == 2) {
		return v;
	}
	else {
		return{};
	}
}

void InputWindow::loadConfig()
{
	auto section = Mupen::ConfigSection{ m_core, fmt::format("Input-SDL-Control{}", m_tabIndex + 1) };

	std::string deviceName = section.getStringOrDefault("name", "Keyboard");
	m_plugged = section.getBoolOrDefault("plugged", false);
	int plugin = section.getIntOrDefault("plugin", 2);
	auto analogDeadzones = scan2i(section.getStringOrDefault("AnalogDeadzone", "4096,4096"));
	auto analogPeaks = scan2i(section.getStringOrDefault("AnalogPeak", "32768,32768"));
	m_analogDeadzones[0] = MappingUtil::deadzoneToPercent(analogDeadzones.first);
	m_analogDeadzones[1] = MappingUtil::deadzoneToPercent(analogDeadzones.second);
	m_analogPeaks[0] = MappingUtil::deadzoneToPercent(analogPeaks.first);
	m_analogPeaks[1] = MappingUtil::deadzoneToPercent(analogPeaks.second);

	if (plugin == 1 || plugin == 2) m_pluginIndex = plugin - 1;
	else if (plugin == 5) m_pluginIndex = 2;
	else m_pluginIndex = 0;

	for (auto& mappingSection : m_mappings) mappingSection.fill({});

	for (std::size_t i{}; i < s_mappingProperties.size(); ++i) {
		std::string format = section.getStringOrDefault(s_mappingProperties[i].c_str(), "");
		std::vector<std::string> keys = split(format, ")");

		for (int j{}; j < static_cast<int>(keys.size()) - 1 && j < NumMappingSections; ++j) {
			trim(keys[j]);
			m_mappings[j][i + 4] = MappingUtil::parseSingleMap(fmt::format("{})", keys[j]));
		}
	}

	for (std::size_t i{}; i < s_mapping2Properties.size(); ++i) {
		std::string format = section.getStringOrDefault(s_mapping2Properties[i].c_str(), "");
		std::vector<std::string> keys = split(format, ")");

		for (int j{}; j < static_cast<int>(keys.size()) - 1 && j < NumMappingSections; ++j) {
			trim(keys[j]);
			auto map = MappingUtil::parseDoubleMap(fmt::format("{})", keys[j]));
			m_mappings[j][i * 2] = map.first;
			m_mappings[j][i * 2 + 1] = map.second;
		}
	}

	// select loaded device if available
	updateDeviceList();
	for (int i{}; i < static_cast<int>(m_devices.size()); ++i) {
		if (m_devices[i] == deviceName) {
			m_deviceIndex = i;
			openSelectedDevice();
		}
	}
}

void InputWindow::saveConfig()
{
	auto section = Mupen::ConfigSection{ m_core, fmt::format("Input-SDL-Control{}", m_tabIndex + 1) };

	int plugin = 1;
	if (m_pluginIndex == 1) plugin = 2;
	else if (m_pluginIndex == 2) plugin = 5;

	section.setInt("version", 2);
	section.setInt("mode", 0);
	section.setInt("device", m_deviceIndex - 1);
	section.setString("name", m_devices[m_deviceIndex]);
	section.setBool("plugged", m_plugged);
	section.setInt("plugin", plugin);
	section.setBool("mouse", false);
	section.setString("MouseSensitivity", "2.00,2.00");
	section.setString("AnalogDeadzone", fmt::format("{},{}",
		MappingUtil::deadzoneFromPercent(m_analogDeadzones[0]),
		MappingUtil::deadzoneFromPercent(m_analogDeadzones[1])));
	section.setString("AnalogPeak", fmt::format("{},{}",
		MappingUtil::deadzoneFromPercent(m_analogPeaks[0]),
		MappingUtil::deadzoneFromPercent(m_analogPeaks[1])));

	for (std::size_t i{}; i < s_mappingProperties.size(); ++i) {
		std::ostringstream oss;

		for (int j{}; j < NumMappingSections; ++j) {
			oss << MappingUtil::formatSingleMap(m_mappings[j][i + 4], false) << ' ';
		}

		std::string key = oss.str();
		trim(key);

		section.setString(s_mappingProperties[i].c_str(), key);
	}

	for (std::size_t i{}; i < s_mapping2Properties.size(); ++i) {
		std::ostringstream oss;

		for (int j{}; j < NumMappingSections; ++j) {
			std::string format = m_mappings[j][i * 2].type == m_mappings[j][i * 2 + 1].type ? MappingUtil::formatDoubleMap({ m_mappings[j][i * 2], m_mappings[j][i * 2 + 1] }) : "";
			oss << format << ' ';
		}

		std::string key = oss.str();
		trim(key);

		section.setString(s_mapping2Properties[i].c_str(), key);
	}

	section.save();
}
