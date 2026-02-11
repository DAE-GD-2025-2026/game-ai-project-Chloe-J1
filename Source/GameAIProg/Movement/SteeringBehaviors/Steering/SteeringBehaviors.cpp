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
//FLEE
//*******
SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{
	SteeringOutput Steering{};
	Steering = Seek::CalculateSteering(DeltaT, Agent);
	
	// Flee is opposite of Seek, so inverse the values
	Steering.LinearVelocity *= -1;
	Steering.AngularVelocity *= -1;
	
	return Steering;
}

//ARRIVE
//*******
SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{
	const float SlowRadius{500.f};
	const float TargetRadius{200.f};
	const float dempValue{50.f};
	
	// Calculate regular speed
	SteeringOutput Steering{};
	Steering = Seek::CalculateSteering(DeltaT, Agent);
	
	//Adapt speed based on radius
	float distance = Steering.LinearVelocity.Length();
	if (distance < TargetRadius )
	{
		Agent.SetMaxLinearSpeed(0.f);
	}
	else if (distance < SlowRadius)
	{
		Agent.SetMaxLinearSpeed(Steering.LinearVelocity.Length() / 3.f);
	}
	else
	{
		// Use original speed
		Agent.SetMaxLinearSpeed(Steering.LinearVelocity.Length());
	}
	
	// Draw helper lines
	constexpr float TargetCircleRadius{10.f};
	DrawDebugCircle(Agent.GetWorld(), FVector(Agent.GetPosition(),0), TargetRadius, 20, FColor::Red, false, -1, 0, 3.f, FVector(0,1,0), FVector(1,0,0));
	DrawDebugCircle(Agent.GetWorld(), FVector(Agent.GetPosition(),0), SlowRadius, 20, FColor::Blue, false, -1, 0, 3.f, FVector(0,1,0), FVector(1,0,0));
	DrawDebugCircle(Agent.GetWorld(), FVector(Target.Position,0), TargetCircleRadius, 20, FColor::Red, false, -1, 0, 3.f, FVector(0,1,0), FVector(1,0,0));
	
	return Steering;
}

//FACE
//*******
SteeringOutput Face::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{
	Agent.SetIsAutoOrienting(false);
	SteeringOutput Steering{};
	double AngularVelocity{0.f};
	FVector2D Destination = Target.Position - Agent.GetPosition();
	double DeltaAngle{ atan2(Destination.Y - Agent.GetActorForwardVector().Y, Destination.X - Agent.GetActorForwardVector().X) };
	
	
	
	Steering.AngularVelocity = AngularVelocity;
	return Steering;
}


// DrawDebugArrow/Direction ipv DrawDebugLine