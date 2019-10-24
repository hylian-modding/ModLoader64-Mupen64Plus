#include "Mupen.h"

#define loadFunction(func) func = m_so.loadFunction<ptr_##func>(#func)

namespace Mupen
{

void Core::load(const char* path)
{
	m_so.load(path);

	loadFunction(CoreErrorMessage);
	loadFunction(CoreStartup);
	loadFunction(CoreShutdown);
	loadFunction(ConfigListSections);
	loadFunction(ConfigOpenSection);
	loadFunction(ConfigListParameters);
	loadFunction(ConfigSaveFile);
	loadFunction(ConfigSaveSection);
	loadFunction(ConfigHasUnsavedChanges);
	loadFunction(ConfigDeleteSection);
	loadFunction(ConfigRevertChanges);
	loadFunction(ConfigSetParameter);
	loadFunction(ConfigSetParameterHelp);
	loadFunction(ConfigGetParameter);
	loadFunction(ConfigGetParameterType);
	loadFunction(ConfigGetParameterHelp);
	loadFunction(ConfigSetDefaultInt);
	loadFunction(ConfigSetDefaultFloat);
	loadFunction(ConfigSetDefaultBool);
	loadFunction(ConfigSetDefaultString);
	loadFunction(ConfigGetParamInt);
	loadFunction(ConfigGetParamFloat);
	loadFunction(ConfigGetParamBool);
	loadFunction(ConfigGetParamString);
}

ConfigSection::ConfigSection(const Core& core, const std::string& name) :
	m_core{ &core },
	m_name{ name },
	m_handle{}
{
	m_core->checked(m_core->ConfigOpenSection(name.c_str(), &m_handle));
}

ConfigSection::ConfigSection(ConfigSection&& other) noexcept :
	m_core{ other.m_core },
	m_name{ std::move(other.m_name) },
	m_handle{ other.m_handle }
{

}

ConfigSection& ConfigSection::operator=(ConfigSection&& other) noexcept
{
	m_core = m_core;
	m_handle = m_handle;
	m_name = std::move(other.m_name);

	return *this;
}

bool ConfigSection::hasUnsavedChanges() const
{
	return m_core->ConfigHasUnsavedChanges(m_name.c_str());
}

void ConfigSection::remove() const
{
	m_core->checked(m_core->ConfigDeleteSection(m_name.c_str()));
}

void ConfigSection::save() const
{
	m_core->checked(m_core->ConfigSaveSection(m_name.c_str()));
}

void ConfigSection::revert() const
{
	m_core->checked(m_core->ConfigRevertChanges(m_name.c_str()));
}

void ConfigSection::setInt(const std::string& name, int value) const
{
	m_core->checked(m_core->ConfigSetParameter(m_handle, name.c_str(), M64TYPE_INT, &value));
}

void ConfigSection::setFloat(const std::string& name, float value) const
{
	m_core->checked(m_core->ConfigSetParameter(m_handle, name.c_str(), M64TYPE_FLOAT, &value));
}

void ConfigSection::setBool(const std::string& name, bool value) const
{
	int v = value;
	m_core->checked(m_core->ConfigSetParameter(m_handle, name.c_str(), M64TYPE_BOOL, &v));
}

void ConfigSection::setString(const std::string& name, const std::string& value) const
{
	m_core->checked(m_core->ConfigSetParameter(m_handle, name.c_str(), M64TYPE_STRING, value.c_str()));
}

void ConfigSection::setDefaultInt(const std::string& name, int value, const std::string& help) const
{
	m_core->checked(m_core->ConfigSetDefaultInt(m_handle, name.c_str(), value, help.c_str()));
}

void ConfigSection::setDefaultFloat(const std::string& name, float value, const std::string& help) const
{
	m_core->checked(m_core->ConfigSetDefaultFloat(m_handle, name.c_str(), value, help.c_str()));
}

void ConfigSection::setDefaultBool(const std::string& name, bool value, const std::string& help) const
{
	m_core->checked(m_core->ConfigSetDefaultBool(m_handle, name.c_str(), value, help.c_str()));
}

void ConfigSection::setDefaultString(const std::string& name, const std::string& value, const std::string& help) const
{
	m_core->checked(m_core->ConfigSetDefaultString(m_handle, name.c_str(), value.c_str(), help.c_str()));
}

int ConfigSection::getInt(const std::string& name) const
{
	return m_core->ConfigGetParamInt(m_handle, name.c_str());
}

float ConfigSection::getFloat(const std::string& name) const
{
	return m_core->ConfigGetParamFloat(m_handle, name.c_str());
}

bool ConfigSection::getBool(const std::string& name) const
{
	return m_core->ConfigGetParamBool(m_handle, name.c_str());
}

std::string ConfigSection::getString(const std::string& name) const
{
	return m_core->ConfigGetParamString(m_handle, name.c_str());
}

int ConfigSection::getIntOrDefault(const std::string& name, int defaultValue) const
{
	m64p_type type;
	if (m_core->ConfigGetParameterType(m_handle, name.c_str(), &type) == M64ERR_INPUT_NOT_FOUND
		|| type != M64TYPE_INT) {
		return defaultValue;
	}
	else {
		return getInt(name);
	}
}

float ConfigSection::getFloatOrDefault(const std::string& name, float defaultValue) const
{
	m64p_type type;
	if (m_core->ConfigGetParameterType(m_handle, name.c_str(), &type) == M64ERR_INPUT_NOT_FOUND
		|| type != M64TYPE_FLOAT) {
		return defaultValue;
	}
	else {
		return getFloat(name);
	}
}

bool ConfigSection::getBoolOrDefault(const std::string& name, bool defaultValue) const
{
	m64p_type type;
	if (m_core->ConfigGetParameterType(m_handle, name.c_str(), &type) == M64ERR_INPUT_NOT_FOUND
		|| type != M64TYPE_BOOL) {
		return defaultValue;
	}
	else {
		return getBool(name);
	}
}

std::string ConfigSection::getStringOrDefault(const std::string& name, const std::string& defaultValue) const
{
	m64p_type type;
	if (m_core->ConfigGetParameterType(m_handle, name.c_str(), &type) == M64ERR_INPUT_NOT_FOUND
		|| type != M64TYPE_STRING) {
		return defaultValue;
	}
	else {
		return getString(name);
	}
}

std::string ConfigSection::getHelp(const std::string& help) const
{
	return m_core->ConfigGetParameterHelp(m_handle, help.c_str());
}

static void listParamsFunc(void* context, const char* paramName, m64p_type paramType)
{
	auto params = reinterpret_cast<std::vector<ConfigSection::Parameter>*>(context);
	params->push_back({ paramName, paramType });
}

std::vector<ConfigSection::Parameter> ConfigSection::getParams() const
{
	std::vector<Parameter> params;
	m_core->checked(m_core->ConfigListParameters(m_handle, &params, listParamsFunc));
	return params;
}

}
