#include "../../include/Game.hpp"
#include "../../include/UICommon.hpp"
#include "../../include/AsciiLegend.hpp"
#include <iostream>

Game::Game(int width, int height)
    : width(width), height(height), is_running(false), buffer(width, height), last_frame_time(std::chrono::steady_clock::now()) {
    entities.emplace_back(width / 2.0f, height / 2.0f, 5.0f, 0.0f, ascii::ShipRight);
}

void Game::run() {
    is_running = true;
    while (is_running) {
        auto current_time = std::chrono::steady_clock::now();
        float delta_time = std::chrono::duration<float>(current_time - last_frame_time).count();
        last_frame_time = current_time;

        processInput();
        update(delta_time);
        render();
    }
}

void Game::processInput() {
    // Placeholder para manejo de entrada.
    // Por ahora, no hace nada.
}

void Game::update(float delta_time) {
    for (auto& entity : entities) {
        entity.update(delta_time);
        // Lógica para que las entidades aparezcan por el otro lado de la pantalla
        if (entity.x < 0) entity.x = width - 1;
        if (entity.x >= width) entity.x = 0;
        if (entity.y < 0) entity.y = height - 1;
        if (entity.y >= height) entity.y = 0;
    }
}

void Game::render() {
    UICommon::clearScreen();
    buffer.clear();
    for (const auto& entity : entities) {
        entity.draw(buffer);
    }
    buffer.present();
}
