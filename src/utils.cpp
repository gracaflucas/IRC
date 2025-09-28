#include "../include/utils.hpp"

int parsePort(const std::string &rawPort) {
    if (rawPort.empty()) {
        throw std::invalid_argument("Port cannot be empty.");
    }
    char *end;
    long port = std::strtol(rawPort.c_str(), &end, 10);
    if (*end != '\0') {
        throw std::invalid_argument("Port must contain only digits.");
    }
    if (port < 1 || port > 65535) {
        throw std::out_of_range("Port must be between 1 and 65535.");
    }
    return static_cast<int>(port);
}

std::string parsePassword(const std::string &rawPassword) {
    if (rawPassword.empty())
        throw std::invalid_argument("Password cannot be empty.");
    for (size_t i = 0; i < rawPassword.length(); i++) {
        if (!std::isprint(rawPassword[i])) {
            throw std::invalid_argument("Password contains non-printable character.");
        }
    }
    return rawPassword;
}