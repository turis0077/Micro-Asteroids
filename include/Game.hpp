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
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

class Game {
public:
    Game(int width, int height, const GameConfig& cfg);
    void run();

private:
    int width, height;
    std::atomic<bool> is_running{false};
    std::atomic<bool> paused{false};
    std::atomic<bool> game_over{false};
    std::atomic<bool> player1_won{false};
    std::atomic<bool> player2_won{false};

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

    // Threading components
    std::mutex game_mutex;
    std::condition_variable cv;
    std::thread input_thread;
    std::thread render_thread;
    std::thread ship1_thread;
    std::thread ship2_thread;

    // Thread functions
    void inputThreadFunc();
    void renderThreadFunc();
    void ship1ThreadFunc();
    void ship2ThreadFunc();

    // Game logic methods
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

    // Win/Loss conditions and score persistence
    void checkGameOver();
    void showGameOverScreen();
    void saveScore(const std::string& playerName, int score);
};
