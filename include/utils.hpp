#ifndef UTILS_HPP
# define UTILS_HPP

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>
#include <sys/socket.h>

int parsePort(const std::string &rawPort);
std::string parsePassword(const std::string &rawPassword);

// New utility functions
std::vector<std::string> splitString(const std::string &str, char delimiter);
void sendResponse(int fd, const std::string &response);
bool isValidChannelName(const std::string &name);

#endif