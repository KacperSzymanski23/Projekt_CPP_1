#ifndef LOGS_HPP
#define LOGS_HPP

#include <ctime>
#include <source_location>
#include <string>

std::tm timestamp();
void logCreate(const std::string &message, const std::source_location LOCATION = std::source_location::current());

#endif // LOGS_HPP