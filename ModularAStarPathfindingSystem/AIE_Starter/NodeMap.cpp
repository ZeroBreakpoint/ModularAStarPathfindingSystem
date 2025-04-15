#include <glm/glm.hpp>
#include "NodeMap.h"
#include "Pathfinding.h"
#include <iostream>
#include <algorithm>
#include <cfloat>
#include "raylib.h"

using namespace AIForGames;

// Constructor: Initialises the node map with default values
NodeMap::NodeMap() : m_width(0), m_height(0), m_cellSize(0), m_nodes(nullptr) {}

// Destructor: Cleans up allocated memory for nodes
NodeMap::~NodeMap() {
    if (m_nodes) {
        for (int i = 0; i < m_width * m_height; i++) {
            delete m_nodes[i]; // Delete each node
        }
        delete[] m_nodes; // Delete the array of pointers
    }
}

// Initialises the node map using an ASCII representation
void NodeMap::Initialise(std::vector<std::string> asciiMap, int cellSize) {
    m_cellSize = static_cast<float>(cellSize); // Convert cell size to float
    const char emptySquare = '0'; // Empty square representation in ASCII map

    // Determine the map's dimensions
    m_height = static_cast<int>(asciiMap.size());
    m_width = static_cast<int>(asciiMap[0].size());

    // Allocate memory for nodes
    m_nodes = new Node * [m_width * m_height]();

    // Loop through the ASCII map to create nodes
    for (int y = 0; y < m_height; y++) {
        std::string& line = asciiMap[y];
        if (line.size() != static_cast<size_t>(m_width)) {
            std::cout << "Mismatched line #" << y << " in ASCII map (" << line.size()
                << " instead of " << m_width << ")" << std::endl;
        }

        for (int x = 0; x < m_width; x++) {
            char tile = x < static_cast<int>(line.size()) ? line[x] : emptySquare;
            if (tile != emptySquare) {
                // Create a new node for non-empty tiles
                m_nodes[x + m_width * y] = new Node(
                    (static_cast<float>(x) + 0.5f) * m_cellSize,
                    (static_cast<float>(y) + 0.5f) * m_cellSize
                );
            }
        }
    }

    // Establish connections between adjacent nodes
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            Node* node = GetNode(x, y);
            if (node) {
                // Connect to the west node
                Node* nodeWest = x == 0 ? nullptr : GetNode(x - 1, y);
                if (nodeWest) {
                    node->ConnectTo(nodeWest, 1); // Default weight of 1
                    nodeWest->ConnectTo(node, 1);
                }

                // Connect to the south node
                Node* nodeSouth = y == 0 ? nullptr : GetNode(x, y - 1);
                if (nodeSouth) {
                    node->ConnectTo(nodeSouth, 1);
                    nodeSouth->ConnectTo(node, 1);
                }
            }
        }
    }
}

// Retrieves the node at the specified (x, y) grid position
Node* NodeMap::GetNode(int x, int y) {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
        return nullptr; // Return nullptr if out of bounds
    return m_nodes[x + m_width * y];
}

// Draws the node map and its connections
void NodeMap::Draw() {
    Color cellColor{ 255, 0, 0, 255 }; // Red for empty cells
    Color lineColor{ 128, 128, 128, 255 }; // Grey for connections

    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            Node* node = GetNode(x, y);
            if (node == nullptr) {
                // Draw a rectangle for empty cells
                DrawRectangle(
                    static_cast<int>(x * m_cellSize),
                    static_cast<int>(y * m_cellSize),
                    static_cast<int>(m_cellSize - 1),
                    static_cast<int>(m_cellSize - 1),
                    cellColor
                );
            }
            else {
                // Draw lines to connected nodes
                for (int i = 0; i < static_cast<int>(node->connections.size()); i++) {
                    Node* other = node->connections[i].target;
                    DrawLine(
                        static_cast<int>(node->position.x),
                        static_cast<int>(node->position.y),
                        static_cast<int>(other->position.x),
                        static_cast<int>(other->position.y),
                        lineColor
                    );
                }
            }
        }
    }
}

// A* Pathfinding algorithm implementation
std::vector<Node*> NodeMap::AStarSearch(Node* startNode, Node* endNode) {
    if (startNode == nullptr || endNode == nullptr) {
        std::cerr << "Error: Start or End node is null." << std::endl;
        return std::vector<Node*>();
    }

    auto heuristic = [](Node* a, Node* b) {
        // Heuristic: Squared Euclidean distance
        glm::vec2 diff = b->position - a->position;
        return glm::dot(diff, diff);
        };

    // Initialise start node
    startNode->gScore = 0;
    startNode->hScore = heuristic(startNode, endNode);
    startNode->fScore = startNode->gScore + startNode->hScore;
    startNode->previous = nullptr;

    std::vector<Node*> openList;
    std::vector<Node*> closedList;

    openList.push_back(startNode);

    while (!openList.empty()) {
        // Sort open list by fScore
        std::sort(openList.begin(), openList.end(), [](Node* a, Node* b) {
            return a->fScore < b->fScore;
            });

        Node* currentNode = openList.front();
        if (currentNode == endNode) break;

        openList.erase(openList.begin());
        closedList.push_back(currentNode);

        for (Edge& connection : currentNode->connections) {
            Node* targetNode = connection.target;
            if (std::find(closedList.begin(), closedList.end(), targetNode) == closedList.end()) {
                float tentative_gScore = currentNode->gScore + connection.cost;
                float tentative_hScore = heuristic(targetNode, endNode);
                float tentative_fScore = tentative_gScore + tentative_hScore;

                if (std::find(openList.begin(), openList.end(), targetNode) == openList.end()) {
                    targetNode->gScore = tentative_gScore;
                    targetNode->hScore = tentative_hScore;
                    targetNode->fScore = tentative_fScore;
                    targetNode->previous = currentNode;
                    openList.push_back(targetNode);
                }
                else if (tentative_fScore < targetNode->fScore) {
                    targetNode->gScore = tentative_gScore;
                    targetNode->hScore = tentative_hScore;
                    targetNode->fScore = tentative_fScore;
                    targetNode->previous = currentNode;
                }
            }
        }
    }

    // Build the path by backtracking from the end node
    std::vector<Node*> path;
    Node* currentNode = endNode;
    while (currentNode != nullptr) {
        path.insert(path.begin(), currentNode);
        currentNode = currentNode->previous;
    }

    return path;
}

// Draws the calculated path on the screen
void NodeMap::DrawPath(const std::vector<Node*>& path, Color lineColor) {
    if (path.empty()) return;

    for (size_t i = 1; i < path.size(); i++) {
        Node* nodeA = path[i - 1];
        Node* nodeB = path[i];

        if (nodeA != nullptr && nodeB != nullptr) {
            DrawLine(
                static_cast<int>(nodeA->position.x),
                static_cast<int>(nodeA->position.y),
                static_cast<int>(nodeB->position.x),
                static_cast<int>(nodeB->position.y),
                lineColor
            );
        }
    }
}

// Finds the closest node to a given world position
Node* NodeMap::GetClosestNode(glm::vec2 worldPos) {
    int i = static_cast<int>(worldPos.x / m_cellSize);
    int j = static_cast<int>(worldPos.y / m_cellSize);

    if (i < 0 || i >= m_width || j < 0 || j >= m_height) {
        std::cerr << "Error: Clicked position is out of bounds." << std::endl;
        return nullptr;
    }

    Node* node = GetNode(i, j);
    if (node == nullptr) {
        std::cerr << "Error: Closest node is null. No valid node at this position." << std::endl;
    }

    return node;
}

namespace AIForGames {
    Node* GetRandomValidNode(NodeMap& nodeMap, int width, int height)
    {
        // Continuously selects a random grid position until it finds a valid node (i.e., one that exists and is walkable).
        // Used by the Blue wanderer agent to find new destinations without crashing on invalid positions.
        Node* node = nullptr;
        while (node == nullptr)
        {
            int x = rand() % 12;
            int y = rand() % 8;
            node = nodeMap.GetNode(x, y);
        }
        return node;
    }
}
