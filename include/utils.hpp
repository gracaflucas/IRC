#ifndef UTILS_HPP
# define UTILS_HPP

#include <cstdlib> // strtol
#include <stdexcept>

int parsePort(const std::string &rawPort);

std::string parsePassword(const std::string &rawPassword);

#endif