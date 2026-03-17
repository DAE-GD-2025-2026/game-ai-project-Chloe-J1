#include "AStar.h"

#include <unordered_set>

using namespace GameAI;

AStar::AStar(Graph* const pGraph, HeuristicFunctions::Heuristic hFunction)
	: pGraph(pGraph)
	, HeuristicFunction(hFunction)
{
}

std::vector<Node*>AStar::FindPath(Node* const pStartNode, Node* const pGoalNode)
{
	std::vector<Node*> path{};
	std::vector<NodeRecord> openList{}; // nodes to be checked
	std::vector<NodeRecord> closedList{}; // nodes already checked
	NodeRecord currentNodeRecord{}; 
	Node* pNextNode{nullptr};
	
	// Create a startRecord and add it to the openList to start the while loop
	NodeRecord startRecord{};
	startRecord.pNode = pStartNode;
	startRecord.pConnection = nullptr;
	startRecord.estimatedTotalCost = GetHeuristicCost(pStartNode, pGoalNode);
	openList.push_back(startRecord);
	float totalGCost{};
	
	while(!openList.empty())
	{
		// Get record from the open list with lowest F-score
		currentNodeRecord = *std::min_element(openList.begin(), openList.end());
		// Check if that record refers to the end node
		if (currentNodeRecord.pNode == pGoalNode)
			break; // if so, exit while loop

		for (Connection* connection : pGraph->FindConnectionsFrom(currentNodeRecord.pNode->GetId()))
		{
			pNextNode = pGraph->GetNodeAs<Node>(connection->GetToId()); // 1.
			totalGCost = currentNodeRecord.costSoFar + connection->GetWeight(); // 2.
			// 3. Check if the connection leads to a node already on the closedList
			
			for (int index = 0; index < closedList.size(); ++index)
			{
				if (closedList[index].pNode == pNextNode)
				{
					if (closedList[index].costSoFar < totalGCost)
						continue;
					else
						closedList.erase(closedList.begin() + index);
					break;
				}
			}

			
			// 4. Check if the connection leads to a node already on the openlist
			for (int index = 0; index < openList.size(); ++index)
			{
				if (openList[index].pNode == pNextNode)
				{
					if (openList[index].costSoFar <= totalGCost)
						continue;
					else
						openList.erase(openList.begin() + index);
					break;
				}
			}
	
			
			// 5. new record
			NodeRecord newRecord{};
			newRecord.pNode = pNextNode;
			newRecord.pConnection = connection;
			newRecord.costSoFar = totalGCost;
			newRecord.estimatedTotalCost = totalGCost + GetHeuristicCost(pNextNode, pGoalNode);
			openList.push_back(newRecord);
		}
		
		// remove the currentNodeRecord from the openList and add it to the closedList
		openList.erase(std::find(openList.begin(), openList.end(), currentNodeRecord));
		closedList.push_back(currentNodeRecord);
	}
	
	// Reconstruct path from last connection to start node
	while (currentNodeRecord.pNode != pStartNode)
	{
		path.push_back(currentNodeRecord.pNode);
		for (const auto& node : closedList)
		{
			if (node.pNode == pGraph->GetNodeAs<Node>(currentNodeRecord.pConnection->GetFromId()))
			{
				currentNodeRecord = node;
				break;
			}
		}
	}
	path.push_back(pStartNode);
	std::reverse(path.begin(), path.end());
	return path;
}

float AStar::GetHeuristicCost(Node* const pStartNode, Node* const pEndNode) const
{
	FVector2D toDestination = pGraph->GetNode(pEndNode->GetId())->GetPosition() - pGraph->GetNode(pStartNode->GetId())->GetPosition();
	return HeuristicFunction(abs(toDestination.X), abs(toDestination.Y));
}