#include "UI.h"
#include <imgui.h>
#include <imgui-SFML.h>
#include <cmath>

void UI::init() {
    presets = getAllPresets();
}

void UI::render(Simulation& sim, Renderer& renderer) {
    panelControls(sim, renderer);
    panelBodyList(sim);
    overlayStats(sim);
    if (showAddBody) windowAddBody(sim);
}

void UI::panelControls(Simulation& sim, Renderer& renderer) {
    ImGui::SetNextWindowPos ({10,10}, ImGuiCond_Always);
    ImGui::SetNextWindowSize({310,0}, ImGuiCond_Always);
    ImGui::Begin("Gravity Simulator", nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // ── Presets ──────────────────────────────────────────────
    ImGui::SeparatorText("Presets");
    std::vector<const char*> names;
    for (auto& p : presets) names.push_back(p.name.c_str());
    if (ImGui::Combo("##preset", &selectedPreset, names.data(), (int)names.size()))
        loadPreset(selectedPreset, sim, renderer);
    if (!presets.empty())
        ImGui::TextWrapped("%s", presets[selectedPreset].description.c_str());

    ImGui::Spacing();

    // ── Sim controls ─────────────────────────────────────────
    ImGui::SeparatorText("Simulation");
    if (sim.paused) { if (ImGui::Button("▶  Resume", {-1,0})) sim.paused=false; }
    else            { if (ImGui::Button("⏸  Pause",  {-1,0})) sim.paused=true;  }
    if (ImGui::Button("⟳  Reset Preset", {-1,0})) loadPreset(selectedPreset,sim,renderer);

    ImGui::Spacing();

    // Log slider for time scale
    float logTS = std::log10((float)sim.timeScale);
    if (ImGui::SliderFloat("Time Scale", &logTS, 2.f, 9.5f, "10^%.1f s/s"))
        sim.timeScale = std::pow(10.0,(double)logTS);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Simulation speed.\n10^4 ~ 3 hrs/s\n10^7 ~ 4 months/s\n10^9 ~ 30 yrs/s");

    int sps=sim.stepsPerFrame;
    if (ImGui::SliderInt("Steps/Frame",&sps,1,60)) sim.stepsPerFrame=sps;
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("More steps = more accurate but slower.");

    ImGui::Spacing();

    // ── Visuals ───────────────────────────────────────────────
    ImGui::SeparatorText("Visuals");
    ImGui::Checkbox("Spacetime Mesh",       &renderer.showMesh);
    ImGui::Checkbox("Orbital Trails",       &renderer.showTrails);
    ImGui::Checkbox("Star Glow",            &renderer.showGlow);
    ImGui::Checkbox("Labels",               &renderer.showLabels);
    ImGui::Checkbox("Gravitational Lensing",&renderer.showLensing);

    ImGui::Spacing();
    ImGui::SeparatorText("Bodies");
    if (ImGui::Button("+ Add Custom Body",{-1,0})) showAddBody=!showAddBody;

    ImGui::Spacing();
    double years = sim.simTime/(365.25*24*3600);
    ImGui::Text("Sim Time : %.2f years", years);
    ImGui::Text("Bodies   : %d", (int)sim.bodies.size());

    ImGui::End();
}

void UI::panelBodyList(Simulation& sim) {
    ImGui::SetNextWindowPos ({10,440}, ImGuiCond_Always);
    ImGui::SetNextWindowSize({310,260},ImGuiCond_Always);
    ImGui::Begin("Body Inspector", nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    for (int i=0; i<(int)sim.bodies.size(); ++i) {
        const auto& b=sim.bodies[i];
        bool sel=(i==selectedBody);
        if (ImGui::Selectable(b.name.c_str(),sel)) selectedBody=i;
        if (sel && !sim.bodies.empty()) {
            ImGui::Indent();
            ImGui::Text("Mass    : %.3e kg (10^%.1f)",b.mass,std::log10(b.mass));
            ImGui::Text("Speed   : %.3e m/s", b.vel.length());
            ImGui::Text("Accel   : %.3e m/s²",b.acc.length());
            ImGui::Text("Dist CoM: %.4f AU",  Vec2::distance(b.pos,{0,0})/AU);
            ImGui::Unindent();
        }
    }
    ImGui::End();
}

void UI::windowAddBody(Simulation& sim) {
    ImGui::SetNextWindowSize({350,310},ImGuiCond_FirstUseEver);
    ImGui::Begin("Add Custom Body",&showAddBody);

    ImGui::InputText("Name",bodyName,64);
    ImGui::SliderFloat("Mass  (10^x kg)",&massExp,20.f,40.f,"10^%.1f");
    ImGui::Text("= %.3e kg", std::pow(10.0,(double)massExp));

    ImGui::Separator();
    ImGui::Text("Position (AU):");
    ImGui::SliderFloat("X##p",&posX,-50.f,50.f);
    ImGui::SliderFloat("Y##p",&posY,-50.f,50.f);
    ImGui::Text("Velocity (km/s):");
    ImGui::SliderFloat("Vx",&velX,-200.f,200.f);
    ImGui::SliderFloat("Vy",&velY,-200.f,200.f);
    ImGui::Text("Colour:");
    ImGui::SliderFloat("R",&colR,0.f,255.f);
    ImGui::SliderFloat("G",&colG,0.f,255.f);
    ImGui::SliderFloat("B",&colB,0.f,255.f);

    if (ImGui::Button("Spawn Body",{-1,0})) {
        Body b(std::string(bodyName),
               std::pow(10.0,(double)massExp),
               Vec2(posX*AU, posY*AU),
               Vec2(velX*1000.0, velY*1000.0),
               sf::Color((uint8_t)colR,(uint8_t)colG,(uint8_t)colB));
        sim.addBody(b);
        showAddBody=false;
    }
    ImGui::End();
}

void UI::overlayStats(const Simulation& sim) {
    ImGuiIO& io=ImGui::GetIO();
    ImGui::SetNextWindowPos({io.DisplaySize.x-10.f,10.f},ImGuiCond_Always,{1,0});
    ImGui::SetNextWindowBgAlpha(0.4f);
    ImGui::Begin("##stats",nullptr,
        ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoInputs|
        ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoNav);
    ImGui::Text("FPS: %.0f",io.Framerate);
    ImGui::Text("Scale: %.0e s/s",sim.timeScale);
    ImGui::End();
}

void UI::loadPreset(int idx, Simulation& sim, Renderer& renderer) {
    if (idx<0||idx>=(int)presets.size()) return;
    const Preset& p=presets[idx];
    sim.clear();
    for (const auto& b:p.bodies) sim.addBody(b);
    for (auto& b:sim.bodies) b.clearTrail();
    renderer.setViewForPreset(p.name);
}
