#include "Flock.h"
#include "FlockingSteeringBehaviors.h"
#include "Movement/SteeringBehaviors/SpacePartitioning/SpacePartitioning.h"
#include "Shared/ImGuiHelpers.h"

// Disable tick voor agents bij spawning
// Eerst Register Agents, dan Tick van alle agents

Flock::Flock(
	UWorld* pWorld,
	TSubclassOf<ASteeringAgent> AgentClass,
	int flockSize,
	float WorldSize,
	ASteeringAgent* const pAgentToEvade,
	bool bTrimWorld)
	: pWorld{pWorld}
	, pAgentToEvade{pAgentToEvade}
{
 // TODO: initialize the flock and the memory pool
	FlockSize = flockSize;
	Agents.SetNumZeroed(FlockSize);
	NrOfNeighbors = 0;
#ifndef GAMEAI_USE_SPACE_PARTITIONING
	Neighbors.SetNumZeroed(FlockSize);
#endif
	
#ifdef GAMEAI_USE_SPACE_PARTITIONING
	OldPositions.SetNumZeroed(flockSize);
	pPartitionedSpace = std::make_unique<CellSpace>(pWorld, WorldSize*2, WorldSize*2, 2,2, FlockSize);
#endif
	
	//Behaviors
	pSeekBehavior = std::make_unique<Seek>();
	pWanderBehavior = std::make_unique<Wander>();
	pCohesionBehavior = std::make_unique<Cohesion>(this);
	pSeparationBehavior = std::make_unique<Separation>(this);
	pAlignmentBehavior = std::make_unique<Alignment>(this);
	pEvadeBehavior = std::make_unique<Evade>();
	

	
	WeightedBehaviors.emplace_back(pCohesionBehavior.get(), 0.3f);
	WeightedBehaviors.emplace_back(pSeparationBehavior.get(), 0.1f);
	WeightedBehaviors.emplace_back(pAlignmentBehavior.get(), 0.3f);
	WeightedBehaviors.emplace_back(pWanderBehavior.get(), 0.1f);
	WeightedBehaviors.emplace_back(pSeekBehavior.get(), 0.3f);
	
	pBlendedSteering = std::make_unique<BlendedSteering>(WeightedBehaviors);
	

	pPrioritySteering = std::make_unique<PrioritySteering>(
		std::vector<ISteeringBehavior*>(std::vector<ISteeringBehavior*>{
			pEvadeBehavior.get(), // -> higher in priority list
			pBlendedSteering.get()}
		));
	
	//EvadeTarget agent
	Wander* pWand = new Wander();
	pAgentToEvade->SetSteeringBehavior(pWand);
	pAgentToEvade->SetDebugRenderingEnabled(false);
		 

	// NO PARTITIONING
	//Flock agents
	
	int SpawnedCount = 0;
	for (int index = 0; index < FlockSize; ++index)
	{
		if (ASteeringAgent* Agent = SpawnAgent(AgentClass, WorldSize))
		{
			Agent->SetActorTickEnabled(false);
			Agent->SetSteeringBehavior(pPrioritySteering.get());
			Agent->SetDebugRenderingEnabled(false);
			Agents[SpawnedCount] = Agent;
#ifdef GAMEAI_USE_SPACE_PARTITIONING
			pPartitionedSpace->AddAgent(*Agent);
			OldPositions[SpawnedCount] = Agent->GetPosition();
#endif
			SpawnedCount++;
		}
		else
		{
			FlockSize--;
		}
	}
	
}

Flock::~Flock()
{
 // TODO: Cleanup any additional data
}

void Flock::Tick(float DeltaTime)
{
 // TODO: update the flock
 // TODO: for every agent:
#ifndef GAMEAI_USE_SPACE_PARTITIONING
	// NO PARTITIONING
	for (ASteeringAgent* pAgent : Agents)
	{
		// TODO: register the neighbors for this agent (-> fill the memory pool with the neighbors for the currently evaluated agent)
		RegisterNeighbors(pAgent);
		
		
		// TODO: update the agent (-> the steeringbehaviors use the neighbors in the memory pool)
		pAgent->Tick(DeltaTime);
		
		// TODO: trim the agent to the world
	}
	

	
#endif
	
#ifdef GAMEAI_USE_SPACE_PARTITIONING
	// PARTITIONING
	// Update cells & agents
	for (int index = 0; index < Agents.Num(); ++index)
	{
		ASteeringAgent* pAgent = Agents[index];
		if (!pAgent) continue;

		pPartitionedSpace->UpdateAgentCell(*pAgent, OldPositions[index]);
		
		pPartitionedSpace->RegisterNeighbors(*pAgent, NeighborhoodRadius);
		if (not IsFirstNeighborhoodInitialized)
		{
			RenderNeighborhood();
			IsFirstNeighborhoodInitialized = true;
		}
		
		
		pAgent->Tick(DeltaTime);
		OldPositions[index] = pAgent->GetPosition();
	}
	IsFirstNeighborhoodInitialized = false;
#endif
	SetTarget_Evade();
	DebugRenderEvadeAgent();
}

void Flock::RenderDebug()
{
 // TODO: Render all the agents in the flock
}

void Flock::ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize)
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		bool bWindowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Gameplay Programming", &bWindowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Flocking");
		ImGui::Spacing();

  // TODO: implement ImGUI checkboxes for debug rendering here

		ImGui::Text("Behavior Weights");
		ImGui::Spacing();
		
		if (ImGui::Checkbox("Spatial Partitioning", &IsSpacePartitioning))
		{
			
		}
		
		

  // TODO: implement ImGUI sliders for steering behavior weights here
		ImGui::Text("Behavior Sliders");
		ImGui::Spacing();
		if (pBlendedSteering != nullptr)
		{
			
			
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Cohesion",
	pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight, 0.f, 1.f,
	[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight = InVal; }, "%.2f");
    
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Separation",
			pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight, 0.f, 1.f,
			[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight = InVal; }, "%.2f");
    
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Alignment",
			pBlendedSteering->GetWeightedBehaviorsRef()[2].Weight, 0.f, 1.f,
			[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[2].Weight = InVal; }, "%.2f");
	
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Wander",
			pBlendedSteering->GetWeightedBehaviorsRef()[3].Weight, 0.f, 1.f,
			[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[3].Weight = InVal; }, "%.2f");
    
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Seek",
			pBlendedSteering->GetWeightedBehaviorsRef()[4].Weight, 0.f, 1.f,
			[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[4].Weight = InVal; }, "%.2f");
		}
		
		//End
		ImGui::End();
	}
#pragma endregion
#endif
}

void Flock::DebugRenderEvadeAgent()
{
	DrawDebugCircle(pWorld, FVector(pAgentToEvade->GetPosition(),0), 50.f, 20, FColor::Purple, false, -1, 0, 3.f,FVector(0,1,0), FVector(1,0,0));
}

void Flock::RenderNeighborhood()
{
 // TODO: Debugrender the neighbors for the first agent in the flock
	if (Agents.Num() == 0) return;

	// Agent of who the neighborhood is
	DrawDebugCircle(pWorld, FVector(Agents[0]->GetPosition().X, Agents[0]->GetPosition().Y, 0.f), NeighborhoodRadius, 20, FColor::Green, false, -1, 0, 3.f,FVector(0,1,0), FVector(1,0,0));

	// Neighbors
	TArray<ASteeringAgent*> CurrentNeighbors = GetNeighbors();
	for (int i = 0; i < GetNrOfNeighbors(); ++i)
	{
		if (CurrentNeighbors[i] == nullptr) continue;
		DrawDebugCircle(pWorld, FVector(CurrentNeighbors[i]->GetPosition().X, CurrentNeighbors[i]->GetPosition().Y, 0.f), 80.f, 20, FColor::Yellow, false, -1, 0, 3.f, FVector(0,1,0), FVector(1,0,0));
	}
}

ASteeringAgent* Flock::SpawnAgent(TSubclassOf<ASteeringAgent> AgentClass, float WorldSize)
{
	constexpr int MaxTries = 50;
    
	for (int Try = 0; Try < MaxTries; ++Try)
	{
		FVector SpawnPos{
			FMath::RandRange(-WorldSize, WorldSize),
			FMath::RandRange(-WorldSize, WorldSize),
			90.f
		};
        
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		
		ASteeringAgent* Agent = pWorld->SpawnActor<ASteeringAgent>(
			AgentClass, SpawnPos, FRotator::ZeroRotator, SpawnParams);
        
		if (Agent != nullptr)
		{
			return Agent;
		}
	}
	return nullptr;
}

#ifndef GAMEAI_USE_SPACE_PARTITIONING
void Flock::RegisterNeighbors(ASteeringAgent* const pAgent)
{
	NrOfNeighbors = 0;
	
	if (pAgent == nullptr)
		return;
	for (ASteeringAgent* NeighborAgent : Agents)
	{
		if (NeighborAgent == pAgent || NeighborAgent == nullptr) continue;
		
		float DistanceToNeighbor = (pAgent->GetPosition() - NeighborAgent->GetPosition()).Length();
		if (DistanceToNeighbor <= NeighborhoodRadius)
		{
			// Add to this Agent's neighbors
			Neighbors[NrOfNeighbors] = NeighborAgent;
			++NrOfNeighbors;
			
		}
		
		
	}
	if (pAgent == Agents[0])
	{
		RenderNeighborhood();
	}

}

#endif



FVector2D Flock::GetAverageNeighborPos() const
{
	FVector2D AvgPosition = FVector2D::ZeroVector;
	int NrNeighbors = GetNrOfNeighbors();
	if (NrNeighbors == 0) return AvgPosition;
    TArray<ASteeringAgent*> Neighb = GetNeighbors();
	for (int index = 0; index < NrNeighbors; ++index)
	{
		AvgPosition += Neighb[index]->GetPosition();
	}
	return AvgPosition / NrNeighbors;
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	FVector2D AvgVelocity = FVector2D::ZeroVector;
	int NrNeighbors = GetNrOfNeighbors();
	if (NrNeighbors == 0) return AvgVelocity;
    
	TArray<ASteeringAgent*> Neighb = GetNeighbors();
	for (int index = 0; index < NrNeighbors; ++index)
	{
		AvgVelocity += Neighb[index]->GetLinearVelocity();
	}

	return AvgVelocity / NrNeighbors;
}

void Flock::SetTarget_Seek(FSteeringParams const& Target)
{
	pSeekBehavior->SetTarget(Target);
}

void Flock::SetTarget_Evade()
{
	if (pAgentToEvade == nullptr) return;
	
	FTargetData TargetData;
	TargetData.AngularVelocity = pAgentToEvade->GetAngularVelocity();
	TargetData.LinearVelocity = pAgentToEvade->GetLinearVelocity();
	TargetData.Position = pAgentToEvade->GetPosition();
	TargetData.Orientation = pAgentToEvade->GetRotation();
	
	pEvadeBehavior->SetTarget(TargetData);
	
}



