#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>

#include "Simulation.h"
#include "Renderer.h"
#include "UI.h"
#include "Presets.h"

int main() {
    constexpr unsigned W=1400, H=900;

    sf::RenderWindow window(sf::VideoMode(W,H),"Gravity Simulator — C++",sf::Style::Default);
    window.setFramerateLimit(60);

    if (!ImGui::SFML::Init(window)) {
        std::cerr << "ImGui-SFML init failed\n";
        return 1;
    }
    ImGui::StyleColorsDark();
    ImGui::GetStyle().WindowRounding = 6.f;
    ImGui::GetStyle().FrameRounding  = 4.f;
    ImGui::GetIO().FontGlobalScale   = 1.1f;

    Simulation sim;
    Renderer   renderer;
    UI         ui;

    renderer.init(W,H);
    ui.init();

    // Load first preset
    auto presets = getAllPresets();
    for (const auto& b : presets[0].bodies) sim.addBody(b);
    renderer.setViewForPreset(presets[0].name);

    bool         dragging = false;
    sf::Vector2i lastMouse;
    sf::Clock    clock;

    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            ImGui::SFML::ProcessEvent(window,ev);

            if (ev.type==sf::Event::Closed) window.close();

            if (ev.type==sf::Event::KeyPressed) {
                if (ev.key.code==sf::Keyboard::Space) sim.paused=!sim.paused;
                if (ev.key.code==sf::Keyboard::R)
                    for (auto& b:sim.bodies) b.clearTrail();
            }

            if (ev.type==sf::Event::MouseWheelScrolled && !ImGui::GetIO().WantCaptureMouse) {
                float f = (ev.mouseWheelScroll.delta>0) ? 0.85f : 1.18f;
                renderer.zoom(f,{(float)ev.mouseWheelScroll.x,(float)ev.mouseWheelScroll.y});
            }
            if (ev.type==sf::Event::MouseButtonPressed && !ImGui::GetIO().WantCaptureMouse) {
                if (ev.mouseButton.button==sf::Mouse::Middle ||
                    ev.mouseButton.button==sf::Mouse::Right) {
                    dragging=true; lastMouse=sf::Mouse::getPosition(window);
                }
            }
            if (ev.type==sf::Event::MouseButtonReleased) dragging=false;
            if (ev.type==sf::Event::MouseMoved && dragging) {
                sf::Vector2i cur=sf::Mouse::getPosition(window);
                renderer.pan({(float)(cur.x-lastMouse.x),(float)(cur.y-lastMouse.y)});
                lastMouse=cur;
            }
            if (ev.type==sf::Event::Resized) {
                window.setView(sf::View(sf::FloatRect(0,0,(float)ev.size.width,(float)ev.size.height)));
            }
        }

        sf::Time dt=clock.restart();
        ImGui::SFML::Update(window,dt);

        sim.step(dt.asSeconds());
        ui.render(sim,renderer);

        window.clear(sf::Color(3,3,12));
        renderer.draw(window,sim);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
