#include "Pathfinding.h"

void AIForGames::Node::ConnectTo(Node* other, float cost)
{
	// Establishes a connection from this node to another node (other) with an associated cost
	connections.push_back(Edge(other, cost));
}
