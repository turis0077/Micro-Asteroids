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
#include <random>

class Game {
public:
    Game(int width, int height, const GameConfig& cfg);
    void run();

private:
    int width, height;
    bool is_running = false;
    bool paused = false;

    RenderBuffer buffer;
    GameConfig cfg;
    Layout layout;
    HUD hud;
    Input input;

    std::vector<GameEntity> ships;
    std::vector<GameEntity> asteroids;
    std::vector<GameEntity> bullets;

    InputState last_input{};
    float p1_cd{0.f}, p2_cd{0.f};

    std::mt19937 rng{12345};

    std::chrono::steady_clock::time_point last_frame_time;

    void processInput();
    void update(float delta_time);
    void render();
    void wrapInPlay(GameEntity& entity);

    void spawnInitialAsteroids();
    void spawnAsteroid(float x,float y, EntityKind k, float speed=12.0f);
    void splitAsteroid(size_t idx);
    void fireBullet(int playerIdx);
    void updateShips(float dt);
    void updateAsteroids(float dt);
    void updateBullets(float dt);
    void handleCollisions();
};
