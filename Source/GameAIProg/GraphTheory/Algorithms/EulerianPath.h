#pragma once
#include <stack>

#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "Shared/Graph/Graph.h"

namespace GameAI
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	class EulerianPath final
	{
	public:
		EulerianPath(Graph* const pGraph);

		Eulerianity IsEulerian() const;
		std::vector<Node*> FindPath(Eulerianity& eulerianity) const;

		bool IsConnected() const; // temp public
	private:
		void VisitAllNodesDFS(const std::vector<Node*>& pNodes, std::vector<bool>& visited, int startIndex) const;
		bool IsOddDegree(int index) const;

		Graph* m_pGraph;
	};

	inline EulerianPath::EulerianPath(Graph* const pGraph)
		: m_pGraph(pGraph)
	{
	}

	inline Eulerianity EulerianPath::IsEulerian() const
	{
		int NrOddDegrees{0};
		std::vector<Node*> Nodes = m_pGraph->GetActiveNodes();
		// TODO If the graph is not connected, there can be no Eulerian Trail
		// TODO Count nodes with odd degree 
		for (int index = 0; index < Nodes.size(); ++index)
		{
			if (IsOddDegree(index))
				if (m_pGraph->GetIsDirectional())
				{
					NrOddDegrees += 2;
				}
		}
		
		// TODO A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian
		if (NrOddDegrees > 2)
			return Eulerianity::notEulerian;
		

		// TODO A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (unless there are only 2 nodes)
		// TODO An Euler trail can be made, but only starting and ending in these 2 nodes
		if (Nodes.size() == 2)
			return Eulerianity::eulerian; // correct?
		if (NrOddDegrees == 2)
			return Eulerianity::semiEulerian;

		// TODO A connected graph with no odd nodes is Eulerian
		if (NrOddDegrees == 0)
			return Eulerianity::eulerian;
		
		return Eulerianity::notEulerian;
	}

	inline std::vector<Node*> EulerianPath::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		Graph graphCopy = m_pGraph->Clone();
		std::vector<Node*> Path = {};
		std::vector<Node*> Nodes = graphCopy.GetActiveNodes();
		int currentNodeId{ Graphs::InvalidNodeId };
		
		// TODO Check if there can be an Euler path
		// TODO If this graph is not eulerian, return the empty path
		
		switch (eulerianity)
		{
			case Eulerianity::notEulerian:
			return Path;
			break;
			case Eulerianity::semiEulerian:
			break;
			case Eulerianity::eulerian:
			break;
		}
		
		for (const auto& Node : Nodes)
		{
			
		}
		
		// TODO Start algorithm loop
		std::stack<int> nodeStack;

		std::reverse(Path.begin(), Path.end());
		return Path;
	}

	inline void EulerianPath::VisitAllNodesDFS(const std::vector<Node*>& Nodes, std::vector<bool>& visited, int startIndex ) const
	{
		// TODO Mark the visited node
		if(Nodes[startIndex]->GetId() == Graphs::InvalidNodeId)
			return;
		
		visited[startIndex] = true;
		// TODO Ask the graph for the connections from that node
		
		std::vector<Connection*> Connections = m_pGraph->FindConnectionsFrom(startIndex);
		// TODO recursively visit any valid connected nodes that were not visited before
		
		for (int index = 0; index < Connections.size(); ++index)
		{
			int checkIdx = Connections[index]->GetToId();
			if (visited[checkIdx] == false)
				VisitAllNodesDFS(Nodes, visited, checkIdx);
		}
		// TODO Tip: use an index-based for-loop to find the correct index
	}

	inline bool EulerianPath::IsConnected() const
	{
		std::vector<Node*> Nodes = m_pGraph->GetActiveNodes();
		if (Nodes.size() == 0)
			return false;
		
		std::vector<bool> Visited(Nodes.size(), false);
		// TODO choose a starting node
		int StartIndex{ 0 };
		// TODO start a depth-first-search traversal from the node that has at least one connection
		VisitAllNodesDFS(Nodes, Visited, StartIndex);
		
		// TODO if a node was never visited, this graph is not connected
		for (bool IsVisited : Visited)
		{
			if (IsVisited == false)
				return false;
		}
		return true;
	}
	
	inline bool EulerianPath::IsOddDegree(int index) const
	{
		int NrOfConnections{ 0 };
		std::vector<Node*> Nodes = m_pGraph->GetActiveNodes();
		std::vector<Connection*> ConnectionsFrom =  m_pGraph->FindConnectionsFrom(index);
		std::vector<Connection*> ConnectionsTo = m_pGraph->FindConnectionsTo(index);
		
		NrOfConnections += ConnectionsFrom.size() + ConnectionsTo.size();
		if (NrOfConnections % 2)
			return false;
		
		return true;
	}
}