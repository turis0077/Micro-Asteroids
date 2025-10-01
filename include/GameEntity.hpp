// include/GameEntity.hpp
#pragma once
#include <string>
#include <cmath>
#include "RenderBuffer.hpp"

enum class EntityKind { Ship1, Ship2, AstBig, AstMed, AstSmall, Bullet1, Bullet2 };

struct GameEntity {
    float x{}, y{}, vx{}, vy{};
    float angle{0.0f};
    float radius{1.0f};
    float ttl{-1.0f};
    std::string sprite;
    EntityKind kind{EntityKind::AstSmall};

    GameEntity() = default;

    GameEntity(float X, float Y, float VX, float VY, const std::string& spr, EntityKind k,
               float R=1.0f, float Ang=0.0f, float TTL=-1.0f): x(X), y(Y), vx(VX), vy(VY),
        angle(Ang), radius(R), ttl(TTL), sprite(spr), kind(k) {}

    inline void update(float delta_time) {
        x += vx * delta_time;
        y += vy * delta_time;
        if (ttl > 0.0f) ttl -= delta_time;
    }

    inline void draw(RenderBuffer& buffer) const {
        buffer.drawText((int)std::lround(x), (int)std::lround(y), sprite);
    }
};
