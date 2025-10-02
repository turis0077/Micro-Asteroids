#pragma once

#include <pthread.h>
#include <atomic>

class Game;

namespace Concurrency {
    struct GameState { 
        std::atomic<bool> running{false};
        std::atomic<bool> paused{false};

        float dt{0.0f}; // delta de tiempo entre frames

        Game* game{nullptr}; // puntero de acceso al juego

        pthread_mutex_t stateMx = PTHREAD_MUTEX_INITIALIZER;

        // Sincronizacion por frame
        bool useBarrier{false};
        pthread_barrier_t frameBarrier;
        
        // Fallback por si barrier no es soportado
        pthread_cond_t  tickCv = PTHREAD_COND_INITIALIZER;
        pthread_mutex_t tickMx = PTHREAD_MUTEX_INITIALIZER;
        int readyCount{0};
        int workers{0};
    };

    bool initConcurrency(GameState& gs, int workers);
    void destroyConcurrency(GameState& gs);
    void frameSync (GameState& gs);

    // prototipos de hilos
    void* inputThread(void* arg);
    void* renderThread(void* arg);
    void* collisionThread(void* arg);
    void* bulletManagerThread(void* arg);
    void* spawnThread(void* arg);
    void* shipThread(void* arg);
    void* asteroidThread(void* arg);
}