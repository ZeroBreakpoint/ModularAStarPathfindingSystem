#pragma once
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "raylib.h"
#include "Pathfinding.h"
#include "NodeMap.h"
#include <cfloat>

namespace AIForGames {

    class PathAgent
    {
    private:
        glm::vec2 m_position{ 0.0f, 0.0f }; // Current position of the agent in world space
        int m_currentIndex{ 0 }; // Current index in the path
        AIForGames::Node* m_currentNode{ nullptr }; // Node the agent is currently sitting on
        float m_speed{ 0.0f }; // Movement speed in pixels per second
		Node* m_targetNode{ nullptr }; // Target node to reach

    public:
        std::vector<AIForGames::Node*> m_path; // Active path the agent is following
        void Update(float deltaTime); // Updates agent movement along its path
		void GoToNode(AIForGames::Node* node, NodeMap& nodeMap, bool setEndNodeAsCurrent = false); // Sets a new target node and calculates the path to it
        void Draw(Color color) const; // Draws the agent on screen
        void SetNode(AIForGames::Node* node); // Sets the agent's current node and updates position
        void SetSpeed(float speed); // Adjusts the movement speed
		AIForGames::Node* GetCurrentNode() const { return m_currentNode; } // Returns the current node
    };
}

