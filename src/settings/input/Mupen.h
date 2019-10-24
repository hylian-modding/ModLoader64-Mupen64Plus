#pragma once

#include <string>
#include <vector>

#include <m64p_common.h>
#include <m64p_config.h>
#include <m64p_frontend.h>

#include "../../imgui/SDLWrapper.h"

namespace Mupen
{

struct Error : std::runtime_error
{
	Error(const char* msg) :
		std::runtime_error{ msg }
	{}
};

class Core final
{
public:
	void load(const char* path);

	void checked(m64p_error error) const
	{
		if (error != M64ERR_SUCCESS) {
			throw Error{ CoreErrorMessage(error) };
		}
	}

private:
	SDL::SharedObject m_so;

public:
	ptr_CoreErrorMessage CoreErrorMessage{};
	ptr_CoreStartup CoreStartup{};
	ptr_CoreShutdown CoreShutdown{};
	ptr_ConfigListSections ConfigListSections{};
	ptr_ConfigOpenSection ConfigOpenSection{};
	ptr_ConfigListParameters ConfigListParameters{};
	ptr_ConfigSaveFile ConfigSaveFile{};
	ptr_ConfigSaveSection ConfigSaveSection{};
	ptr_ConfigHasUnsavedChanges ConfigHasUnsavedChanges{};
	ptr_ConfigDeleteSection ConfigDeleteSection{};
	ptr_ConfigRevertChanges ConfigRevertChanges{};
	ptr_ConfigSetParameter ConfigSetParameter{};
	ptr_ConfigSetParameterHelp ConfigSetParameterHelp{};
	ptr_ConfigGetParameter ConfigGetParameter{};
	ptr_ConfigGetParameterType ConfigGetParameterType{};
	ptr_ConfigGetParameterHelp ConfigGetParameterHelp{};
	ptr_ConfigSetDefaultInt ConfigSetDefaultInt{};
	ptr_ConfigSetDefaultFloat ConfigSetDefaultFloat{};
	ptr_ConfigSetDefaultBool ConfigSetDefaultBool{};
	ptr_ConfigSetDefaultString ConfigSetDefaultString{};
	ptr_ConfigGetParamInt ConfigGetParamInt{};
	ptr_ConfigGetParamFloat ConfigGetParamFloat{};
	ptr_ConfigGetParamBool ConfigGetParamBool{};
	ptr_ConfigGetParamString ConfigGetParamString{};
};

struct CoreWrapper final
{
	Core core;

	CoreWrapper(const char* path)
	{
		core.load(path);
		core.checked(core.CoreStartup(0x020001, "emulator", "emulator", nullptr, nullptr, nullptr, nullptr));
	}

	~CoreWrapper()
	{
		core.checked(core.CoreShutdown());
	}
};

class ConfigSection final
{
public:
	struct Parameter
	{
		std::string name;
		m64p_type type;
	};

	ConfigSection(const Core& core, const std::string& name);
	ConfigSection(const ConfigSection&) = delete;
	ConfigSection(ConfigSection&& other) noexcept;
	ConfigSection& operator=(const ConfigSection&) = delete;
	ConfigSection& operator=(ConfigSection&& other) noexcept;

	bool hasUnsavedChanges() const;
	void remove() const;
	void save() const;
	void revert() const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setBool(const std::string& name, bool value) const;
	void setString(const std::string& name, const std::string& value) const;
	void setDefaultInt(const std::string& name, int value, const std::string& help = "") const;
	void setDefaultFloat(const std::string& name, float value, const std::string& help = "") const;
	void setDefaultBool(const std::string& name, bool value, const std::string& help = "") const;
	void setDefaultString(const std::string& name, const std::string& value, const std::string& help = "") const;
	int getInt(const std::string& name) const;
	float getFloat(const std::string& name) const;
	bool getBool(const std::string& name) const;
	std::string getString(const std::string& name) const;
	int getIntOrDefault(const std::string& name, int defaultValue) const;
	float getFloatOrDefault(const std::string& name, float defaultValue) const;
	bool getBoolOrDefault(const std::string& name, bool defaultValue) const;
	std::string getStringOrDefault(const std::string& name, const std::string& defaultValue) const;
	std::string getHelp(const std::string& help) const;
	std::vector<Parameter> getParams() const;

private:
	const Core* m_core;
	std::string m_name;
	m64p_handle m_handle;
};

}