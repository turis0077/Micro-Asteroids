// src/ui/menu.cpp

#include "../../include/UICommon.hpp"
#include "../../include/Game.hpp"
#include "../../include/UIControl.hpp"
#include "../../include/Concurrency.hpp"

#include <iostream>

namespace UIControl {
    using namespace std;

    void showMenu() {
        UICommon::clearScreen();
        cout << "\n.    ·     .      : .    ·     .      :\n"
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

    void startGameModeSelection() {
        GameMode mode = selectGameMode();
        GameConfig cfg = makeConfig(mode);
        UICommon::clearScreen();
        cout << "\nConfiguración del juego:\n\n"
            << "- Modo: " << ((cfg.mode == GameMode::Modo1) ? "Modo 1" : "Modo 2") << "\n"
            << "- Jugadores: " << cfg.players << "\n"
            << "- Vidas por jugador: " << cfg.livesPerPlayer << "\n"
            << "- Asteroides grandes iniciales: " << cfg.largeAsteroids << "\n"
            << "- Asteroides pequeños necesarios para ganar: " << cfg.targetSmallToWin << "\n";
        UICommon::waitEnter();
    }

    void processEntry(int opcion) {
        switch (opcion) {
            case 1:
                {
                    cout << "\nIniciando juego...\n\n";
                    
                    GameMode mode = selectGameMode();
                    GameConfig cfg = makeConfig(mode);
                    Game game(80, 24, cfg);
                    game.run();
                }
                break;
            case 2:
                UICommon::clearScreen();
                showScoreboardUI("src/data/scores.txt");
                break;
            case 3:
                cout << "\nMostrando instrucciones...\n\n";
                showGameInstructions();
                break;
            case 4:
                cout << "\nMostrando controles...\n";
                showControlsGuide();
                break;
            case 5:
                cout << "\nSaliendo del juego. ¡Adiós Astronauta(s)!\n";
                break;
            default:
                cout << "\nOpción no válida\n";
                break;
        }
    }
}