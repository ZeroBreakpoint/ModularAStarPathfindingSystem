#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include "Pathfinding.h"
#include <raylib.h>

namespace AIForGames {

    class NodeMap
    {
        int m_width, m_height; // Dimensions of the grid (in cells)
        float m_cellSize; // Size of each cell in pixels
        AIForGames::Node** m_nodes; // 2D array of node pointers (flattened)

    public:
        NodeMap(); // Constructor
        ~NodeMap(); // Destructor
        AIForGames::Node* GetNode(int x, int y); // Retrieves a node at specific coordinates (nullptr if out of bounds)
        void Initialise(std::vector<std::string> asciiMap, int cellSize); // Builds the node map from an ASCII layout
        void Draw(); // Renders the map including walls and node connections
        std::vector<AIForGames::Node*> AStarSearch(AIForGames::Node* startNode, AIForGames::Node* endNode); // A* implementation
        void DrawPath(const std::vector<AIForGames::Node*>& path, Color lineColor); // Draws a computed path visually
        AIForGames::Node* GetClosestNode(glm::vec2 worldPos); // Gets the nearest node to a mouse click or agent position
    };
    Node* GetRandomValidNode(NodeMap& nodeMap, int width, int height); // Utility function that returns a random walkable node from the map
}