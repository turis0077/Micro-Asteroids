// src/ui/gameOverScreen.cpp
#include "../../include/UIControl.hpp"
#include "../../include/UICommon.hpp"
#include "../../include/Game.hpp"

#include <iostream>
#include <iomanip>
#include <limits>
#include <string>

namespace UIControl {
    using namespace std;

    void showGameOverScreen(Game& game) {
    UICommon::clearScreen();
    
    cout << "\n\n";
    cout << "  ╔══════════════════════════════════════╗\n";
    cout << "  ║            JUEGO TERMINADO           ║\n";
    cout << "  ╠══════════════════════════════════════╣\n";
    
    if (game.playersCount() == 1) {
        if (game.player1Won()) {
            cout << "  ║          ¡FELICIDADES!               ║\n";
            cout << "  ║      Has destruido todos los         ║\n";
            cout << "  ║          asteroides!                 ║\n";
        } else {
            cout << "  ║          GAME OVER                   ║\n";
            cout << "  ║      Te quedaste sin vidas           ║\n";
        }
        cout << "  ║                                      ║\n";
        cout << "  ║  Puntuación Final: " << setw(15) << game.p1Score() << "  ║\n";
    } else {
        if (game.player1Won()) {
            cout << "  ║        ¡JUGADOR 1 GANA!              ║\n";
        } else if (game.player2Won()) {
            cout << "  ║        ¡JUGADOR 2 GANA!              ║\n";
        } else {
            cout << "  ║           ¡EMPATE!                   ║\n";
        }
        cout << "  ║                                      ║\n";
        cout << "  ║  Jugador 1: " << setw(20) << game.p1Score() << "  ║\n";
        cout << "  ║  Jugador 2: " << setw(20) << game.p2Score() << "  ║\n";
    }
    
    cout << "  ╠══════════════════════════════════════╣\n";
    cout << "  ║  Ingresa tu nombre para guardar      ║\n";
    cout << "  ║  tu puntuación:                      ║\n";
    cout << "  ╚══════════════════════════════════════╝\n";
    cout << "\n  Nombre: ";
    
    // Limpia posible '\n' pendiente antes de getline
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string playerName;
    getline(cin, playerName);
    if (playerName.empty()) playerName = "Anónimo";

    // Puntaje final: único jugador o ganador en 2P
    int finalScore = (game.playersCount() == 1)
                     ? game.p1Score()
                     : (game.player1Won() ? game.p1Score() : game.p2Score());

    game.saveScore(playerName, finalScore);
    
    cout << "\n  ¡Puntuación guardada exitosamente!\n";
    cout << "  Presiona Enter para continuar...";
    cin.get();
}

}