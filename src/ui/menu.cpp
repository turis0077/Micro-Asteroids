#include <iostream>
#include "../../include/UICommon.hpp"
#include "scoreboard.hpp"
#include "../../include/Game.hpp"

using namespace std;

void showMenu() {
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
"·  ·    .    ·  ·     .      :.     ·" << endl;

 }

 void processEntry(int opcion) {
    switch (opcion) {
        case 1:
            {
                std::cout << "\nIniciando juego...\n";
                Game game(80, 24);
                game.run();
            }
            break;
        case 2:
            UICommon::clearScreen();
            showScoreboardUI("src/data/scores.txt");
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
        UICommon::clearScreen();
        showMenu();
        cout << "Opción: ";
        cin >> opcion;
        processEntry(opcion);
    } while (opcion != 5);
    return 0;
}