# Numerical Integration

The equations of motion cant always be solved analytically
for many-body systems

A better solution is to approximate motion over small time steps using numerical integration methods

## Euler Integration
simple but unstable for long running gravitational simulations because it doesnt conserve energy well

Velocity update:
$$
v_{t+Δt} = v_t + a_t Δt
$$

Position update:

$$
x_{t+Δt} = x_t + v_t Δt
$$

## Semi-Implicit Euler

Velocity is updated before position:

$$
v_{t+Δt} = v_t + a_t Δt
$$

$$
x_{t+Δt} = x_t + v_{t+Δt} Δt
$$
