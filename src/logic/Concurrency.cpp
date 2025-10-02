#include "../../include/Concurrency.hpp"

namespace Concurrency {
    bool initConcurrency(GameState& gs, int workers) {
        gs.workers   = workers;
        gs.useBarrier = false;

        if (pthread_barrier_init(&gs.frameBarrier, nullptr, workers) == 0) {
            gs.useBarrier = true;
            return true;
        }

        gs.useBarrier = false;
        gs.readyCount = 0;
        return true;
    }

    void destroyConcurrency(GameState& gs) {
        if (gs.useBarrier) {
            pthread_barrier_destroy(&gs.frameBarrier);
        }
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

    static void* generic_worker(GameState* gs) {
        while (gs->running.load(std::memory_order_acquire)) {
            frameSync(*gs);
        }
        return nullptr;
    }
}