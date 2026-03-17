most gravity equations can be found here. 

# Gravitational Model
This simulation uses Newtonian gravity which describes gravity as a resultant force acting between all objects with a mass

$$
F = G \frac{m_1 m_2}{r^2}
$$

where:
- $F$ is the gravitational force
- $G$ is the gravitational constant (6.69 x10^-11) 
- $m_1, m_2$ are masses of the interacting objects
- $r$ is the distance between bodies

Now, gravity gives a fore but motion requires accelaration, from Newton's second law:
$$
F = ma
$$

Therefore:
$$
a = \frac{F}{m}
$$


## Vector Form
Gravitational force between bodies i and j:

$$
\vec{F}_{ij} =
G \frac{m_i m_j}{|\vec{r}_{ij}|^3} \vec{r}_{ij}
$$

Where:
- $\vec{r}_{ij}$ is the displacement vector between bodies.


## N-Body Simulation
The total force on a body is the sum of gravitational forces
from all other bodies.

$$
F_i = \sum_{j \ne i} F_{ij}
$$


## Time Discretisation
The simulation advances time using discrete timesteps:

t → t + Δt

Each timestep updates
- acceleration
- velocity
- position
