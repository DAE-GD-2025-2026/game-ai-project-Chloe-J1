#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"


//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput Steering{};
	
	if (pFlock->GetNrOfNeighbors() == 0)
	{
		Steering.IsValid = false;
		return Steering;
	}
	
	FVector2D AvgPos = pFlock->GetAverageNeighborPos();
	
	
	
	
	Target.Position = AvgPos;
	Steering = Seek::CalculateSteering(deltaT, pAgent);
	Steering.IsValid = true;

	
	return Steering;
	
}

//*********************
//SEPARATION (FLOCKING)
SteeringOutput Separation::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput Steering{};
	if (pFlock->GetNrOfNeighbors() == 0)
	{
		Steering.IsValid = false;
		return Steering;
	}
	FVector2D Destination{};
	for (const auto& Neighbor : pFlock->GetNeighbors())
	{
		if (Neighbor == nullptr) continue;
		FVector2D VecToNeighbor = FVector2D(Neighbor->GetPosition() - pAgent.GetPosition());
		float DistanceToNeighbor = VecToNeighbor.Length();
		Destination += VecToNeighbor.GetSafeNormal() * (1.f / DistanceToNeighbor);
	}
	FVector2D OldTarget = Target.Position;
	Target.Position = Destination;
	Steering = Seek::CalculateSteering(deltaT, pAgent);
	Steering.LinearVelocity *= -1; // Move AWAY
	
	
	Target.Position = OldTarget;
	return Steering;
}

//*************************
//VELOCITY MATCH (FLOCKING)
SteeringOutput Alignment::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput Steering{};
	
	Steering.LinearVelocity = pFlock->GetAverageNeighborVelocity();
	
	return Steering;
}


