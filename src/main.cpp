#include <iostream>

// Runs the simulation loop

int main() {
    std::cout << "Gravity simulator starting...\n";

    PhysicsEngine engine;

    engine.addBody(...);
    engine.addBody(...);

    while (true) {
        engine.update(0.01);
    }
    
}


import independent


# Unweighted GreedyIS
g = independent.Graph(7, "graph1U", False)
print("GreedyIS (unweighted):", g.GreedyIS())

# Weighted GreedyIS (criterion a)
g = independent.Graph(7, "graph1w", True)
print("GreedyIS (weighted a):", g.GreedyIS())

# Weighted GreedyIS_b (criterion b)
g = independent.Graph(7, "graph1w", True)
print("GreedyIS_b (weighted b):", g.GreedyIS_b())

# GridIS value
g = independent.Graph(40, "graph2Wgrid40", True)
print("GridIS value:", g.GridIS())
print("GridIS set:", g.GridISset())