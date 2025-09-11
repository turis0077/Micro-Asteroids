// src/ui/menu.cpp
#include <iostream>
#include <limits>
#include <string>
using namespace std;

enum class GameMode { Modo1 = 1, Modo2 = 2 };

struct GameConfig {
    GameMode mode;
    int players;
    int livesPer;
    int largeAsteroids;
    int targetSmallToWin;
    int scorePerSmall;
    bool wrap;
};


static void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


static void waitEnter() {
    cout << "Presiona Enter para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}


static int readInt(const string& prompt, int minVal, int maxVal) {
    int v;
    while (true) {
        cout << prompt;
        if (cin >> v && v >= minVal && v <= maxVal) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return v;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entrada invalida. Intenta de nuevo.\n";
    }
}


static void showMenu() {
    clearScreen();
    cout << ".    ·     .      : .    ·     .      :\n"
"·   @   ·   .  ·   . O  ·   .  ·   .   \n"
"  :·  o    .    ·   :·   ·      .    · \n"
"·   .   ·  Asteroids  .  ·   .  ·  ·    \n"
" .    ·  ·   .   ·   .  ·   . :·  ·    .\n"
" . O    1) Iniciar juego   ·   < .   o   \n"
"  :·  · 2) Puntajes     ·      ·      .  \n"
"·   .   3) Instrucciones del juego . \n"
"  .     4) Controles    ·:  .     .  \n"
"·   .  ·5) Salir    .    ·    .    ·  .\n"
" .    ·  ·   .   ·   .  ·   .   @    .\n"
"  -> Escribe la opción en terminal .  .\n"
"·  ·    .    ·  ·     .      :.     ·\n\n";

 }

 void processEntry(int opcion) {
    switch (opcion) {
        case 1:
            cout << "\nIniciando juego...\n";
            break;
        case 2:
            cout << "\nMostrando puntajes...\n";
            break;
        case 3:
            cout << "\nMostrando instrucciones...\n";
            break;
        case 4:
            cout << "\nMostrando controles...\n";
            break;
        case 5:
            cout << "\nSaliendo del juego. ¡Adiós!\n";
            break;
        default:
            cout << "\nOpción no válida\n";
    }
}

int main() {
    int opcion = 0;
    do {
        showMenu();
        cout << "Opción: ";
        cin >> opcion;
        processEntry(opcion);
    } while (opcion != 4);
    return 0;
}