#include <cstdio>
#include <string>
#include <iostream>

FILE* my_fopen(const char* filename, const char* mode) {
    std::string fullPath = "ux0:data/CSPSP/" + std::string(filename);
    std::cout << "Redirected fopen call: " << fullPath << std::endl;
    return fopen(fullPath.c_str(), mode);
}