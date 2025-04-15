#include "NodeMap.h"
#include "PathAgent.h"
#include "raylib.h"
#include <algorithm>
#include <cfloat>
#include <iostream>

using namespace AIForGames;

void PathAgent::SetNode(AIForGames::Node* node)
{
    // Sets the agent's current node and updates its position to the node's position.
    if (node == nullptr) {
        std::cerr << "Error: Attempted to set a null node." << std::endl;
        return;
    }

    m_currentNode = node;
    m_position = node->position;
}

void PathAgent::SetSpeed(float speed)
{
    // Sets the movement speed of the agent
    m_speed = speed;
}



void PathAgent::Update(float deltaTime)
{
    // If no path to follow, exit early
    if (m_path.empty()) return;

    Node* nextNode = m_path[m_currentIndex];
    if (nextNode == nullptr) {
        std::cerr << "Error: Next node in the path is null." << std::endl;
        return;
    }

    // Compute direction and distance to the next node
    glm::vec2 direction = nextNode->position - m_position;
    float distance = glm::length(direction);

    glm::vec2 unitDirection = glm::normalize(direction);
    distance -= m_speed * deltaTime;

    if (distance > m_speed * deltaTime) {
        // Still moving toward target node
        m_position += unitDirection * m_speed * deltaTime;
    }
    else {
        // Arrived at (or overshot) the target node
        m_position = nextNode->position;
        m_currentIndex++;

        if (m_currentIndex >= m_path.size()) {
            if (m_targetNode != nullptr) {
                m_currentNode = m_targetNode;
                m_targetNode = nullptr;
            }
            m_path.clear();
        }
        else {
            // Transition to next node in the path
            m_position = nextNode->position;

            Node* newNextNode = m_path[m_currentIndex];
            if (newNextNode == nullptr) {
                std::cerr << "Error: New next node is null." << std::endl;
                return;
            }

            float overshootDistance = -distance;
            glm::vec2 newDirection = newNextNode->position - nextNode->position;
            glm::vec2 newUnitDirection = glm::normalize(newDirection);

            m_position += newUnitDirection * overshootDistance;
        }
    }
}

void PathAgent::GoToNode(AIForGames::Node* node, NodeMap& nodeMap, bool setEndNodeAsCurrent)
{
    if (node == nullptr) {
        std::cerr << "Error: Destination node is null." << std::endl;
        return;
    }

    m_path = nodeMap.AStarSearch(m_currentNode, node);
    if (m_path.empty()) {
        std::cerr << "Error: Path is empty. Check if start and end nodes are properly connected." << std::endl;
        return;
    }

    m_currentIndex = 0;

    // If we want the final node to become the new "start" node once we reach it,
    // store it so we can do that in Update()
    if (setEndNodeAsCurrent && !m_path.empty()) {
        m_targetNode = node;
    }
    else {
        m_targetNode = nullptr;
    }
}

    void PathAgent::Draw(Color color) const
    {
        // Renders the agent as a circle at its current position.
        DrawCircle((int)m_position.x, (int)m_position.y, 8, color);
    }
