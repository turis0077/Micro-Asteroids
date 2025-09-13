// include/Game.hpp
#pragma once
#include "RenderBuffer.hpp"
#include "GameEntity.hpp"
#include "Config.hpp"
#include "Layout.hpp"
#include "HUD.hpp"
#include "Input.hpp"


#include <vector>
#include <chrono>

class Game {
public:
    Game(int width, int height, const GameConfig& cfg);
    void run();

private:
    int width, height;
    bool is_running = false;
    bool paused = false;

    RenderBuffer buffer;
    std::vector<GameEntity> entities;

    GameConfig cfg;
    Layout layout;
    HUD hud;
    Input input;

    std::chrono::steady_clock::time_point last_frame_time;

    void processInput();
    void update(float delta_time);
    void render();
    void wrapInPlay(GameEntity& entity);
};
