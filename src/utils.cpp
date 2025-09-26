#include "../include/utils.hpp"

int parsePort(std::string rawPort) {
    int parsed = atoi(rawPort.c_str());
    return parsed;
}

std::string parsePassword(std::string rawPassword) {
    return rawPassword;
}