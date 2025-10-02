// include/Game.hpp
#pragma once
#include "RenderBuffer.hpp"
#include "GameEntity.hpp"
#include "Config.hpp"
#include "Layout.hpp"
#include "HUD.hpp"
#include "Input.hpp"
#include "Concurrency.hpp"

#include <pthread.h>
#include <mutex>
#include <vector>
#include <chrono>
#include <random>
#include <atomic>
#include <condition_variable>
#include <array>

class Game {
public:
    Game(int width, int height, const GameConfig& cfg);
    void run();

    inline bool isGameOver() const { return game_over.load(); }
    inline bool player1Won() const { return player1_won.load(); }
    inline bool player2Won() const { return player2_won.load(); }
    inline int  playersCount() const { return cfg.players; }
    inline int  p1Score() const { return hud.st.p1Score; }
    inline int  p2Score() const { return hud.st.p2Score; }
    inline int  p1Lives() const { return hud.st.p1Lives; }
    inline int  p2Lives() const { return hud.st.p2Lives; }
    void saveScore(const std::string& playerName, int score);


private:
    int width, height;

    // Estado de ejecución
    std::atomic<bool> is_running{false};
    std::atomic<bool> paused{false};
    std::atomic<bool> game_over{false};
    std::atomic<bool> player1_won{false};
    std::atomic<bool> player2_won{false};

    // Nucleo del juego
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

    // Sincronización
    pthread_mutex_t state_mx{};
    Concurrency::GameState gs{};
    float frame_dt{0.f};

    // hilos
    pthread_t th_input{};
    pthread_t th_render{};
    pthread_t th_ship1{};
    pthread_t th_ship2{};
    pthread_t th_bullets{};
    pthread_t th_asteroids{};
    pthread_t th_collisions{};
    pthread_t th_spawn{};

    // Inicio y detnecion de hilos
    void startWorkerThreads();
    void stopWorkerThreads();

    // Entradas
    static void* thInput(void* arg);
    static void* thRender(void* arg);
    static void* thShip1(void* arg);
    static void* thShip2(void* arg);
    static void* thBullets(void* arg);
    static void* thAsteroids(void* arg);
    static void* thCollisions(void* arg);
    static void* thSpawn(void* arg);

    // Lógica comun del juego
    void processInput();
    void fireBullet(int playerIdx);
    void wrapInPlay(GameEntity& e);
    void render();

    // Helpers
    void updateShips(size_t idx, float dt);
    void updateAsteroids(float dt);
    void updateBullets(float dt);
    void handleCollisions();

    // Spawn y split
    void spawnInitialAsteroids();
    void spawnAsteroid(float x,float y, EntityKind k, float speed=12.0f);
    void splitAsteroid(size_t idx);

    // Win/Loss y scores
    void checkGameOver();
    void showGameOverScreen();

    // spawn seguro
    bool inSafeZone(float x, float y, float safeR) const;
};
