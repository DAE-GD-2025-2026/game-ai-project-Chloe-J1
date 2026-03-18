#include "NavGraph.h"

#include "NavGraphNode.h"
#include "StaticMeshAttributes.h"

GameAI::NavGraph::NavGraph(std::unique_ptr<TriPolygon> && NavPoly)
	: Graph{false}
	, pNavPoly{std::move(NavPoly)}
{
	CreateNavigationGraph();
}

GameAI::NavGraph::NavGraph(const NavGraph& Other)
	: Graph(false)
{
	Nodes.reserve(Other.Nodes.size());
	for (std::unique_ptr<Node> const & OtherNode : Other.Nodes)
	{
		Nodes.push_back(std::make_unique<NavGraphNode>(*dynamic_cast<NavGraphNode*>(OtherNode.get())));
	}
        
	Connections.reserve(Other.Connections.size());
	for (std::unique_ptr<Connection> const & OtherConnection : Other.Connections)
	{
		Connections.push_back(std::make_unique<Connection>(*OtherConnection.get()));
	}
}

std::unique_ptr<GameAI::NavGraph> GameAI::NavGraph::Clone() const
{
	return std::make_unique<NavGraph>(*this);
}

int GameAI::NavGraph::GetNodeIdFromEdgeIndex(int EdgeIdx) const
{
	if (EdgeIdx >= 0)
	{
		for (auto const & pNode : Nodes)
		{
			if (reinterpret_cast<NavGraphNode*>(pNode.get())->GetEdgeIdx() == EdgeIdx)
			{
				return pNode->GetId();
			}
		}
	}
	
	return Graphs::InvalidNodeId;
}

void GameAI::NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigation mesh and create nodes
			
	
	for (const TriPolygon::Edge& edge: pNavPoly->GetEdges())
	{
		for (const TriPolygon::Triangle& triangle : pNavPoly->GetTriangles())
		{
			if (triangle.HasEdge(edge))
			{
				FVector2D pos {edge.GetP1(*pNavPoly) + edge.GetP2(*pNavPoly)};
				pos /= 2; // middle pos
				auto result = pNavPoly->FindEdgeIndex(edge);
				if (result.has_value())
				{
					// Create node here
					std::unique_ptr<NavGraphNode> newNode = std::make_unique<NavGraphNode>(pos , result.value());
					this->AddNode(std::move(newNode));
				}
			}
		}
	}
	std::vector<int> nodeIds;
	//2. Create connections now that every node is created
	//3. Set the connections cost to the actual distance
	for (const TriPolygon::Triangle& triangle : pNavPoly->GetTriangles())
	{
		for (const auto& edge : triangle.GetEdges())
		{
			auto result = pNavPoly->FindEdgeIndex(edge);
			if (result.has_value())
			{
				nodeIds.push_back(GetNodeIdFromEdgeIndex(result.value()));
			}
		}
		//2 valid nodes -> 1 connection
		if (nodeIds.size() == 2)
		{
			Connection newConnection{ nodeIds[0], nodeIds[1]};
			newConnection.SetWeight(GetDistance(nodeIds[0], nodeIds[1]));
			this->AddConnection(std::make_unique<Connection>(newConnection));
		}
		//3 valid nodes -> 3 connections
		else if (nodeIds.size() == 3)
		{
			std::unique_ptr<Connection> newConnection = std::make_unique<Connection>(nodeIds[0], nodeIds[1]);
			newConnection->SetWeight(GetDistance(nodeIds[0], nodeIds[1]));
			this->AddConnection(std::move(newConnection));
			newConnection = std::make_unique<Connection>(nodeIds[1], nodeIds[2]);
			newConnection->SetWeight(GetDistance(nodeIds[1], nodeIds[2]));
			this->AddConnection(std::move(newConnection));
			newConnection = std::make_unique<Connection>(nodeIds[2], nodeIds[0]);
			newConnection->SetWeight(GetDistance(nodeIds[2], nodeIds[0]));
			this->AddConnection(std::move(newConnection));
		}
		
		nodeIds.clear();
		
	}
		
}

float GameAI::NavGraph::GetDistance(int id1, int id2) const
{
	return (this->GetNodeAs<Node>(id1)->GetPosition() - this->GetNodeAs<Node>(id2)->GetPosition()).Length();
}
