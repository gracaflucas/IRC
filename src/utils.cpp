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

std::vector<std::string> splitString(const std::string &str, char delimiter) {
    std::vector<std::string> result;
    std::string current;
    
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == delimiter) {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current += str[i];
        }
    }
    
    if (!current.empty())
        result.push_back(current);
    
    return result;
}

void sendResponse(int fd, const std::string &response) {
    send(fd, response.c_str(), response.length(), 0);
}

bool isValidChannelName(const std::string &name) {
    if (name.empty() || name.length() > 50)
        return false;
    
    // Must start with # or &
    if (name[0] != '#' && name[0] != '&')
        return false;
    
    // Check for invalid characters (space, comma, control-G)
    for (size_t i = 1; i < name.length(); i++) {
        if (name[i] == ' ' || name[i] == ',' || name[i] == '\x07')
            return false;
    }
    
    return true;
}