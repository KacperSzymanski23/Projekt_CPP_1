#ifndef LOGS_HPP
#define LOGS_HPP

#include <ctime>

std::tm timestamp();
void logCreate(const std::string& message);

#endif // LOGS_HPP