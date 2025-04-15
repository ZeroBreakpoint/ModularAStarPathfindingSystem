#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <raylib.h>
#include <cfloat>

namespace AIForGames
{
    // Forward declaration for circular references
    struct Node;

    // Edge represents a connection from one node to another with an associated cost
    struct Edge {
        Node* target; // Destination node of the edge
        float cost; // Travel cost
        Edge() : target(nullptr), cost(0) {}
        Edge(Node* _target, float _cost) : target(_target), cost(_cost) {}
    };

    // Node represents a single walkable location on the map
    struct Node {
        glm::vec2 position; // Position in world space
        float hScore; // Heuristic cost (estimated cost to goal)
        float fScore; // Total cost (g + h)
        std::vector<Edge> connections; // Adjacent nodes and their travel costs
        float gScore; // Cost from start node to this node
        Node* previous; // Pointer to previous node in the path
        Node() : position(0.0f, 0.0f), gScore(FLT_MAX), hScore(FLT_MAX), fScore(FLT_MAX), previous(nullptr) {} // Default constructor initialises scores to FLT_MAX (unvisited)
        Node(float x, float y) : position(x, y), gScore(FLT_MAX), hScore(FLT_MAX), fScore(FLT_MAX), previous(nullptr) {} // Constructor with specific position
        void ConnectTo(Node* other, float cost); // Adds a connection to another node with the given cost
    };
}
