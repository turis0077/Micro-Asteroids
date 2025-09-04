// src/ui/menu.cpp
#include <iostream>
using namespace std;

void showMenu() {
    cout << ".    ·     .      : .    ·     .      :\n"
"·   @   ·   .  ·   . O  ·   .  ·   .   \n"
"  :·  o    .    ·   :·   ·      .    · \n"
"·   .   ·  Asteroids  .  ·   .  ·  ·    \n"
" .    ·  ·   .   ·   .  ·   . :·  ·    .\n"
" . O    1) Iniciar juego   ·   < .   o   \n"
"  :·  · 2) Puntajes     ·      ·      .  \n"
"·   .   3) Instrucciones   . \n"
"·   .  ·4) Salir    .    ·    .    ·  .\n"
" .    ·  ·   .   ·   .  ·   .   @    .\n"
"  -> Escribe la opción en terminal .  .\n"
"·  ·    .    ·  ·     .      :.     ·" << endl;

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