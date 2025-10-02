// src/logic/Game.cpp
#include "../../include/Game.hpp"
#include "../../include/UICommon.hpp"
#include "../../include/AsciiLegend.hpp"
#include "../../include/Concurrency.hpp"

#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <random>
#include <string>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>


using namespace ascii;
using namespace UICommon;
using namespace std;

static inline float deg2rad(float d){ return d * 3.1415926535f / 180.0f; }

namespace {
    inline string sym(char c) { return string(1, c); }
    inline string sym(const char* s) { return string(s); }
    inline string sym(const string& s) { return s; }
}

Game::Game(int width, int height, const GameConfig& cfgIn): width(width), height(height), buffer(width, height), cfg(cfgIn), last_frame_time(chrono::steady_clock::now()) {

    pthread_mutex_init(&state_mx, nullptr);

    layout.compute(width, height);

    float cx = layout.play.x + layout.play.w * 0.5f;
    float cy = layout.play.y + layout.play.h * 0.5f;
    
    // Crear naves y establecer vidas
    ships.emplace_back(cx - 5.f, cy, 0.f, 0.f, sym(ShipUp), EntityKind::Ship1, 1.0f, -deg2rad(90.f));
    if (cfg.players == 2)
        ships.emplace_back(cx + 5.f, cy, 0.f, 0.f, sym(ShipUp), EntityKind::Ship2, 1.0f, -deg2rad(90.f));

    hud.st.p1Lives = cfg.livesPerPlayer;
    hud.st.p2Lives = (cfg.players == 2 ? cfg.livesPerPlayer : 0);

    input.init();
    initConsoleUTF8();

    spawnInitialAsteroids();

    gs.game = this;
}

/* Bucle principal del juego: Se encarga de calcular delta_time, extraer el input acumulado por el hilo de entrada y lo aplica.
 * Si no esta pausado/finalizado, se actualiza y renderiza el frame. Si finaliza, muestra GameOver.
*/
void Game::run() {
    is_running = true;
    startWorkerThreads();

    while (is_running) {
        auto now = chrono::steady_clock::now();
    float dt = chrono::duration<float>(now - last_frame_time).count();
    last_frame_time = now;

    pthread_mutex_lock(&state_mx);
    frame_dt = dt;
    gs.dt = dt;
    gs.paused.store(paused.load(), std::memory_order_relaxed);
    pthread_mutex_unlock(&state_mx);

    Concurrency::frameSync(gs); // A
    Concurrency::frameSync(gs); // B
    Concurrency::frameSync(gs); // C

    std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 6FP

    if (game_over) {
        is_running = false;
    }
    }

    stopWorkerThreads();
    pthread_mutex_destroy(&state_mx);
    input.shutdown();
}

void Game::startWorkerThreads() {
    int workers = 0;
    workers += 1; // render
    workers += 1; // ship1
    if (cfg.players == 2) workers += 1; // ship2
    workers += 1; // bullets
    workers += 1; // asteroids
    workers += 1; // collisions
    workers += 1; // spawn
    workers += 1; // hilo principal (main game loop)

    Concurrency::initConcurrency(gs, workers);
    gs.running.store(true, memory_order_release);

    pthread_create(&th_input, nullptr, &Game::thInput, this);
    pthread_create(&th_render, nullptr, &Game::thRender, this);
    pthread_create(&th_ship1, nullptr, &Game::thShip1, this);
    if (cfg.players == 2) {
        pthread_create(&th_ship2, nullptr, &Game::thShip2, this);
    }
    pthread_create(&th_bullets, nullptr, &Game::thBullets, this);
    pthread_create(&th_asteroids, nullptr, &Game::thAsteroids, this);
    pthread_create(&th_collisions, nullptr, &Game::thCollisions, this);
    pthread_create(&th_spawn, nullptr, &Game::thSpawn, this);
}

void Game::stopWorkerThreads() {
    gs.running.store(false, memory_order_release);
    Concurrency::frameSync(gs);
    Concurrency::frameSync(gs);
    Concurrency::frameSync(gs);

    if (th_input) pthread_join(th_input, nullptr);
    if (th_render) pthread_join(th_render, nullptr);
    if (th_ship1) pthread_join(th_ship1, nullptr);
    if (th_ship2) pthread_join(th_ship2, nullptr);
    if (th_bullets) pthread_join(th_bullets, nullptr);
    if (th_asteroids) pthread_join(th_asteroids, nullptr);
    if (th_collisions) pthread_join(th_collisions, nullptr);
    if (th_spawn) pthread_join(th_spawn, nullptr);

    th_input = th_render = th_ship1 = th_ship2 = th_bullets = th_asteroids = th_collisions = th_spawn = {};

    Concurrency::destroyConcurrency(gs);
}

void* Game::thInput(void* arg) {
    Game* g = static_cast<Game*>(arg);
    while (g->gs.running.load(std::memory_order_acquire)) {
        pthread_mutex_lock(&g -> state_mx);
        g -> processInput();
        pthread_mutex_unlock(&g -> state_mx);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return nullptr;
}

void* Game::thRender(void* arg) {
    Game* g = static_cast<Game*>(arg);
    while (g->gs.running.load(std::memory_order_acquire)) {
        Concurrency::frameSync(g->gs); // A
        Concurrency::frameSync(g->gs); // B
        pthread_mutex_lock(&g->state_mx);
        g->render();
        pthread_mutex_unlock(&g->state_mx);
        Concurrency::frameSync(g->gs); // C
    }
    return nullptr;
}

void* Game::thShip1(void* arg) {
    Game* g = static_cast<Game*>(arg);
    while (g->gs.running.load(std::memory_order_acquire)) {
        Concurrency::frameSync(g->gs); // A
        if (!g->paused && !g->game_over) {
            pthread_mutex_lock(&g->state_mx);
            float dt = g->frame_dt;
            g->updateShips(0, dt);
            pthread_mutex_unlock(&g->state_mx);
        }
        Concurrency::frameSync(g->gs); // B
        Concurrency::frameSync(g->gs); // C
    }
    return nullptr;
}

void* Game::thShip2(void* arg) {
    Game* g = static_cast<Game*>(arg);
    while (g->gs.running.load(std::memory_order_acquire)) {
        Concurrency::frameSync(g->gs); // A
        if (!g->paused && !g->game_over) {
            pthread_mutex_lock(&g->state_mx);
            float dt = g->frame_dt;
            bool hasP2 = (g->ships.size() > 1);
            if (hasP2) g->updateShips(1, dt);
            pthread_mutex_unlock(&g->state_mx);
        }
        Concurrency::frameSync(g->gs); // B
        Concurrency::frameSync(g->gs); // C
    }
    return nullptr;
}

void* Game::thBullets(void* arg) {
    Game* g = static_cast<Game*>(arg);
    while (g->gs.running.load(std::memory_order_acquire)) {
        Concurrency::frameSync(g -> gs); // A
        if (!g -> paused && !g -> game_over) {
            float dt;
            pthread_mutex_lock(&g -> state_mx);
            dt = g -> frame_dt;
            g -> updateBullets(dt);
            pthread_mutex_unlock(&g -> state_mx);
        }
        Concurrency::frameSync(g->gs); // B
        Concurrency::frameSync(g->gs); // C
    }
    return nullptr;
}

void* Game::thAsteroids(void* arg) {
    Game* g = static_cast<Game*>(arg);
    while (g->gs.running.load(std::memory_order_acquire)) {
        Concurrency::frameSync(g -> gs); // A
        if (!g -> paused && !g -> game_over) {
            float dt;
            pthread_mutex_lock(&g -> state_mx);
            dt = g -> frame_dt;
            g -> updateAsteroids(dt);
            pthread_mutex_unlock(&g -> state_mx);
        }
        Concurrency::frameSync(g -> gs); // B
        Concurrency::frameSync(g -> gs); // C
    }
    return nullptr;
}

void* Game::thCollisions(void* arg) {
    Game* g = static_cast<Game*>(arg);
    while (g->gs.running.load(std::memory_order_acquire)) {
        Concurrency::frameSync(g -> gs); // A
        Concurrency::frameSync(g -> gs); // B
        if (!g -> paused && !g -> game_over) {
            pthread_mutex_lock(&g -> state_mx);
            g -> handleCollisions();
            g -> checkGameOver();
            pthread_mutex_unlock(&g -> state_mx);
        }
        Concurrency::frameSync(g -> gs); // C
    }
    return nullptr;
}

void* Game::thSpawn(void* arg) {
    Game* g = static_cast<Game*>(arg);
    std::uniform_real_distribution<float> U01(0.f, 1.f);
    float accum = 0.f;
    while (g->gs.running.load(std::memory_order_acquire)) {
        Concurrency::frameSync(g -> gs); // A
        float dt;
        pthread_mutex_lock(&g -> state_mx);
        dt = g -> frame_dt;
        pthread_mutex_unlock(&g -> state_mx);

        Concurrency::frameSync(g -> gs); // B

        if (!g -> paused && !g -> game_over) {
            accum += dt;
            if (accum > 0.12f) {
                accum = 0.f;
                pthread_mutex_lock(&g -> state_mx);
                if ((int)g -> asteroids.size() < std::max(2, g -> cfg.largeAsteroids / 2)) {
                    int side = (int)floor(U01(g->rng) * 4.f) % 4;
                    float x = 0,y = 0;
                    if (side == 0) {
                        x = g -> layout.play.x + 1;
                        y = g -> layout.play.y + U01(g -> rng)*(g -> layout.play.h - 2);
                    }
                    if (side == 1) {
                        x = g -> layout.play.x + g -> layout.play.w - 2;
                        y = g -> layout.play.y + U01(g -> rng) * (g -> layout.play.h - 2);
                    }
                    if (side == 2) {
                        y = g -> layout.play.y + 1;
                        x = g -> layout.play.x + U01(g -> rng) * (g -> layout.play.w - 2);
                    }
                    if (side == 3) {
                        y = g -> layout.play.y + g -> layout.play.h - 2;
                        x = g -> layout.play.x + U01(g -> rng) * (g -> layout.play.w - 2);
                    }
                    if (!g -> inSafeZone(x, y, 8.f)) {
                        std::uniform_real_distribution<float> ang(0.f, 2.f*3.1415926535f);
                        std::uniform_real_distribution<float> v(10.f, 14.f);
                        float a = ang(g -> rng), sp = v(g -> rng);
                        float vx = cos(a) * sp, vy = sin(a) * sp;
                        g -> asteroids.emplace_back(x, y, vx, vy, sym(BigAst), EntityKind::AstBig, 2.0f);
                    }
                }
                pthread_mutex_unlock(&g->state_mx);
            }
        }
        Concurrency::frameSync(g -> gs); // C
    }
    return nullptr;
}

void Game::processInput() {
    InputState st = input.poll();

    if (st.quit) { is_running = false; return; }
    if (st.pause) paused = !paused;
    if (st.newround) {
        hud.st.p1Score = hud.st.p2Score = 0;
        hud.st.p1Lives = cfg.livesPerPlayer;
        hud.st.p2Lives = (cfg.players == 2 ? cfg.livesPerPlayer : 0);
        game_over = false;
        player1_won = false;
        player2_won = false;

        for (auto& s: ships) {
            s.x = layout.play.x + layout.play.w / 2.f;
            s.y = layout.play.y + layout.play.h / 2.f;
            s.vx = s.vy = 0.f;
            s.angle = -deg2rad(90.f);
            s.sprite = ShipUp;
        }
        bullets.clear();
        asteroids.clear();
        spawnInitialAsteroids();
    }
    last_input = st;
}

void Game::checkGameOver() {
    if (asteroids.empty()) {
        game_over = true;
        if (cfg.players == 1) {
            player1_won = true;
        } else {
            player1_won = (hud.st.p1Score >= hud.st.p2Score);
            player2_won = (hud.st.p2Score > hud.st.p1Score);
        }
        return;
    }
    if (hud.st.p1Lives <= 0) {
        game_over = true;
        if (cfg.players == 2) player2_won = true;
        return;
    }
    if (cfg.players == 2 && hud.st.p2Lives <= 0) {
        game_over = true;
        player1_won = true;
        return;
    }
}

void Game::saveScore(const string& playerName, int score) {
    ofstream file("scores.csv", ios::app);
    if (!file.is_open()) {
        std::ofstream newFile("scores.csv");
        newFile << "Nombre,Puntuacion,Fecha\n";
        newFile.close();
        file.open("scores.csv", ios::app);
    }
    auto now = chrono::system_clock::now();
    auto tt = chrono::system_clock::to_time_t(now);
    auto tm = *localtime(&tt);
    char dateStr[100];
    std::strftime(dateStr, sizeof(dateStr), "%Y-%m-%d %H:%M:%S", &tm);
    file << playerName << "," << score << "," << dateStr << "\n";
    file.close();
}

void Game::updateShips(size_t i, float dt) {
    if (i >= ships.size()) return;
    auto& s = ships[i];

    const float ROT = deg2rad(180.f);
    const float THRUST = 40.f;
    const float BRAKE = 0.80f;
    const float MAXS = 60.f;

    bool L = false, R = false, T = false, B = false, F = false;
    if (i==0) {
        L = last_input.p1_left;  R = last_input.p1_right;
        T = last_input.p1_thrust;B = last_input.p1_brake;
        F = last_input.p1_fire;
        if (F && p1_cd<=0.f) { fireBullet(0); p1_cd = 0.25f; }
    } else {
        L = last_input.p2_left;
        R = last_input.p2_right;
        T = last_input.p2_thrust;
        B = false;
        F = last_input.p2_fire;
        if (F && p2_cd<=0.f) { fireBullet(1); p2_cd = 0.25f; }
    }

    if (L) s.angle -= ROT * dt;
    if (R) s.angle += ROT * dt;

    if (T) {
        s.vx += std::cos(s.angle) * THRUST * dt;
        s.vy += std::sin(s.angle) * THRUST * dt;
    }
    if (B) { s.vx *= BRAKE; s.vy *= BRAKE; }

    float speed = std::sqrt(s.vx*s.vx + s.vy*s.vy);
    if (speed > MAXS) {
        float k = MAXS / speed;
        s.vx *= k; s.vy *= k;
    }

    s.update(dt);
    wrapInPlay(s);

    float deg = s.angle * 180.f / 3.1415926535f;
    while (deg < 0) deg += 360;
    while (deg >= 360) deg -= 360;

    if (deg >= 315 || deg < 45) s.sprite = ShipUp;
    else if (deg >= 45 && deg < 135) s.sprite = ShipRight;
    else if (deg >= 135 && deg < 225) s.sprite = ShipDown;
    else s.sprite = ShipLeft;
}

void Game::updateAsteroids(float dt) {
    for (auto& a: asteroids) { a.update(dt); wrapInPlay(a); }
}

void Game::updateBullets(float dt) {
    for (auto& b: bullets) { b.update(dt); wrapInPlay(b); }
    bullets.erase(remove_if(bullets.begin(), bullets.end(), [](const GameEntity& b){ return b.ttl <= 0.f; }), bullets.end());
}

void Game::handleCollisions() {
    // balas vs asteroides
    for (size_t bi=0; bi<bullets.size();) {
        bool hit = false;
        for (size_t ai=0; ai<asteroids.size(); ++ai) {
            auto& b = bullets[bi];
            auto& a = asteroids[ai];
            float dx = a.x - b.x, dy = a.y - b.y;
            float r = a.radius + b.radius;
            if (dx*dx + dy*dy <= r*r) {
                hit = true;
                if (b.kind==EntityKind::Bullet1) hud.st.p1Score += cfg.scorePerSmall;
                else hud.st.p2Score += cfg.scorePerSmall;
                splitAsteroid(ai);
                break;
            }
        }
        if (hit) bullets.erase(bullets.begin()+bi);
        else ++bi;
    }

    // naves vs asteroides
    for (size_t si=0; si<ships.size(); ++si) {
        auto& s = ships[si];
        for (auto& a : asteroids) {
            float dx = a.x - s.x, dy = a.y - s.y;
            float R  = a.radius + s.radius;
            if (dx*dx + dy*dy <= R*R) {
                if (si==0 && hud.st.p1Lives>0) --hud.st.p1Lives;
                if (si==1 && hud.st.p2Lives>0) --hud.st.p2Lives;
                s.x = layout.play.x + layout.play.w*0.5f;
                s.y = layout.play.y + layout.play.h*0.5f;
                s.vx = s.vy = 0.f;
                s.angle = -deg2rad(90.f);
                s.sprite = ShipUp;
            }
        }
    }
}

void Game::splitAsteroid(size_t idx) {
    auto a = asteroids[idx];
    asteroids.erase(asteroids.begin()+idx);

    if (a.kind == EntityKind::AstBig) {
        spawnAsteroid(a.x, a.y, EntityKind::AstMed, 18.f);
        spawnAsteroid(a.x, a.y, EntityKind::AstMed, 18.f);
    } else if (a.kind == EntityKind::AstMed) {
        spawnAsteroid(a.x, a.y, EntityKind::AstSmall, 24.f);
        spawnAsteroid(a.x, a.y, EntityKind::AstSmall, 24.f);
    } else {
        // Se borra
    }
}

void Game::spawnInitialAsteroids() {
    uniform_real_distribution<float> px(layout.play.x+2, layout.play.x + layout.play.w-3);
    uniform_real_distribution<float> py(layout.play.y+2, layout.play.y + layout.play.h-3);

    for (int i=0;i<cfg.largeAsteroids;i++) {
        spawnAsteroid(px(rng), py(rng), EntityKind::AstBig, 12.f);
    }
}

void Game::spawnAsteroid(float x,float y, EntityKind k, float speed) {
    uniform_real_distribution<float> ang(0.f, 2.f*3.1415926535f);
    float a = ang(rng);
    float vx = cos(a)*speed, vy = sin(a)*speed;

    string spr = sym(SmallAst);
    float r = 1.f;
    if (k == EntityKind::AstBig) { spr = sym(BigAst); r = 2.0f; }
    if (k == EntityKind::AstMed) { spr = sym(MedAst); r = 1.5f; }
    if (k == EntityKind::AstSmall) { spr = sym(SmallAst); r = 1.0f; }

    asteroids.emplace_back(x, y, vx, vy, spr, k, r);
}

bool Game::inSafeZone(float x, float y, float safeR) const {
    for (auto const& s : ships) {
        float dx = x - s.x, dy = y - s.y;
        if (dx*dx + dy*dy <= safeR*safeR) return true;
    }
    return false;
}

void Game::fireBullet(int playerIdx) {
    if (playerIdx < 0 || playerIdx >= (int)ships.size()) return;
    auto& s = ships[playerIdx];
    float spd = 80.f;
    float bx = s.x + cos(s.angle)*1.5f;
    float by = s.y + sin(s.angle)*1.5f;
    float bvx = s.vx + cos(s.angle)*spd;
    float bvy = s.vy + sin(s.angle)*spd;

    EntityKind bk = (playerIdx==0) ? EntityKind::Bullet1 : EntityKind::Bullet2;
    bullets.emplace_back(bx, by, bvx, bvy, sym(Bullet), bk, 0.4f, 0.f, 1.2f);
}

void Game::wrapInPlay(GameEntity& e) {
    float minx = layout.play.x;
    float maxx = layout.play.x + layout.play.w - 1;
    float miny = layout.play.y;
    float maxy = layout.play.y + layout.play.h - 1;

    if (e.x < minx) e.x = maxx;
    if (e.x > maxx) e.x = minx;
    if (e.y < miny) e.y = maxy;
    if (e.y > maxy) e.y = miny;
}

void Game::render() {
    clearScreen();
    buffer.clear(' ');

    layout.compute(width, height);

    // Entidades
    for (const auto& a : asteroids) a.draw(buffer);
    for (const auto& s : ships) s.draw(buffer);
    for (const auto& b : bullets) b.draw(buffer);

    // HUD
    hud.draw(layout, cfg, buffer);
    layout.drawFrames(buffer);
    
    if (paused) {
        string p = "[PAUSED]";
        int px = layout.play.x + (layout.play.w - (int)p.size())/2;
        int py = layout.play.y + layout.play.h/2;
        buffer.drawText(px, py, p);
    }

    buffer.present();
}