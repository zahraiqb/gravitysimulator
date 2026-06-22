#pragma once
#include "Body.h"
#include <vector>

class Simulation {
public:
    static constexpr double G         = 6.674e-11; // SI
    static constexpr double SOFTENING = 1e8;       // metres — prevents div/0 singularity

    std::vector<Body> bodies;

    double simTime      = 0.0;
    double timeScale    = 3600.0 * 24.0 * 7.0; // sim-seconds per real-second (1 week/s default)
    bool   paused       = false;
    int    stepsPerFrame = 10;  // sub-steps per rendered frame for accuracy

    void addBody(const Body& b);
    void clear();
    void step(float realDeltaSeconds);  // call once per frame

private:
    struct State { Vec2 pos, vel; };
    struct Deriv { Vec2 dpos, dvel; };

    std::vector<Vec2>  computeAccelerations(const std::vector<State>& states) const;
    std::vector<Deriv> evaluate(const std::vector<State>& initial,
                                double dt,
                                const std::vector<Deriv>& d) const;
    void integrateRK4(double dt);
};
