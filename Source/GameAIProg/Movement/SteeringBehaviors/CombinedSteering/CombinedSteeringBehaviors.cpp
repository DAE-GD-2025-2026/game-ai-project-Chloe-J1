
#include "CombinedSteeringBehaviors.h"
#include <algorithm>
#include "../SteeringAgent.h"

BlendedSteering::BlendedSteering(const std::vector<WeightedBehavior>& WeightedBehaviors)
	:WeightedBehaviors(WeightedBehaviors)
{};

//****************
//BLENDED STEERING
SteeringOutput BlendedSteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput BlendedSteering{};
	// TODO: Calculate the weighted average steeringbehavior
	float TotalWeight{0.f};
	
	
	for (const WeightedBehavior& Behavior : WeightedBehaviors)
	{
		TotalWeight += Behavior.Weight;
		
		Behavior.pBehavior->SetTarget(Target);
	}
	
	
	
	for (const WeightedBehavior& Behavior : WeightedBehaviors)
	{
		ISteeringBehavior* SteeringBehavior = Behavior.pBehavior;
		SteeringOutput Steering = SteeringBehavior->CalculateSteering(DeltaT, Agent);
		BlendedSteering.LinearVelocity += Steering.LinearVelocity * (Behavior.Weight / TotalWeight);
	}
	// TODO: Add debug drawing
	constexpr float LineSize{100.f};
	FVector2D Forward = FVector2D(Agent.GetActorForwardVector()).GetSafeNormal();
	FVector2D End = Agent.GetPosition() + Forward * LineSize;
	DrawDebugDirectionalArrow(Agent.GetWorld(), FVector(Agent.GetPosition(),0), FVector(End.X, End.Y,0), 3.f, FColor::Red );

	
	return BlendedSteering;
}

float* BlendedSteering::GetWeight(ISteeringBehavior* const SteeringBehavior)
{
	auto it = find_if(WeightedBehaviors.begin(),
		WeightedBehaviors.end(),
		[SteeringBehavior](const WeightedBehavior& Elem)
		{
			return Elem.pBehavior == SteeringBehavior;
		}
	);

	if(it!= WeightedBehaviors.end())
		return &it->Weight;
	
	return nullptr;
}

//*****************
//PRIORITY STEERING
SteeringOutput PrioritySteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering = {};
	
	
	
	for (ISteeringBehavior* const pBehavior : m_PriorityBehaviors)
	{
		
		pBehavior->SetTarget(Target);

		
		Steering = pBehavior->CalculateSteering(DeltaT, Agent);
		
		if (Steering.IsValid)
		{
			break;
		}
	}

	//If non of the behavior return a valid output, last behavior is returned
	return Steering;
}