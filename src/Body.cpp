#include "Body.h"
#include <cmath>
#include <algorithm>

Body::Body(std::string name, double mass, Vec2 pos, Vec2 vel,
           sf::Color color, bool isFixed, bool isStar)
    : name(std::move(name)), mass(mass), pos(pos), vel(vel),
      color(color), isFixed(isFixed), isStar(isStar)
{}

void Body::pushTrail() {
    trail.push_back(pos);
    while ((int)trail.size() > MAX_TRAIL)
        trail.pop_front();
}

void Body::clearTrail() { trail.clear(); }

float Body::visualRadius() const {
    // log10(Earth) ≈ 24.8  |  log10(Sun) ≈ 30.3  |  log10(Sgr A*) ≈ 36.9
    double logM = std::log10(std::max(mass, 1.0));
    float r = (float)((logM - 22.0) * 1.8);
    return std::clamp(r, 2.f, 35.f);
}
