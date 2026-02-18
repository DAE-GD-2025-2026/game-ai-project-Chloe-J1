#include "SteeringBehaviors.h"

#include "MeshPaintVisualize.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"

//SEEK
//*******
// TODO: Do the Week01 assignment :^)

SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{
	SteeringOutput Steering{};
	
	if (Target.Position != FVector2D{0,0})
		Steering.LinearVelocity = Target.Position - Agent.GetPosition();

	// Helper debug lines
	constexpr float LineSize{100.f};
	FVector2D Forward = FVector2D(Agent.GetActorForwardVector()).GetSafeNormal();
	FVector2D End = Agent.GetPosition() + Forward * LineSize;
	DrawDebugDirectionalArrow(Agent.GetWorld(), FVector(Agent.GetPosition(),0), FVector(End.X, End.Y,0), 3.f, FColor::Green );

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
	constexpr float SlowRadius{500.f};
	constexpr float TargetRadius{200.f};
	
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
	constexpr float Threshold{0.1f};
	const float RotSpeed{80.f};
	double AngularVelocity{0.f};
	
	
	
	FVector2D AgentForward(Agent.GetActorForwardVector().X, Agent.GetActorForwardVector().Y);

	// calc delta angle
	FVector2D Destination = Target.Position - Agent.GetPosition();
	
	double TargetAngle = FMath::Atan2(Destination.Y, Destination.X);
	double ForwardAngle = FMath::Atan2(AgentForward.Y, AgentForward.X);
	
	double DeltaAngle = TargetAngle - ForwardAngle;
	
	// if(GEngine)
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("dest: %f"), DeltaAngle));	
	
	if (abs(DeltaAngle) >= Threshold)
	{
		AngularVelocity =  DeltaAngle * DeltaT * RotSpeed;
	}
	else
	{
		AngularVelocity = 0.f;
	}
	
	Steering.AngularVelocity = AngularVelocity;
	return Steering;
	
}

//PURSUIT
//*******
SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{
	SteeringOutput Steering{};
	float Time{};
	const float Distance = (Agent.GetPosition() - Target.Position).Length();
	float Speed = Target.LinearVelocity.Length();
	
	if (Speed != 0)
		Time = Distance / Speed;
	
	FVector2D Predicted = Target.Position + Target.LinearVelocity * Time;
	FVector2D OriginalPos = Target.Position;
	Target.Position = Predicted;
	
	Steering = Seek::CalculateSteering(Time, Agent);
	
	// Reset target to the original target
	Target.Position = OriginalPos;
	
	// Helper debug lines
	constexpr float LineSize{100.f};
	FVector2D Forward = FVector2D(Agent.GetActorForwardVector()).GetSafeNormal();
	FVector2D End = Agent.GetPosition() + Forward * LineSize;
	DrawDebugDirectionalArrow(Agent.GetWorld(), FVector(Agent.GetPosition(),0), FVector(End.X, End.Y,0), 3.f, FColor::Green );
	
	DrawDebugCircle(Agent.GetWorld(), FVector(Predicted.X, Predicted.Y, 0.f), 10.f, 20, FColor::Purple, false, -1, 0, 3.f, FVector(0,1,0), FVector(1,0,0));
	
	return Steering;
}

SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{Pursuit::CalculateSteering(DeltaT, Agent)};
	
	// Evade is opposite of Pursuit, so inverse the values
	Steering.LinearVelocity *= -1;
	Steering.AngularVelocity *= -1;
	
	return Steering;
}

SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	constexpr float MaxAngle{360.f};
	constexpr float MinAngle{0.f};
	constexpr float Radius{80.f};
	constexpr float Offset{30.f};
	FVector2D Forward = FVector2D(Agent.GetActorForwardVector()).GetSafeNormal();
	FVector2D Center = Agent.GetPosition() + Forward * Radius + Offset;
	float Angle = FMath::RandRange(MinAngle, MaxAngle);
	
	FVector2D RandPoint = FVector2D(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius) + Center;
	
	Target.Position = RandPoint;
	
	Steering = Seek::CalculateSteering(DeltaT, Agent);
	
	// Draw Debug lines

	DrawDebugCircle(Agent.GetWorld(), FVector(Center.X, Center.Y, 0.f), Radius, 20, FColor::Purple, false, -1, 0, 3.f, FVector(0,1,0), FVector(1,0,0));
	DrawDebugCircle(Agent.GetWorld(), FVector(RandPoint.X, RandPoint.Y, 0.f), 10.f, 20, FColor::Purple, false, -1, 0, 3.f, FVector(0,1,0), FVector(1,0,0));
	
	
	return Steering;
}
