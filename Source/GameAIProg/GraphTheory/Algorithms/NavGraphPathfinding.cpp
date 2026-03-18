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

	//Create Extra node for the Start Node (Agent's position

	//Create extra node for the endNode

	//Run A star on new graph

	//Debug Visualisation

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