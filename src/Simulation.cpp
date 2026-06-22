#include "Simulation.h"
#include <cmath>

// ── Public ───────────────────────────────────────────────────────────────────

void Simulation::addBody(const Body& b) { bodies.push_back(b); }

void Simulation::clear() {
    bodies.clear();
    simTime = 0.0;
}

void Simulation::step(float realDeltaSeconds) {
    if (paused) return;
    double dt = (double)realDeltaSeconds * timeScale / stepsPerFrame;
    for (int i = 0; i < stepsPerFrame; ++i) {
        integrateRK4(dt);
        simTime += dt;
    }
    for (auto& b : bodies)
        b.pushTrail();
}

// ── RK4 internals ────────────────────────────────────────────────────────────

std::vector<Vec2> Simulation::computeAccelerations(const std::vector<State>& states) const {
    int n = (int)states.size();
    std::vector<Vec2> acc(n, {0, 0});

    for (int i = 0; i < n; ++i) {
        if (bodies[i].isFixed) continue;
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            Vec2   r      = states[j].pos - states[i].pos;
            double distSq = r.lengthSq() + SOFTENING * SOFTENING;
            double mag    = G * bodies[j].mass / distSq; // a = GM/r²
            acc[i] += r.normalized() * mag;
        }
    }
    return acc;
}

std::vector<Simulation::Deriv> Simulation::evaluate(
    const std::vector<State>& initial,
    double dt,
    const std::vector<Deriv>& d) const
{
    int n = (int)initial.size();
    std::vector<State> states(n);
    for (int i = 0; i < n; ++i)
        states[i] = { initial[i].pos + d[i].dpos * dt,
                      initial[i].vel + d[i].dvel * dt };

    auto accs = computeAccelerations(states);

    std::vector<Deriv> out(n);
    for (int i = 0; i < n; ++i)
        out[i] = { states[i].vel, accs[i] };
    return out;
}

void Simulation::integrateRK4(double dt) {
    int n = (int)bodies.size();

    std::vector<State> s0(n);
    for (int i = 0; i < n; ++i)
        s0[i] = { bodies[i].pos, bodies[i].vel };

    std::vector<Deriv> zero(n, {{0,0},{0,0}});
    auto k1 = evaluate(s0, 0.0,    zero);
    auto k2 = evaluate(s0, dt*0.5, k1);
    auto k3 = evaluate(s0, dt*0.5, k2);
    auto k4 = evaluate(s0, dt,     k3);

    for (int i = 0; i < n; ++i) {
        if (bodies[i].isFixed) continue;
        bodies[i].pos += (dt/6.0) * (k1[i].dpos + 2*k2[i].dpos + 2*k3[i].dpos + k4[i].dpos);
        bodies[i].vel += (dt/6.0) * (k1[i].dvel + 2*k2[i].dvel + 2*k3[i].dvel + k4[i].dvel);
        bodies[i].acc  = k1[i].dvel; // store latest acceleration for UI display
    }
}
