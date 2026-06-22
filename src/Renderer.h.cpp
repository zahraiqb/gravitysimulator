#pragma once
#include "Simulation.h"
#include <SFML/Graphics.hpp>

class Renderer {
public:
    // View state — public so UI can read/display them
    double viewScale  = 3e8;   // metres per pixel
    Vec2   viewCenter = {0,0}; // sim-space coords at screen centre

    // Feature toggles
    bool showMesh    = true;
    bool showTrails  = true;
    bool showGlow    = true;
    bool showLabels  = true;
    bool showLensing = true;

    void init(unsigned int W, unsigned int H);
    void draw(sf::RenderWindow& window, const Simulation& sim);
    void setViewForPreset(const std::string& presetName);

    sf::Vector2f toScreen(Vec2 simPos)        const;
    Vec2         toSim   (sf::Vector2f screen) const;

    void pan (sf::Vector2f delta);
    void zoom(float factor, sf::Vector2f pivotScreen);

private:
    unsigned int W = 0, H = 0;
    bool fontLoaded = false;
    sf::Font font;
    sf::Shader lensingShader;
    bool shaderAvailable = false;
    sf::RenderTexture backgroundRT;
    sf::VertexArray   starfield;

    void generateStarfield();
    void drawSpacetimeMesh(sf::RenderTarget& rt, const std::vector<Body>& bodies);
    void drawTrails       (sf::RenderTarget& rt, const std::vector<Body>& bodies);
    void drawBodies       (sf::RenderTarget& rt, const std::vector<Body>& bodies);
    void drawGlow         (sf::RenderTarget& rt, const Body& b, sf::Vector2f sp);
    void drawTimeDilation (sf::RenderTarget& rt, const std::vector<Body>& bodies);
    void applyLensing     (sf::RenderWindow& win,const std::vector<Body>& bodies);

    sf::Color meshColor(float intensity);
};
