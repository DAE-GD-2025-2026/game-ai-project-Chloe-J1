#include "Flock.h"
#include "FlockingSteeringBehaviors.h"
#include "Shared/ImGuiHelpers.h"

// Disable tick voor agents bij spawning
// Eerst Register Agents, dan Tick van alle agents

Flock::Flock(
	UWorld* pWorld,
	TSubclassOf<ASteeringAgent> AgentClass,
	int FlockSize,
	float WorldSize,
	ASteeringAgent* const pAgentToEvade,
	bool bTrimWorld)
	: pWorld{pWorld}
	, FlockSize{ FlockSize }
	, pAgentToEvade{pAgentToEvade}
{
 // TODO: initialize the flock and the memory pool
	Agents.SetNum(FlockSize);
	Neighbors.SetNum(FlockSize);
	pSeekBehaviors.reserve(FlockSize);
	NrOfNeighbors = 0;


	for (int index = 0; index < FlockSize; ++index)
	{
		WeightedBehaviors.clear();
		pPriorityBehaviors.clear();
		// Random spawn location
		FVector SpawnPos{
			// FMath::RandRange(0.f, WorldSize),
			// FMath::RandRange(0.f, WorldSize),
			// 90.f
			index * 100.f,
			index * 100.f,
			90.f
		};
	
	
		ASteeringAgent* Agent =
			pWorld->SpawnActor<ASteeringAgent>(
				AgentClass,
				SpawnPos,
				FRotator::ZeroRotator);
		
		// pCohesion = new Cohesion(this);
		// pSeparation = new Separation(this);
		// pAlignment = new Alignment(this);
		// pWander = new Wander();
		// pSeek = new Seek();
		
		Seek* pSeek = new Seek();
		Cohesion* pCohesion = new Cohesion(this);
		Separation* pSeparation = new Separation(this);
		Alignment* pAlignment = new Alignment(this);
		Wander* pWander = new Wander();
		
		pSeekBehaviors.emplace_back(pSeek);
		
		
		WeightedBehaviors.emplace_back(pCohesion, 0.3f);
		WeightedBehaviors.emplace_back(pSeparation, 0.2f);
		WeightedBehaviors.emplace_back(pAlignment, 0.1f);
		WeightedBehaviors.emplace_back(pWander, 0.2f);
		WeightedBehaviors.emplace_back(pSeek, 0.2f);
		
		BlendedSteering* pBlended = new BlendedSteering(WeightedBehaviors);
		
		Evade* pEvade = new Evade();
		pPriorityBehaviors.emplace_back(pEvade); // -> higher in priority list
		pPriorityBehaviors.emplace_back(pBlended);
		pPrioritySteering = new PrioritySteering(pPriorityBehaviors);
		Agent->SetSteeringBehavior(pPrioritySteering);
		
	
		Agents[index] = Agent;
		
		
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
  
  
  
	for (ASteeringAgent* pAgent : Agents)
	{
		// TODO: register the neighbors for this agent (-> fill the memory pool with the neighbors for the currently evaluated agent)
		RegisterNeighbors(pAgent);
		
		// TODO: update the agent (-> the steeringbehaviors use the neighbors in the memory pool)
		pAgent->Tick(DeltaTime);
		
		// TODO: trim the agent to the world
	}
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

  // TODO: implement ImGUI sliders for steering behavior weights here
		//End
		ImGui::End();
	}
#pragma endregion
#endif
}

void Flock::RenderNeighborhood()
{
 // TODO: Debugrender the neighbors for the first agent in the flock
}

#ifndef GAMEAI_USE_SPACE_PARTITIONING
void Flock::RegisterNeighbors(ASteeringAgent* const pAgent)
{
	NrOfNeighbors = 0;
	
	if (pAgent == nullptr)
		return;
	for (ASteeringAgent* NeighborAgent : Agents)
	{
		if (NeighborAgent == pAgent) continue;
		
		float DistanceToNeighbor = (pAgent->GetPosition() - NeighborAgent->GetPosition()).Length();
		if (DistanceToNeighbor <= NeighborhoodRadius)
		{
			// Add to this Agent's neighbors
			Neighbors[NrOfNeighbors] = NeighborAgent;
			++NrOfNeighbors;
			
		}
	}
		
}
#endif

FVector2D Flock::GetAverageNeighborPos() const
{
	FVector2D avgPosition = FVector2D::ZeroVector;
	if (NrOfNeighbors == 0) return avgPosition;
	int ValidCount = 0;

	for (ASteeringAgent* NeighborAgent : Neighbors)
	{
		if (NeighborAgent != nullptr)
		{
			avgPosition += NeighborAgent->GetPosition();
			ValidCount++;
		}
			
	}
	
	return avgPosition / ValidCount;
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	FVector2D avgVelocity = FVector2D::ZeroVector;
	if (NrOfNeighbors == 0) return avgVelocity;
	int ValidCount = 0;
	
	
	for (ASteeringAgent* NeighborAgent : Neighbors)
	{
		if (NeighborAgent != nullptr)
		{
			avgVelocity += NeighborAgent->GetLinearVelocity();
			ValidCount++;
		}
	}

	return avgVelocity / ValidCount;
}

void Flock::SetTarget_Seek(FSteeringParams const& Target)
{
	for (const auto& Seek : pSeekBehaviors)
		Seek->SetTarget(Target);
}

