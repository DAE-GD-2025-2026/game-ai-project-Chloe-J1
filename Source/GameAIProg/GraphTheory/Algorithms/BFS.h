#pragma once
#include <unordered_map>
#include <vector>

namespace GameAI
{
	class Graph;
	class Node;

	class BFS
	{
	public:
		BFS(Graph* const pGraph);

		std::vector<Node*> FindPath(Node* const pStartNode, Node* const pDestinationNode) const;

	private:
		Graph* pGraph;
		
		std::vector<Node*> ReconstructPath(std::unordered_map<int, Node*> parentMap, Node* const pStartNode, Node* const pDestinationNode) const;
	};
}
