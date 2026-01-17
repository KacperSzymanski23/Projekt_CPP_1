#ifndef SETTINGS_HPP
#define SETTINGS_HPP

// STD
#include <string>
#include <unordered_map>

class Settings {
	  public:
		static Settings &getInstance();

		Settings(const Settings &) = delete;
		Settings &operator=(const Settings &) = delete;

		Settings(Settings &&) = delete;
		Settings &operator=(Settings &&) = delete;

		~Settings() = default;

		bool loadSettings();
		bool saveSettings();

		void setSettingsEntry(const std::string &entryKey, const std::string &entryValue);
		std::string getSettingsEntry(const std::string &entryKey);

		bool isEntryKeyExists(const std::string &entryKey);

	  private:
		explicit Settings(const std::string &settingsPath);

		static std::string trim(const std::string &str);
		static bool parseLine(const std::string &line, std::string &entryKey, std::string &entryValue);

		std::string m_settingsPath;

		std::unordered_map<std::string, std::string> m_settings;
};

#endif // SETTINGS_HPP
