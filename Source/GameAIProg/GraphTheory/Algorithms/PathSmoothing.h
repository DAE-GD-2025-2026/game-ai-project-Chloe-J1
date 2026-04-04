#pragma once
#include <vector>

#include "NavGraphPathfinding.h"
#include "Movement/Pathfinding/Navmesh/TriPolygon.h"
#include "Shared/Graph/Graph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

namespace GameAI
{
	class SSFA final
{
public:
	//=== SSFA Functions ===
	//--- References ---
	//http://digestingduck.blogspot.be/2010/03/simple-stupid-funnel-algorithm.html
	//https://gamedev.stackexchange.com/questions/68302/how-does-the-simple-stupid-funnel-algorithm-work
	// static std::vector<NavLine> FindPortals(std::vector<Node*> const & Path, TriPolygon const & NavPoly)
	// {
	// 	//Container
	// 	std::vector<NavLine> Portals = {};
	// 	if (Path.empty()) return Portals;
	// 	
	// 	//For each node received, get it's corresponding line
	// 	
	// 		//Redetermine it's "orientation" based on the required path (left-right vs right-left) - p1 should be right point
	//
	// 		//Store portal
	//
	// 	//Add degenerate portal to force end evaluation
	//
	// 	return Portals;
	// }
		
static std::vector<NavLine> FindPortals(std::vector<Node*> const& Path, TriPolygon const& NavPoly)
{
    std::vector<NavLine> Portals = {};

    if (Path.empty()) return Portals;

    // First portal: start position
    {
        NavLine StartPortal{};
        StartPortal.P1 = Path.front()->GetPosition();
        StartPortal.P2 = Path.front()->GetPosition();
        Portals.push_back(StartPortal);
    }

    // For each node received, get its corresponding line
    for (int index = 1; index < static_cast<int>(Path.size()) - 1; ++index)
    {
        FVector2D NodePos     = Path[index]->GetPosition();
        FVector2D NextNodePos = Path[index + 1]->GetPosition();

        // Find the edge whose midpoint matches the node position
        int foundEdgeIdx = -1;
        for (int e = 0; e < static_cast<int>(NavPoly.GetEdges().size()); ++e)
        {
            TriPolygon::Edge const& E = NavPoly.GetEdges()[e];
            FVector Mid   = (E.GetP1(NavPoly) + E.GetP2(NavPoly)) / 2.f;
            FVector2D Mid2D(Mid.X, Mid.Y);
            if (FVector2D::Distance(Mid2D, NodePos) < 1.f)
            {
                foundEdgeIdx = e;
                break;
            }
        }

        if (foundEdgeIdx < 0) continue;

        TriPolygon::Edge const& Edge = NavPoly.GetEdges()[foundEdgeIdx];
        FVector P1 = Edge.GetP1(NavPoly);
        FVector P2 = Edge.GetP2(NavPoly);

        // Redetermine orientation - P1 should be right point
        FVector2D Dir     = (NextNodePos - NodePos).GetSafeNormal();
        FVector2D EdgeVec = FVector2D(P2.X - P1.X, P2.Y - P1.Y);
        float Cross2D     = Dir.X * EdgeVec.Y - Dir.Y * EdgeVec.X;

        // Store portal
        NavLine Portal{};
    	if (Cross2D < 0.f)
    	{
    		Portal.P1 = FVector2D(P2.X, P2.Y); // right
    		Portal.P2 = FVector2D(P1.X, P1.Y); // left
    	}
    	else
    	{
    		Portal.P1 = FVector2D(P1.X, P1.Y); // right
    		Portal.P2 = FVector2D(P2.X, P2.Y); // left
    	}

        Portals.push_back(Portal);
    }

    // Add degenerate portal to force end evaluation
    {
        NavLine EndPortal{};
        EndPortal.P1 = Path.back()->GetPosition();
        EndPortal.P2 = Path.back()->GetPosition();
        Portals.push_back(EndPortal);
    }

    return Portals;
}

	static std::vector<FVector2D> OptimizePortals( std::vector<NavLine> const & Portals, TriPolygon const & NavPoly)
	{
		std::vector<FVector2D> Path{};
		FVector2D rightLeg{};
		FVector2D leftLeg{};
		FVector2D apexPoint{Portals[0].P1};
		// int apexIndex  = 0;
		int rightLegIndex = 0;
		int leftLegIndex  = 0;
		
		Path.push_back(apexPoint); // Add apexPoint to the path
		//P1 == right point of portal, P2 == left point of portal
		for (int portalIdx = 1; portalIdx < Portals.size(); portalIdx++)
		{
			NavLine currentPortal{Portals[portalIdx]};
			//--- RIGHT CHECK ---
			FVector2D newRightLeg{currentPortal.P1 - apexPoint};
			//1. See if moving funnel inwards - RIGHT
			if (FVector2D::CrossProduct(rightLeg, newRightLeg) >= 0) // Going inward
			{
				//2. See if new line degenerates a line segment - RIGHT
				if (FVector2D::CrossProduct(leftLeg, newRightLeg) >= 0) // Cross left leg
				{
					//Leftleg becomes new apex point
					apexPoint += leftLeg;
					portalIdx = leftLegIndex + 1;
					leftLegIndex = portalIdx;
					rightLegIndex = portalIdx;
					
					Path.push_back(apexPoint);
					//Calculate new legs (if not the end)
					if (portalIdx < Portals.size())
					{
						rightLeg = Portals[portalIdx].P1 - apexPoint;
						leftLeg = Portals[portalIdx].P1 - apexPoint;
						continue;
					}
				}
				else
				{
					rightLeg = newRightLeg;
					rightLegIndex = portalIdx;
				}
				
			}


			//--- LEFT CHECK ---
			FVector2D newLeftLeg{currentPortal.P2 - apexPoint};
			//1. See if moving funnel inwards - LEFT
			if (FVector2D::CrossProduct(leftLeg, newLeftLeg) <= 0)
			{
				//2. See if new line degenerates a line segment - LEFT
				if (FVector2D::CrossProduct(rightLeg, newLeftLeg) <= 0)
				{
					//Rightleg becomes new apex point
					apexPoint += rightLeg;
					portalIdx = rightLegIndex + 1;
					rightLegIndex = portalIdx;
					leftLegIndex = portalIdx;
					
					Path.push_back(apexPoint);
					//Calculate new legs (if not the end)
					if (portalIdx < Portals.size())
					{
						rightLeg = Portals[portalIdx].P2 - apexPoint;
						leftLeg = Portals[portalIdx].P2 - apexPoint;
						continue;
					}
				}
				else
				{
					leftLeg = newLeftLeg;
					leftLegIndex = portalIdx;
				}
			}
		}
			


		// Add last path point
		Path.push_back(Portals.back().P1);
		return Path;
	}
private:
	SSFA() {};
	~SSFA() {};
};
}
