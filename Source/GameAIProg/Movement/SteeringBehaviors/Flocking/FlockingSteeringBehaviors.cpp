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
	
	FVector2D OutputVelocity{};
	TArray<ASteeringAgent*> pNeighborAgents = pFlock->GetNeighbors();
	
	for (int index = 0; index < pFlock->GetNrOfNeighbors(); ++index)
	{
		if (pNeighborAgents[index] == nullptr) continue;
		
		FVector2D ToAgent = FVector2D(pAgent.GetPosition() - pNeighborAgents[index]->GetPosition());
		// Inverse proportional = normalized / distance
		FVector2D PushForce = ToAgent / ToAgent.SquaredLength();
		OutputVelocity += PushForce;
	}
	

	Steering.LinearVelocity = OutputVelocity;

	
	Steering.IsValid = true;
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


