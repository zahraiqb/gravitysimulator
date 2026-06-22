#pragma once
#include "Simulation.h"
#include "Renderer.h"
#include "Presets.h"
#include <vector>

class UI {
public:
    bool showAddBody = false;

    void init();
    void render(Simulation& sim, Renderer& renderer);

private:
    std::vector<Preset> presets;
    int selectedPreset = 0;
    int selectedBody   = 0;

    // Add-body form
    char  bodyName[64] = "New Body";
    float massExp      = 24.f;  // mass = 10^massExp kg
    float posX=0.f,posY=0.f;   // AU
    float velX=0.f,velY=0.f;   // km/s
    float colR=200.f,colG=200.f,colB=200.f;

    void panelControls (Simulation& sim, Renderer& renderer);
    void panelBodyList (Simulation& sim);
    void windowAddBody (Simulation& sim);
    void overlayStats  (const Simulation& sim);
    void loadPreset    (int idx, Simulation& sim, Renderer& renderer);
};
