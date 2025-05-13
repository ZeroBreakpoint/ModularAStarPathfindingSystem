#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raylib.h"
#include "Pathfinding.h"
#include "NodeMap.h"
#include "PathAgent.h"
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <thread>
#include <mutex>
#include <atomic>
#include <ctime>

using namespace AIForGames;

int main(int argc, char* argv[])
{
    srand((unsigned int)time(nullptr));  // Seed random number generator

    int screenWidth = 1200;
    int screenHeight = 850;
    InitWindow(screenWidth, screenHeight, "A* Pathfinding - MultiThreaded Programming - Bradley Robertson");
    SetTargetFPS(120);

    // Define the ASCII map where 0 = empty, 1 = walkable
    std::vector<std::string> asciiMap = {
    "000000000000000000000000",
    "011111101111111011111110",
    "010001001000001010000010",
    "011101111011101011101110",
    "010100000010001010100010",
    "011101111111101011101110",
    "010000100000001010000010",
    "011110101111111011111110",
    "010000101000000010000010",
    "011111101011111111101110",
    "010000001000000000001010",
    "011111111111111111111110",
    "010000100000001000000010",
    "011101101111101011101110",
    "010001001000001010000010",
    "011111111011111011111110",
    "000000000000000000000000"
    };

    NodeMap nodeMap;
    nodeMap.Initialise(asciiMap, 50); // Build the node map using ASCII layout

    Node* startNode = nodeMap.GetNode(1, 1);
    Node* endNode = nodeMap.GetNode(10, 2);

    PathAgent agent; // Player controlled agent (green)
    agent.SetNode(startNode);
    agent.SetSpeed(64);

    std::vector<Node*> nodeMapPath = nodeMap.AStarSearch(startNode, endNode); // Initial path
    agent.GoToNode(endNode, nodeMap, false);

    // Multithreading for Player Agent
    std::thread pathfindingThread;
    std::mutex pathMutex;
    std::atomic<bool> isPathfinding(false);
    std::atomic<bool> newPathAvailable(false);
    std::vector<Node*> computedPath;

    // Wanderer Agent and its Thread
    PathAgent wanderer; // Blue autonomous agent
    wanderer.SetSpeed(64);
    bool isWandering = false;
    bool wandererNeedsNewPath = false;
    std::thread wandererThread;
    std::mutex wandererMutex;
    std::vector<Node*> wandererPath;
    std::atomic<bool> wandererPathReady = false;
    std::atomic<bool> wandererIsCalculating = false;

    float time = (float)GetTime();
    float deltaTime;

    std::cout << "[SYSTEM] Game started. Window initialized.\n";

    while (!WindowShouldClose()) // Main game loop
    {
        float fTime = (float)GetTime();
        deltaTime = fTime - time;
        time = fTime;

        agent.Update(deltaTime);
        wanderer.Update(deltaTime);

        // Toggle wandering on W key
        if (IsKeyPressed(KEY_W)) {
            isWandering = !isWandering;
            std::cout << "[WANDERER] Wandering " << (isWandering ? "started.\n" : "stopped.\n");

            if (!isWandering && wandererThread.joinable()) {
                wandererThread.join();
            }
        }

        // When wanderer needs a new path
        if (isWandering && wanderer.m_path.empty() && !wandererNeedsNewPath && !wandererIsCalculating) {
            wandererNeedsNewPath = true;
        }

        if (isWandering && wandererNeedsNewPath && !wandererIsCalculating) {
            wandererNeedsNewPath = false;
            wandererIsCalculating = true;

            Node* start = wanderer.GetCurrentNode();
            if (!start) {
                // Assign a starting node only once if it was never set
                start = GetRandomValidNode(nodeMap, 12, 8);
                wanderer.SetNode(start);
            }

            Node* end = GetRandomValidNode(nodeMap, 12, 8);

            wandererThread = std::thread([&, start, end]() {
                std::cout << "[WANDERER] Searching path from " << start->position.x << "," << start->position.y
                    << " to " << end->position.x << "," << end->position.y << " in thread ID: "
                    << std::this_thread::get_id() << "\n";

                auto path = nodeMap.AStarSearch(start, end);

                std::lock_guard<std::mutex> lock(wandererMutex);
                wandererPath = path;
                wandererPathReady = true;
                });
        }

		// Apply wanderer's path if ready
        if (wandererPathReady) {
            std::lock_guard<std::mutex> lock(wandererMutex);
            wanderer.GoToNode(wandererPath.back(), nodeMap, true);
            wanderer.m_path = wandererPath;
            wandererPathReady = false;

            if (wandererThread.joinable())
                wandererThread.join();

            wandererIsCalculating = false;
        }

        // Left click: Set new start node for player agent
        if (IsMouseButtonPressed(0) && !isPathfinding)
        {
            Vector2 mousePos = GetMousePosition();
            Node* selected = nodeMap.GetClosestNode(glm::vec2(mousePos.x, mousePos.y));
            if (selected) {
                startNode = selected;
                agent.SetNode(startNode);
                std::cout << "[INPUT] Start node set. Launching pathfinding thread.\n";

                if (pathfindingThread.joinable()) {
                    std::cout << "[THREAD] Joining previous pathfinding thread.\n";
                    pathfindingThread.join();
                }

                isPathfinding = true;
                pathfindingThread = std::thread([&]() {
                    std::cout << "[THREAD] A* pathfinding started...\n";
                    auto path = nodeMap.AStarSearch(startNode, endNode);
                    std::lock_guard<std::mutex> lock(pathMutex);
                    computedPath = path;
                    newPathAvailable = true;
                    isPathfinding = false;
                    });
            }
        }

		// Right click: Set new end node for player agent
        if (IsMouseButtonPressed(1))
        {
            Vector2 mousePos = GetMousePosition();
            Node* selected = nodeMap.GetClosestNode(glm::vec2(mousePos.x, mousePos.y));
            if (selected) {
                endNode = selected;
                std::cout << "[INPUT] End node set. Launching pathfinding thread.\n";

                if (!isPathfinding && startNode != nullptr) {
                    if (pathfindingThread.joinable()) {
                        std::cout << "[THREAD] Joining previous pathfinding thread.\n";
                        pathfindingThread.join();
                    }

                    isPathfinding = true;
                    pathfindingThread = std::thread([&]() {
                        std::cout << "[PLAYER] A* pathfinding started in thread ID: " << std::this_thread::get_id() << "\n";
                        auto path = nodeMap.AStarSearch(startNode, endNode);
                        std::lock_guard<std::mutex> lock(pathMutex);
                        computedPath = path;
                        newPathAvailable = true;
                        isPathfinding = false;
                        });
                }
            }
        }

		// Apply computed path to agent if available
        if (newPathAvailable && !isPathfinding)
        {
            std::lock_guard<std::mutex> lock(pathMutex);
            std::cout << "[MAIN] Applying computed path to agent.\n";
            nodeMapPath = computedPath;
            agent.GoToNode(endNode, nodeMap, false);
            newPathAvailable = false;
        }

        // Render everything
        BeginDrawing();
        ClearBackground(BLACK);
        nodeMap.Draw();
        nodeMap.DrawPath(nodeMapPath, WHITE);
        agent.Draw(GREEN);
        wanderer.Draw(BLUE);
        EndDrawing();
    }

    // Join threads before shutdown
    if (pathfindingThread.joinable())
        pathfindingThread.join();
    if (wandererThread.joinable())
        wandererThread.join();

    std::cout << "[SYSTEM] Game shutting down.\n";
    CloseWindow();
    return 0;
}