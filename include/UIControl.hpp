// include/UIControl.hpp
#pragma once
#include <string>
#include <vector>
#include "Config.hpp"

namespace UIControl {
    using GameMode = ::GameMode;
    using GameConfig = ::GameConfig;
    using ::makeConfig;

    struct ScoreEntry {
        std::string name;
        int score;
    };
    std::vector<ScoreEntry> loadScores(const std::string& path);

    // Menu principal
    void showMenu();
    void processEntry(int opcion);

    // Menu de Instrucciones
    void showGameInstructions();

    // Menu de Controles
    void showControlsGuide();

    // Selector de modo de juego
    GameMode selectGameMode();

    // Marcador de puntos
    void showScoreboardUI(const std::string& path);
}