#pragma once
#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <deque>

struct Body {
    std::string  name;
    double       mass;           // kg (SI)
    Vec2         pos;            // metres (SI)
    Vec2         vel;            // m/s
    Vec2         acc;            // m/s² — stored after each step for the UI
    sf::Color    color;
    bool         isFixed = false; // true → not moved by gravity (e.g. galactic centres)
    bool         isStar  = false; // true → rendered with glow halo

    static constexpr int MAX_TRAIL = 500;
    std::deque<Vec2> trail;       // position history in sim coords

    Body() = default;
    Body(std::string name, double mass, Vec2 pos, Vec2 vel,
         sf::Color color, bool isFixed = false, bool isStar = false);

    void  pushTrail();
    void  clearTrail();
    float visualRadius() const;  // log-scaled screen radius
};
