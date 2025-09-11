#pragma once
#include <iostream>
#include <limits>
#include <string>

using namespace std;

namespace UICommon {
    inline void clearScreen() {
        // limpieza simple de consola (Windows/Linux)
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    inline void waitEnter() {
        cout << "\nPresiona Enter para continuar...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    inline int readInt(const string& prompt, int low, int high) {
        int v;
        while (true) {
            cout << prompt;
            if (cin >> v && v >= low && v <= high) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return v;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Entrada invalida. Intenta de nuevo.\n";
        }
    }
}