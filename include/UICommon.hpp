// include/UICommon.hpp

#pragma once
#include <iostream>
#include <limits>
#include <string>
#include <clocale>
#ifdef _WIN32
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
    #include <windows.h>
#endif

namespace UICommon {
    inline void initConsoleUTF8() {
        #ifdef _WIN32
            SetConsoleOutputCP(CP_UTF8);
            SetConsoleCP(CP_UTF8);
            setlocale(LC_ALL, ".UTF-8");
        #else
            setlocale(LC_ALL, "");
        #endif
    }
    
    // limpieza simple de consola (Windows/Linux)
    inline void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    inline void waitEnter() {
        std::cout << "\nPresiona Enter para continuar...";
        std::cout.flush();

        if(std::cin.peek() == '\n')
            std::cin.get();

        std::cin.get();
    }

    inline int readInt(const std::string& prompt, int low, int high) {
        int v;
        while (true) {
            std::cout << prompt;
            if (std::cin >> v && v >= low && v <= high) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return v;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Entrada invalida. Intenta de nuevo.\n";
        }
    }
}