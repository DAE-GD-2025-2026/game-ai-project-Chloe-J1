#include "BFS.h"

#include <map>
#include <queue>
#include <unordered_set>

#include "AStar.h"
#include "Shared/Graph/Graph.h"

using namespace GameAI;

BFS::BFS(Graph* const pGraph)
	: pGraph(pGraph)
{
}

// TODO Breath First Search Algorithm searches for a path from the startNode to the destinationNode
std::vector<Node*> BFS::FindPath(Node* const pStartNode, Node* const pDestinationNode) const
{
	std::vector<Node*> path;
	std::unordered_set<int> visited; // Nodes for which the neighbors need to be checked next 
	std::unordered_map<int, Node*> parent; // Nodes that have been checked 
	Node* node;
	std::queue<Node*> queue;
	
	queue.push(pStartNode);
	visited.insert(pStartNode->GetId());
	
	
	while (!queue.empty())
	{
		node = queue.front();
		queue.pop();

	
		if (node == pDestinationNode)
			return ReconstructPath(parent, pStartNode, pDestinationNode);
		
		for (Connection* neighbor : pGraph->FindConnectionsFrom(node->GetId()))
		{
			if (visited.contains(neighbor->GetToId()) == false)
			{
				visited.insert(neighbor->GetToId());
				parent[neighbor->GetToId()] = node;
				queue.push(pGraph->GetNodeAs<Node>(neighbor->GetToId()));
			}
			
		}
	}
	
	return path; // no path found
}

std::vector<Node*> BFS::ReconstructPath(std::unordered_map<int, Node*> parentMap, Node* const pStartNode, Node* const pDestinationNode) const
{
	Node* current = pDestinationNode;
	std::vector<Node*> path;
	
	while (current != pStartNode)
	{
		path.push_back(current);
		current = parentMap[current->GetId()];
	}
	path.push_back(pStartNode);	
	std::ranges::reverse(path.begin(), path.end());
	return path;
}
