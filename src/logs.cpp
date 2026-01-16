#include "logs.hpp"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

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

void logCreate(const std::string &message) {
		std::tm localTime = timestamp();
		std::ostringstream oss;
		std::string filename;
		oss << std::put_time(&localTime, "%d-%m-%Y_%H-%M");
		filename = oss.str() + ".log";

		std::ofstream file("../../logs/" + filename, std::ios::app);
		if (!file) {
				throw std::runtime_error("File can't be created");
		}
		file << message << "\n";
		file.close();
}
