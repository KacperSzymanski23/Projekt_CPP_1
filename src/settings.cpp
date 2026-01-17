#include "settings.hpp"
// STD
#include <fstream>
#include <iostream>
// Tracy
#include <tracy/Tracy.hpp>

Settings::Settings(const std::string &settingsPath)
	: m_settingsPath(settingsPath) {

		loadSettings();
}

std::string Settings::trim(const std::string &str) {
		ZoneScoped;

		std::string trimmed = str;

		trimmed.erase(0, trimmed.find_first_not_of(" \n\r\t"));
		trimmed.erase(trimmed.find_last_not_of(" \n\r\t") + 1);

		return trimmed;
}

bool Settings::parseLine(const std::string &line, std::string &entryKey, std::string &entryValue) {
		ZoneScoped;

		const std::string TRIMMED_LINE = trim(line);

		if (TRIMMED_LINE.empty()) {
				return false;
		}

		if (TRIMMED_LINE.front() == '#') {
				return false;
		}

		size_t const POSITION = TRIMMED_LINE.find_first_of('=');

		if (POSITION == std::string::npos) {
				std::cerr << "Invalid settings line '" << line << "'\n";

				return false;
		}

		entryKey = TRIMMED_LINE.substr(0, POSITION);
		entryValue = TRIMMED_LINE.substr(POSITION + 1);

		return !entryValue.empty();
}

bool Settings::loadSettings() {
		ZoneScoped;

		std::ifstream settingsFile(m_settingsPath);

		if (!settingsFile.is_open()) {
				std::cerr << "Can't open settings file.\n";

				settingsFile.close();
				return false;
		}

		m_settings.clear();

		std::string line{};
		std::string key{};
		std::string value{};

		while (std::getline(settingsFile, line)) {
				if (parseLine(line, key, value)) {
						m_settings[key] = value;
				}
		}

		settingsFile.close();
		return true;
}

bool Settings::saveSettings() {
		ZoneScoped;

		std::ofstream settingsFile(m_settingsPath, std::ios::trunc);

		if (!settingsFile.is_open()) {
				std::cerr << "Can't open settings file.\n";

				settingsFile.close();
				return false;
		}

		for (const auto &[key, value] : m_settings) {
				settingsFile << key << "=" << value << "\n";
		}

		settingsFile.close();
		return true;
}

std::string Settings::getSettingsEntry(const std::string &entryKey) {
		ZoneScoped;

		const auto ENTRY_VALUE = m_settings.find(entryKey);
		const auto LAST_ENTRY = m_settings.end();

		if (ENTRY_VALUE == LAST_ENTRY) {
				return {};
		}

		return ENTRY_VALUE->second;
}

void Settings::setSettingsEntry(const std::string &entryKey, const std::string &entryValue) {
		ZoneScoped;

		m_settings[entryKey] = entryValue;
}

bool Settings::isEntryKeyExists(const std::string &entryKey) {
		return m_settings.contains(entryKey);
}
