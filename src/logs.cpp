#include "logs.hpp"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

std::tm timestamp() { // date and time mark
		auto now = std::chrono::system_clock::now();
		std::time_t now_time = std::chrono::system_clock::to_time_t(now);
		std::tm time;
		errno_t err = localtime_s(&time, &now_time);
		return time;
}

void logCreate(const std::string &message) {
		std::tm local_time = timestamp();
		std::ostringstream oss;
		std::string filename;
		oss << std::put_time(&local_time, "%d-%m-%Y_%H-%M");
		filename = oss.str() + ".log";
		std::ofstream file("../logs/"+filename, std::ios::app);
		if (!file) {
				throw std::runtime_error("File can't be created");
		}
		file << message << "\n";
		file.close();
		return;
}

