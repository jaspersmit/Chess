#include "Util.h"

std::vector<std::string> Split(const std::string& str, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(str);
    std::string part;
    while (std::getline(ss, part, delimiter)) parts.push_back(part);
    return parts;
}