// src/ui/modeSelect.cpp
#include "../../include/UICommon.hpp"
#include "../../include/UIControl.hpp"
#include "../../include/Config.hpp"

#include <iostream>

namespace UIControl {
    using namespace std;
    
    GameMode selectGameMode() {
        UICommon::clearScreen();

        cout << "\n.    ·     .      : .    ·     .      :\n"
        "·   @   ·   .  ·   . O  ·   .  ·   .   \n"
        "  :·  o    .    ·   :·   ·      .    · \n"
        "·   .   ·  Selección de Modo ·   .  ·   \n"
        " .    ·  ·   .   ·   .  ·   . :·  ·  .\n"
        " . O  1) Solitario (1 jugador) .   o   \n"
        "  :·  2) Duo (2 jugadores)       ·    .  \n"
        "·   .  ·  ·   .:   ·     .    ·   ^  . \n"
        "  .   3) Revisar guía del astronauta   .\n"
        "·   .  ·    .   ·     ·    .    ·  .  .\n"
        " .    ·  ·   .   ·   .  ·   .   @    .\n";

        int mode = UICommon::readInt(
            "  -> Escribe la opción en terminal .  .\n"
        "·  ·    .    ·  ·     .      :.     ·\n\n", 1, 3);

        if (mode == 1) return GameMode::Modo1;
        else if (mode == 2) return GameMode::Modo2;
        else if (mode == 3) {
            showGameInstructions();
            return selectGameMode();
        }
        else {
            cout << "\nOpción inválida\n";
            return selectGameMode();
        }
    }
}

