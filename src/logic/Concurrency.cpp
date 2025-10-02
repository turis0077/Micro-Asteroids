// include/Concurrency.cpp
#include "../../include/Concurrency.hpp"

namespace Concurrency {
    bool initConcurrency(GameState& gs, int workers) {
        gs.workers = workers;
        gs.useBarrier = false;
        gs.useBarrier = false;
        gs.readyCount = 0;

        pthread_mutex_init(&gs.tickMx, nullptr);
        pthread_cond_init(&gs.tickCv, nullptr);

        if (pthread_barrier_init(&gs.frameBarrier, nullptr, workers) == 0) {
            gs.useBarrier = true;
        } else {
            gs.useBarrier = false;
        }
        return true;
    }

    void destroyConcurrency(GameState& gs) {
        if (gs.useBarrier) {
            pthread_barrier_destroy(&gs.frameBarrier);
        }
        pthread_cond_destroy(&gs.tickCv);
        pthread_mutex_destroy(&gs.tickMx);
    }

    void frameSync(GameState& gs) {
        if (gs.useBarrier) {
            pthread_barrier_wait(&gs.frameBarrier);
            return;
        }
        // Barrera manual con condvar
        pthread_mutex_lock(&gs.tickMx);
        gs.readyCount++;
        if (gs.readyCount >= gs.workers) {
            gs.readyCount = 0;
            pthread_cond_broadcast(&gs.tickCv);
        } else {
            pthread_cond_wait(&gs.tickCv, &gs.tickMx);
        }
        pthread_mutex_unlock(&gs.tickMx);
    }
}