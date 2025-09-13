#pragma once
#include <iostream>
#include <limits>
#include <string>
#include <clocale>
#ifdef _WIN32
    #include <windows.h>
#endif

using namespace std;

namespace UICommon {
    inline void initConsoleUTF8() {
        #ifdef _WIN32
            SetConsoleOutputCP(CP_UTF8);
            SetConsoleCP(CP_UTF8);
        #endif
        setlocale(LC_ALL, ".UTF-8");
    }
    
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
        cout.flush();
        
        if(cin.peek() == '\n')
            cin.get();

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