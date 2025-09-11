#pragma once
#include "RenderBuffer.hpp"

class GameEntity {
public:
    float x, y;
    float vx, vy;
    char display_char;
    bool is_active;

    GameEntity(float x, float y, float vx, float vy, char dc)
        : x(x), y(y), vx(vx), vy(vy), display_char(dc), is_active(true) {}

    virtual ~GameEntity() = default;

    void update(float delta_time) {
        if (!is_active) return;
        x += vx * delta_time;
        y += vy * delta_time;
    }

    void draw(RenderBuffer& buffer) const {
        if (!is_active) return;
        buffer.set(static_cast<int>(x), static_cast<int>(y), display_char);
    }
};
