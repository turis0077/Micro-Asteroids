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
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
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
    
    // Ancho/alto actuales de la terminal
    inline int termCols() {
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    #else
        winsize ws{};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
        return ws.ws_col ? ws.ws_col : 80;
    #endif
    }
    inline int termRows() {
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    #else
        winsize ws{};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
        return ws.ws_row ? ws.ws_row : 25;
    #endif
    }

    // Limpieza por ANSI
    inline void clearScreen() {
    #ifdef _WIN32
        int _rc = std::system("cls"); (void)_rc;
    #else
        std::cout << "\x1b[2J\x1b[H"; // borrar y mover cursor a 0,0
        std::cout.flush();
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