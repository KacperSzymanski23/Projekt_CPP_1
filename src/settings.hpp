#ifndef SETTINGS_HPP
#define SETTINGS_HPP

// STD
#include <string>
#include <unordered_map>

class Settings {
	  public:
		static Settings &getInstance();

		// Usuwanie konstruktora kopiującego i przenoszącego
		// ponieważ ta klasa to Singleton
		Settings(const Settings &) = delete;
		Settings &operator=(const Settings &) = delete;

		Settings(Settings &&) = delete;
		Settings &operator=(Settings &&) = delete;

		~Settings() = default;

		bool loadSettings(); // Wczytuje ustawienia z pliku
		bool saveSettings(); // Zapisuje ustawienia do pliku

		void setSettingsEntry(const std::string &entryKey, const std::string &entryValue); // Ustawia wartość ustawienia o podanym kluczu
		std::string getSettingsEntry(const std::string &entryKey);                         // Zwraca wartość ustawienia o podanym kluczu

		bool isEntryKeyExists(const std::string &entryKey); // Sprawdza czy ustawienie o podanym kluczu istnieje

	  private:
		explicit Settings(const std::string &settingsPath); // Konstruktor prywatny

		static std::string trim(const std::string &str);                                                // Usuwa białe znaki z początku i końca łańcucha
		static bool parseLine(const std::string &line, std::string &entryKey, std::string &entryValue); // Sprawdza poprawność linii ustawień

		std::string m_settingsPath; // Ścieżka do pliku z ustawieniami

		std::unordered_map<std::string, std::string> m_settings; // Mapa z ustawieniami w formacie klucz-wartość
};

#endif // SETTINGS_HPP
