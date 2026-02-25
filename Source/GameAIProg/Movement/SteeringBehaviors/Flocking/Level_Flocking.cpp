// Fill out your copyright notice in the Description page of Project Settings.


#include "Level_Flocking.h"

#include <format>


// Sets default values
ALevel_Flocking::ALevel_Flocking()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_Flocking::BeginPlay()
{
	Super::BeginPlay();

	TrimWorld->SetTrimWorldSize(3000.f);
	TrimWorld->bShouldTrimWorld = true;
	
	pAgentToEvade =
			GetWorld()->SpawnActor<ASteeringAgent>(
				SteeringAgentClass,
				FVector{200,200,90.f},
				FRotator::ZeroRotator);

	pFlock = TUniquePtr<Flock>(
		new Flock(
			GetWorld(),
			SteeringAgentClass,
			FlockSize,
			TrimWorld->GetTrimWorldSize(),
			pAgentToEvade,
			true)
			);
}

bool ALevel_Flocking::AddAgent(BehaviorTypes BehaviorType, bool AutoOrient)
{
	ImGui_Agent ImGuiAgent = {};
	ImGuiAgent.Agent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, FVector{0,0,90}, FRotator::ZeroRotator);
	if (IsValid(ImGuiAgent.Agent))
	{
		ImGuiAgent.SelectedBehavior = static_cast<int>(BehaviorType);
		ImGuiAgent.SelectedTarget = -1; // Mouse
		
		SetAgentBehavior(ImGuiAgent);

		
		SteeringAgents.push_back(std::move(ImGuiAgent));
		
		
		RefreshTargetLabels();
		
		return true;
	}
	
	return false;
}

void ALevel_Flocking::RemoveAgent(unsigned int Index)
{
	SteeringAgents[Index].Agent->Destroy();
	SteeringAgents.erase(SteeringAgents.begin() + Index);

	RefreshTargetLabels();
	RefreshAgentTargets(Index);
}

void ALevel_Flocking::SetAgentBehavior(ImGui_Agent& Agent)
{
	Agent.Behavior.reset();
	
	switch (static_cast<BehaviorTypes>(Agent.SelectedBehavior))
	{
		//TODO; Implement behaviors setting here
	case BehaviorTypes::Seek:
		Agent.Behavior = std::make_unique<Seek>();
		break;
	case BehaviorTypes::Flee:
		Agent.Behavior = std::make_unique<Flee>();
		break;
	case BehaviorTypes::Arrive:
		Agent.Behavior = std::make_unique<Arrive>();
		break;
	case BehaviorTypes::Face:
		Agent.Behavior = std::make_unique<Face>();
		break;
	case BehaviorTypes::Pursuit:
		Agent.Behavior = std::make_unique<Pursuit>();
		break;
	case BehaviorTypes::Evade:
		Agent.Behavior = std::make_unique<Evade>();
		break;
	case BehaviorTypes::Wander:
		Agent.Behavior = std::make_unique<Wander>();
		break;
	// case BehaviorTypes::Blended:
	// 	Agent.Behavior = std::make_unique<BlendedSteering>(CombinedBehaviors);
	// 	Agent.Behavior->SetTarget(MouseTarget);
	// 	pBlendedSteering = static_cast<BlendedSteering*>(Agent.Behavior.get());  
	// 	break;
	// case BehaviorTypes::Priority:
	// 	Agent.Behavior = std::make_unique<PrioritySteering>(PriorityBehaviors);
	// 	break;
	default:
		assert(false); // Incorrect Agent Behavior gotten during SetAgentBehavior()	
	} 

	UpdateTarget(Agent);
	
	Agent.Agent->SetSteeringBehavior(Agent.Behavior.get());
}

void ALevel_Flocking::RefreshTargetLabels()
{
	TargetLabels.clear();
	
	TargetLabels.push_back("Mouse");
	for (int i{0}; i < SteeringAgents.size(); ++i)
	{
		TargetLabels.push_back(std::format("Agent {}", i));
	}
}

void ALevel_Flocking::UpdateTarget(ImGui_Agent& Agent)
{
	// Note: MouseTarget position is updated via Level BP every click
	
	bool const bUseMouseAsTarget = Agent.SelectedTarget < 0;
	if (!bUseMouseAsTarget)
	{
		ASteeringAgent* const TargetAgent = SteeringAgents[Agent.SelectedTarget].Agent;

		FTargetData Target;
		Target.Position = TargetAgent->GetPosition();
		Target.Orientation = TargetAgent->GetRotation();
		Target.LinearVelocity = TargetAgent->GetLinearVelocity();
		Target.AngularVelocity = TargetAgent->GetAngularVelocity();

		Agent.Behavior->SetTarget(Target);
	}
	else
	{
		Agent.Behavior->SetTarget(MouseTarget);
	}
}

void ALevel_Flocking::RefreshAgentTargets(unsigned int IndexRemoved)
{
	for (UINT i = 0; i < SteeringAgents.size(); ++i)
	{
		if (i >= IndexRemoved)
		{
			auto& Agent = SteeringAgents[i];
			if (Agent.SelectedTarget == IndexRemoved || i  == Agent.SelectedTarget)
			{
				--Agent.SelectedTarget;
			}
		}
	}
}

// Called every frame
void ALevel_Flocking::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	pFlock->ImGuiRender(WindowPos, WindowSize);
	pFlock->Tick(DeltaTime);
	pFlock->RenderDebug();
	if (bUseMouseTarget)
	{
		pFlock->SetTarget_Seek(MouseTarget);
	}
	pFlock->SetTarget_Evade();
}

