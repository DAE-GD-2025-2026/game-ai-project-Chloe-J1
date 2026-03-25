#include "NavGraphPathfinding.h"

#include "AStar.h"
#include "PathSmoothing.h"
#include "VectorTypes.h"
#include "Shared/Graph/NavGraph/NavGraph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

using namespace GameAI;

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos,
	NavGraph* const pNavGraph, std::vector<FVector2D>& debugNodePositions, std::vector<NavLine>& debugPortals) 
{
	//Create the path to return
	std::vector<FVector2D> finalPath{};

	//Get the start and endTriangle
	const TriPolygon::Triangle* startTriangle{pNavGraph->GetNavPolygon()->GetTriangleAtPosition(startPos, true)};
	const TriPolygon::Triangle* endTriangle{pNavGraph->GetNavPolygon()->GetTriangleAtPosition(endPos, true)};
	if (startTriangle == nullptr || endTriangle == nullptr) return finalPath;
	
	if (startTriangle == endTriangle)
	{
		finalPath.push_back(startPos);
		finalPath.push_back(endPos);
		return finalPath;
	}

	//We have valid start/end triangles and they are not the same
	//=> Start looking for a path
	//Copy the graph
	std::shared_ptr<NavGraph> clonedGraph{ pNavGraph->Clone()};

	//Create Extra node for the Start Node (Agent's position)
	Node startNode{startPos};
	int startNodeId = clonedGraph->AddNode(std::make_unique<Node>(startNode));
	int startEdgeIdx{-1};
	
	std::array<TriPolygon::Edge, 3> edges = startTriangle->GetEdges();
	for (const auto& edge : edges)
	{
		// Find index of edge, doesn't exist? -> skip
		std::optional<int> edgeIdx = pNavGraph->GetNavPolygon()->FindEdgeIndex(edge);
		if (!edgeIdx.has_value()) continue;
		
		int nodeId = clonedGraph->GetNodeIdFromEdgeIndex(edgeIdx.value());
		if (nodeId == GameAI::Graphs::InvalidNodeId) continue;
		
		std::unique_ptr<Connection> newConnection = std::make_unique<Connection>(startNodeId, nodeId);
		float cost = (startPos - clonedGraph->GetNode(nodeId)->GetPosition()).Length();
		newConnection->SetWeight(cost);
		clonedGraph->AddConnection(std::move(newConnection));
	}
	
	//Create extra node for the endNode
	Node endNode{endPos};
	int endNodeId = clonedGraph->AddNode(std::make_unique<Node>(endNode));
	std::array<TriPolygon::Edge, 3> endEdges = endTriangle->GetEdges();
	for (const auto& edge : endEdges)
	{
		std::optional<int> edgeIdx = pNavGraph->GetNavPolygon()->FindEdgeIndex(edge);
		if (!edgeIdx.has_value()) continue;

		int nodeId = clonedGraph->GetNodeIdFromEdgeIndex(edgeIdx.value());
		if (nodeId == GameAI::Graphs::InvalidNodeId) continue;

		auto newConnection = std::make_unique<Connection>(nodeId, endNodeId);
		float cost = (endPos - clonedGraph->GetNode(nodeId)->GetPosition()).Length();
		newConnection->SetWeight(cost);
		clonedGraph->AddConnection(std::move(newConnection));
	}

	
	//Run A star on new graph
	AStar AStar{clonedGraph.get(), HeuristicFunctions::Euclidean};
	Node* pStartNode = clonedGraph->GetNode(startNodeId).get();
	Node* pEndNode = clonedGraph->GetNode(endNodeId).get();

	std::vector<Node*> nodePath = AStar.FindPath(pStartNode, pEndNode);
	//Debug Visualisation
	for (Node* pNode : nodePath)
	{
		debugNodePositions.push_back(pNode->GetPosition());
		finalPath.push_back(pNode->GetPosition());
	}

	// Extra: Run optimiser on new graph (First check if everything works without SSFA!)
	// debugPortals = SSFA::FindPortals(nodes, *pNavGraph->GetNavPolygon());
	// finalPath = SSFA::OptimizePortals(debugPortals, *pNavGraph->GetNavPolygon());
	
	
	return finalPath;
}

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos, NavGraph* const pNavGraph)
{
	std::vector<FVector2D> debugNodePositions{};
	std::vector<NavLine> debugPortals{};

	return FindPath(startPos, endPos, pNavGraph, debugNodePositions, debugPortals);
}