#include "logs.hpp"
// STD
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
// Tracy
#include <tracy/Tracy.hpp>

std::tm timestamp() { // date and time mark
		auto now = std::chrono::system_clock::now();
		std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
		std::tm time;
#ifdef _WIN32
		localtime_s(&time, &nowTime);
#else
		localtime_r(&nowTime, &time);
#endif

		return time;
}

void logCreate(const std::string &message, const std::source_location LOCATION) {
		ZoneScoped;

		static const std::tm LOCAL_TIME = timestamp();
		std::ostringstream oss;
		std::string filename;
		oss << std::put_time(&LOCAL_TIME, "%d-%m-%Y_%H-%M");
		filename = oss.str() + ".log";

		std::ofstream file("../../logs/" + filename, std::ios::app);
		if (!file) {
				throw std::runtime_error("File can't be created");
		}
		file << "file: " << LOCATION.file_name() << '(' << LOCATION.line() << ':' << LOCATION.column() << ") `" << LOCATION.function_name() << "`: " << message
			 << '\n';
		file.close();
}
