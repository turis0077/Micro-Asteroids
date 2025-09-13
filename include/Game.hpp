// include/Game.hpp

#pragma once
#include "RenderBuffer.hpp"
#include "GameEntity.hpp"
#include <vector>
#include <chrono>

class Game {
public:
    Game(int width, int height);
    void run();

private:
    void processInput();
    void update(float delta_time);
    void render();

    int width, height;
    bool is_running;
    RenderBuffer buffer;
    std::vector<GameEntity> entities;
    std::chrono::steady_clock::time_point last_frame_time;
};
