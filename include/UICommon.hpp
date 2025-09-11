#pragma once
#include <iostream>

namespace UICommon {
    inline void clearScreen() {
        // limpieza simple de consola (Windows/Linux)
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
}