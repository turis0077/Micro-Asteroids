// include/Config.hpp
#pragma once
#include <string>

enum class GameMode { Modo1 = 1, Modo2 };

struct GameConfig {
    GameMode mode;
    int players;
    int livesPerPlayer;
    int largeAsteroids;
    int targetSmallToWin;
    int scorePerSmall;
    bool wrap;
};

inline GameConfig makeConfig(GameMode m) {
    GameConfig c{};
    c.mode = m;
    c.players = (m == GameMode::Modo1) ? 1 : 2;
    c.livesPerPlayer = 3;
    c.largeAsteroids = (m == GameMode::Modo1) ? 3 : 5;
    c.targetSmallToWin = (m == GameMode::Modo1) ? 6 : 10;
    c.scorePerSmall = 10;
    c.wrap = true;
    return c;
}
