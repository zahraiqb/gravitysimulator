#pragma once
#include "Body.h"
#include <vector>
#include <string>
#include <cmath>

static constexpr double AU = 1.496e11;   // 1 Astronomical Unit in metres
static constexpr double MS = 1.989e30;   // 1 Solar Mass in kg
static constexpr double G_ = 6.674e-11;  // gravitational constant

// Circular orbit speed around a central mass M at radius r
static double circVel(double M, double r) {
    return std::sqrt(G_ * M / r);
}

struct Preset {
    std::string        name;
    std::string        description;
    std::vector<Body>  bodies;
};

// ── Preset factories ──────────────────────────────────────────────────────────

inline Preset makeInnerSolarSystem() {
    return {
        "Inner Solar System",
        "Sun + 4 inner planets. Good baseline to see how gravity scales with mass.",
        {
            Body("Sun",     MS,       {0,0},          {0,0},                       sf::Color(255,220,50),  true,  true),
            Body("Mercury", 3.30e23,  {0.387*AU,0},   {0, circVel(MS,0.387*AU)},   sf::Color(180,160,140)),
            Body("Venus",   4.87e24,  {0.723*AU,0},   {0, circVel(MS,0.723*AU)},   sf::Color(220,180,80)),
            Body("Earth",   5.97e24,  {AU,0},         {0, circVel(MS,AU)},         sf::Color(80,140,255)),
            Body("Mars",    6.39e23,  {1.524*AU,0},   {0, circVel(MS,1.524*AU)},   sf::Color(200,80,50)),
        }
    };
}

inline Preset makeOuterSolarSystem() {
    return {
        "Outer Solar System",
        "Gas giants — orbital periods from 12 years (Jupiter) to 248 years (Neptune).",
        {
            Body("Sun",     MS,       {0,0},        {0,0},                    sf::Color(255,220,50),  true,  true),
            Body("Jupiter", 1.898e27, {5.2*AU,0},   {0, circVel(MS,5.2*AU)}, sf::Color(200,160,100)),
            Body("Saturn",  5.683e26, {9.5*AU,0},   {0, circVel(MS,9.5*AU)}, sf::Color(220,200,140)),
            Body("Uranus",  8.681e25, {19.2*AU,0},  {0, circVel(MS,19.2*AU)},sf::Color(150,220,220)),
            Body("Neptune", 1.024e26, {30.1*AU,0},  {0, circVel(MS,30.1*AU)},sf::Color(80,120,220)),
        }
    };
}

inline Preset makeKepler16() {
    // Kepler-16: a real circumbinary system — a planet orbiting two stars
    double mA  = 0.6897 * MS,  mB = 0.2026 * MS;
    double sep = 0.22 * AU,    totalM = mA + mB;

    double xA  = -sep * mB / totalM;
    double xB  =  sep * mA / totalM;
    double vA  = std::sqrt(G_ * mB * mB / (totalM * sep));
    double vB  = std::sqrt(G_ * mA * mA / (totalM * sep));

    double rPlanet = 0.70 * AU;
    double vPlanet = circVel(totalM, rPlanet);

    return {
        "Kepler-16 Binary",
        "Two stars orbiting each other + Kepler-16b circumbinary planet (real system!).",
        {
            Body("Kepler-16A", mA, {xA,0}, {0,-vA}, sf::Color(255,160,60), false, true),
            Body("Kepler-16B", mB, {xB,0}, {0, vB}, sf::Color(200,100,50), false, true),
            Body("Kepler-16b", 1.0e26, {rPlanet,0}, {0, vPlanet}, sf::Color(100,160,255)),
        }
    };
}

inline Preset makeGalacticCentre() {
    // Sagittarius A* — the Milky Way's supermassive black hole
    double sgrMass = 4.15e6 * MS;
    double r1 = 2.0e14; // ~1300 AU — scale of innermost S-stars
    return {
        "Galactic Centre",
        "Sagittarius A* (Milky Way SMBH, 4.15M solar masses) + orbiting S-stars.",
        {
            Body("Sgr A*",    sgrMass, {0,0},         {0,0},               sf::Color(255,60,10),  true, true),
            Body("S2",        15.0*MS, {r1,0},         {0,circVel(sgrMass,r1)},       sf::Color(200,220,255)),
            Body("S14",        7.0*MS, {1.4*r1,0},     {0,circVel(sgrMass,1.4*r1)},   sf::Color(180,200,255)),
            Body("S62",        5.0*MS, {0.55*r1,0},    {0,circVel(sgrMass,0.55*r1)},  sf::Color(220,230,255)),
        }
    };
}

inline Preset makeNeutronStarBinary() {
    double m   = 1.4 * MS;
    double sep = 0.008 * AU;           // ~1.2 million km apart
    double v   = std::sqrt(G_ * m / (2.0 * sep));
    return {
        "Neutron Star Binary",
        "Two 1.4-solar-mass neutron stars — extreme gravity, very fast orbits.",
        {
            Body("Pulsar A", m, {-sep/2, 0}, {0, -v}, sf::Color(150,230,255), false, true),
            Body("Pulsar B", m, { sep/2, 0}, {0,  v}, sf::Color(180,255,230), false, true),
        }
    };
}

inline std::vector<Preset> getAllPresets() {
    return {
        makeInnerSolarSystem(),
        makeOuterSolarSystem(),
        makeKepler16(),
        makeGalacticCentre(),
        makeNeutronStarBinary(),
    };
}
