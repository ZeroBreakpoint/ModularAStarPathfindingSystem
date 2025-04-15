# Modular A* Pathfinding System (C++ / Raylib / GLM)

This project implements a modular, multithreaded A* pathfinding system for 2D grid-based environments.  It is designed for integration into real-time applications such as tactical games, simulations, or AI testing tools.

The system features:
- **Multithreaded A\*** searches to offload pathfinding from the main thread.
- **Modular architecture**: decoupled classes for agents, map and pathfinding logic.
- **Smooth agent movement** with overshoot correction.
- **Interactive test application** built using Raylib and GLM.

## Controls

- **Left Click**: Set the **start node** for the player-controlled agent (green).
- **Right Click**: Set the **end node** for the player-controlled agent.
- **`W` Key**: Toggle the autonomous **Wanderer agent** (blue) on/off.
  - When active, it continuously picks a new random destination once it finishes each path.
- Real-time feedback is printed to the console, including pathfinding thread activity and debug logs.

##  Key Features

- **A\* Pathfinding Algorithm**  
  Custom implementation with squared Euclidean heuristic (no external dependencies).

- **Multithreading**  
  Thread-safe path calculation using `std::thread`, `std::mutex`, and `std::atomic` for responsiveness.

- **Modular Design**  
  Organized into reusable source modules:
  - `Pathfinding.h/.cpp`
  - `NodeMap.h/.cpp`
  - `PathAgent.h/.cpp`

- **Cross-Platform Friendly**  
  Built using open-source libraries:
  - [Raylib](https://www.raylib.com/)
  - [GLM](https://github.com/g-truc/glm)
