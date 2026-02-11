#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"

//SEEK
//*******
// TODO: Do the Week01 assignment :^)

SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{
		 SteeringOutput Steering{};
	 	
	 	 Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	
	 	 return Steering;
}

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{
	SteeringOutput Steering{};
	Steering = Seek::CalculateSteering(DeltaT, Agent);
	
	// Flee is opposite of Seek, so inverse the values
	Steering.LinearVelocity *= -1;
	Steering.AngularVelocity *= -1;
	
	return Steering;
}

// Agent.GetWorld()