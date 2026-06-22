#include "Renderer.h"
#include <cmath>
#include <algorithm>
#include <random>
#include <iostream>

// ── Init ─────────────────────────────────────────────────────────────────────

void Renderer::init(unsigned int w, unsigned int h) {
    W = w; H = h;

    // Try bundled font, fall back to common system fonts
    if (!font.loadFromFile("assets/fonts/Roboto-Regular.ttf")) {
        fontLoaded = font.loadFromFile("C:/Windows/Fonts/Arial.ttf") ||
                     font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") ||
                     font.loadFromFile("/System/Library/Fonts/Helvetica.ttc");
    } else {
        fontLoaded = true;
    }

    if (!fontLoaded)
        std::cerr << "[Renderer] No font found — labels disabled.\n";

    shaderAvailable = sf::Shader::isAvailable();
    if (shaderAvailable) {
        if (!lensingShader.loadFromFile("assets/shaders/lensing.frag", sf::Shader::Fragment)) {
            std::cerr << "[Renderer] Could not load lensing.frag\n";
            shaderAvailable = false;
        }
    } else {
        std::cerr << "[Renderer] Shaders not supported — lensing disabled.\n";
        showLensing = false;
    }

    backgroundRT.create(W, H);
    generateStarfield();
}

void Renderer::generateStarfield() {
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> xd(0.f,(float)W), yd(0.f,(float)H);
    std::uniform_int_distribution<int>    bd(120,255);
    std::uniform_real_distribution<float> sd(0.5f,2.2f);

    const int N = 700;
    starfield.setPrimitiveType(sf::Quads);
    starfield.resize(N * 4);
    for (int i = 0; i < N; ++i) {
        float cx=xd(rng), cy=yd(rng), s=sd(rng);
        sf::Color c(bd(rng),bd(rng),bd(rng),180);
        starfield[i*4+0]=sf::Vertex({cx-s,cy-s},c);
        starfield[i*4+1]=sf::Vertex({cx+s,cy-s},c);
        starfield[i*4+2]=sf::Vertex({cx+s,cy+s},c);
        starfield[i*4+3]=sf::Vertex({cx-s,cy+s},c);
    }
}

// ── Main draw ─────────────────────────────────────────────────────────────────

void Renderer::draw(sf::RenderWindow& window, const Simulation& sim) {
    // 1. Render starfield + spacetime mesh into backgroundRT
    backgroundRT.clear(sf::Color(3,3,12));
    backgroundRT.draw(starfield);
    if (showMesh) drawSpacetimeMesh(backgroundRT, sim.bodies);
    backgroundRT.display();

    // 2. Draw background to window (with lensing shader if available)
    if (showLensing && shaderAvailable)
        applyLensing(window, sim.bodies);
    else {
        sf::Sprite bg(backgroundRT.getTexture());
        window.draw(bg);
    }

    // 3. Draw on top of lensed background
    drawTimeDilation(window, sim.bodies);
    if (showTrails) drawTrails(window, sim.bodies);
    drawBodies(window, sim.bodies);
}

// ── Spacetime Mesh ────────────────────────────────────────────────────────────

sf::Color Renderer::meshColor(float t) {
    t = std::clamp(t, 0.f, 1.f);
    // Blue → teal → white → orange as warp intensity increases
    if (t < 0.4f) {
        float s = t / 0.4f;
        return sf::Color((uint8_t)(10+s*10), (uint8_t)(80+s*120), (uint8_t)(180-s*30), (uint8_t)(55+s*85));
    } else if (t < 0.75f) {
        float s = (t-0.4f)/0.35f;
        return sf::Color((uint8_t)(20+s*200),(uint8_t)(200+s*55),(uint8_t)(150-s*100),(uint8_t)(140+s*60));
    } else {
        float s = (t-0.75f)/0.25f;
        return sf::Color(255,(uint8_t)(255-s*120),(uint8_t)(50-s*40),(uint8_t)(200+s*55));
    }
}

void Renderer::drawSpacetimeMesh(sf::RenderTarget& rt, const std::vector<Body>& bodies) {
    const int COLS=48, ROWS=32;
    float cw=(float)W/COLS, ch=(float)H/ROWS;

    // Precompute per-body screen influence
    struct Inf { sf::Vector2f sp; float nm; };
    std::vector<Inf> inf;
    for (const auto& b : bodies) {
        float nm = (float)(std::log10(std::max(b.mass,1.0)) / 37.0);
        inf.push_back({ toScreen(b.pos), nm });
    }

    // Returns {warped position, warp intensity 0-1} for grid vertex (c,r)
    auto getVertex = [&](int c, int r) -> std::pair<sf::Vector2f,float> {
        float bx = c*cw, by = r*ch;
        float wx=0, wy=0, totalW=0;
        for (const auto& i : inf) {
            float dx=bx-i.sp.x, dy=by-i.sp.y;
            float distSq = dx*dx + dy*dy + 300.f;
            float str = i.nm * 9000.f / distSq;
            wx -= dx*str;  wy -= dy*str;
            totalW += str;
        }
        wx = std::clamp(wx,-90.f,90.f);
        wy = std::clamp(wy,-90.f,90.f);
        return { sf::Vector2f(bx+wx,by+wy), std::clamp(totalW*1.5f,0.f,1.f) };
    };

    sf::VertexArray lines(sf::Lines);

    for (int r=0;r<=ROWS;++r)
        for (int c=0;c<COLS;++c) {
            auto [p0,w0]=getVertex(c,r); auto [p1,w1]=getVertex(c+1,r);
            sf::Color col=meshColor((w0+w1)*0.5f);
            lines.append({p0,col}); lines.append({p1,col});
        }
    for (int c=0;c<=COLS;++c)
        for (int r=0;r<ROWS;++r) {
            auto [p0,w0]=getVertex(c,r); auto [p1,w1]=getVertex(c,r+1);
            sf::Color col=meshColor((w0+w1)*0.5f);
            lines.append({p0,col}); lines.append({p1,col});
        }

    rt.draw(lines);
}

// ── Trails ────────────────────────────────────────────────────────────────────

void Renderer::drawTrails(sf::RenderTarget& rt, const std::vector<Body>& bodies) {
    for (const auto& b : bodies) {
        int n = (int)b.trail.size();
        if (n < 2) continue;
        sf::VertexArray line(sf::LineStrip, n);
        for (int i = 0; i < n; ++i) {
            float alpha = (float)i / n;
            sf::Color c = b.color;
            c.a = (uint8_t)(alpha * alpha * 190.f);
            line[i] = sf::Vertex(toScreen(b.trail[i]), c);
        }
        rt.draw(line);
    }
}

// ── Bodies ────────────────────────────────────────────────────────────────────

void Renderer::drawGlow(sf::RenderTarget& rt, const Body& b, sf::Vector2f sp) {
    float r = b.visualRadius();
    for (int ring = 4; ring >= 1; --ring) {
        float gr = r * ring * 0.85f;
        sf::CircleShape g(gr);
        g.setOrigin(gr,gr);
        g.setPosition(sp);
        sf::Color gc = b.color;
        gc.a = (uint8_t)(22 / ring);
        g.setFillColor(gc);
        rt.draw(g, sf::BlendAdd);
    }
}

void Renderer::drawBodies(sf::RenderTarget& rt, const std::vector<Body>& bodies) {
    for (const auto& b : bodies) {
        sf::Vector2f sp = toScreen(b.pos);
        float r = b.visualRadius();

        if (showGlow && b.isStar) drawGlow(rt, b, sp);

        sf::CircleShape circle(r);
        circle.setOrigin(r,r);
        circle.setPosition(sp);
        circle.setFillColor(b.color);
        rt.draw(circle);

        if (showLabels && fontLoaded) {
            sf::Text lbl(b.name, font, 11);
            lbl.setPosition(sp + sf::Vector2f(r+4.f,-6.f));
            lbl.setFillColor(sf::Color(210,210,210,180));
            rt.draw(lbl);
        }
    }
}

// ── Time Dilation Heat Overlay ────────────────────────────────────────────────

void Renderer::drawTimeDilation(sf::RenderTarget& rt, const std::vector<Body>& bodies) {
    // Subtle red haze near very massive objects — represents gravitational time dilation
    for (const auto& b : bodies) {
        double logM = std::log10(b.mass);
        if (logM < 28.0) continue;
        float norm = (float)std::clamp((logM-28.0)/9.0, 0.0, 1.0);
        float rad  = norm * 220.f + 25.f;
        sf::CircleShape td(rad);
        td.setOrigin(rad,rad);
        td.setPosition(toScreen(b.pos));
        td.setFillColor(sf::Color(200,20,5, (uint8_t)(norm*30)));
        rt.draw(td, sf::BlendAdd);
    }
}

// ── Lensing post-process ──────────────────────────────────────────────────────

void Renderer::applyLensing(sf::RenderWindow& win, const std::vector<Body>& bodies) {
    int n = (int)std::min((int)bodies.size(), 8);
    sf::Vector2f positions[8];
    float        masses[8] = {};

    for (int i = 0; i < n; ++i) {
        positions[i] = toScreen(bodies[i].pos);
        double logM  = std::log10(std::max(bodies[i].mass, 1.0));
        masses[i]    = (float)std::clamp((logM-22.0)/15.0, 0.0, 1.0);
    }

    lensingShader.setUniform("u_texture",    sf::Shader::CurrentTexture);
    lensingShader.setUniform("u_resolution", sf::Vector2f((float)W,(float)H));
    lensingShader.setUniform("u_numBodies",  n);
    lensingShader.setUniformArray("u_bodyPos",  positions, 8);
    lensingShader.setUniformArray("u_bodyMass", masses,    8);

    sf::Sprite bgSprite(backgroundRT.getTexture());
    win.draw(bgSprite, &lensingShader);
}

// ── Coordinate conversion ─────────────────────────────────────────────────────

sf::Vector2f Renderer::toScreen(Vec2 p) const {
    return sf::Vector2f(
        (float)((p.x - viewCenter.x) / viewScale + W * 0.5),
        (float)((p.y - viewCenter.y) / viewScale + H * 0.5)
    );
}
Vec2 Renderer::toSim(sf::Vector2f s) const {
    return Vec2(
        (s.x - W*0.5) * viewScale + viewCenter.x,
        (s.y - H*0.5) * viewScale + viewCenter.y
    );
}
void Renderer::pan(sf::Vector2f delta) {
    viewCenter.x -= delta.x * viewScale;
    viewCenter.y -= delta.y * viewScale;
}
void Renderer::zoom(float factor, sf::Vector2f pivot) {
    Vec2 pSim = toSim(pivot);
    viewScale *= factor;
    viewCenter.x = pSim.x - (pivot.x - W*0.5) * viewScale;
    viewCenter.y = pSim.y - (pivot.y - H*0.5) * viewScale;
}
void Renderer::setViewForPreset(const std::string& name) {
    viewCenter = {0,0};
    if      (name=="Inner Solar System")  viewScale = AU*2.0/(W*0.4);
    else if (name=="Outer Solar System")  viewScale = AU*35.0/(W*0.45);
    else if (name=="Kepler-16 Binary")    viewScale = AU*2.0/(W*0.45);
    else if (name=="Galactic Centre")     viewScale = 2.5e14/(W*0.45);
    else if (name=="Neutron Star Binary") viewScale = AU*0.015/(W*0.45);
    else                                  viewScale = AU*3.0/(W*0.4);
}
