#pragma once
#include <iostream>
class Logger {
public:
    static void info(const std::string& message) {
        std::cout << "\033[32m[I]\033[0m " << message << std::endl; // Зеленый
    }
    static void warning(const std::string& message) {
        std::cout << "\033[33m[W]\033[0m " << message << std::endl; // Желтый
    }
    static void error(const std::string& message) {
        std::cout << "\033[31m[E]\033[0m " << message << std::endl; // Красный
    }
    static void debug(const std::string& message) {
        std::cout << "\033[34m[D]\033[0m " << message << std::endl; // Синий
    }
};

