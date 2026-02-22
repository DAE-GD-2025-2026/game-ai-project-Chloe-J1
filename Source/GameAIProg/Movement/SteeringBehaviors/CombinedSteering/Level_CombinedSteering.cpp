#include "Level_CombinedSteering.h"
#include <format>
#include "imgui.h"


// Sets default values
ALevel_CombinedSteering::ALevel_CombinedSteering()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_CombinedSteering::BeginPlay()
{
	Super::BeginPlay();
	
	// Combined steering agent behaviors
	pSeek   = std::make_unique<Seek>();
	pWander = std::make_unique<Wander>();
	
	CombinedBehaviors.emplace_back(pSeek.get(),   0.5f);
	CombinedBehaviors.emplace_back(pWander.get(), 0.5f);
	
	// Priority steering agent behaviors
	Evade* pEvade = new Evade();
	Wander* pWander1 = new Wander();
	
	PriorityBehaviors.emplace_back(pEvade); // higher in priority list
	PriorityBehaviors.emplace_back(pWander1);
	
	// Agents
	AddAgent(BehaviorTypes::Blended);
	SteeringAgents[0].Agent->SetDebugRenderingEnabled(true);
	
	AddAgent(BehaviorTypes::Priority);
	
	 
}

void ALevel_CombinedSteering::BeginDestroy()
{
	Super::BeginDestroy();

}

bool ALevel_CombinedSteering::AddAgent(BehaviorTypes BehaviorType, bool AutoOrient)
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

void ALevel_CombinedSteering::RemoveAgent(unsigned int Index)
{
	SteeringAgents[Index].Agent->Destroy();
	SteeringAgents.erase(SteeringAgents.begin() + Index);

	RefreshTargetLabels();
	RefreshAgentTargets(Index);
}

void ALevel_CombinedSteering::SetAgentBehavior(ImGui_Agent& Agent)
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
	case BehaviorTypes::Blended:
		Agent.Behavior = std::make_unique<BlendedSteering>(CombinedBehaviors);
		Agent.Behavior->SetTarget(MouseTarget);
		pBlendedSteering = static_cast<BlendedSteering*>(Agent.Behavior.get());  
		break;
	case BehaviorTypes::Priority:
		Agent.Behavior = std::make_unique<PrioritySteering>(PriorityBehaviors);
		break;
	default:
		assert(false); // Incorrect Agent Behavior gotten during SetAgentBehavior()	
	} 

	UpdateTarget(Agent);
	
	Agent.Agent->SetSteeringBehavior(Agent.Behavior.get());
}

void ALevel_CombinedSteering::RefreshTargetLabels()
{
	TargetLabels.clear();
	
	TargetLabels.push_back("Mouse");
	for (int i{0}; i < SteeringAgents.size(); ++i)
	{
		TargetLabels.push_back(std::format("Agent {}", i));
	}
}

void ALevel_CombinedSteering::UpdateTarget(ImGui_Agent& Agent)
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

void ALevel_CombinedSteering::RefreshAgentTargets(unsigned int IndexRemoved)
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
void ALevel_CombinedSteering::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
	
#pragma region UI
	//UI
	{
		//Setup
		bool windowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Game AI", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	
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
		ImGui::Spacing();
	
		ImGui::Text("Flocking");
		ImGui::Spacing();
		ImGui::Spacing();
	
		if (ImGui::Checkbox("Debug Rendering", &CanDebugRender))
		{
   // TODO: Handle the debug rendering of your agents here :)
		}
		ImGui::Checkbox("Trim World", &TrimWorld->bShouldTrimWorld);
		if (TrimWorld->bShouldTrimWorld)
		{
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Trim Size",
				TrimWorld->GetTrimWorldSize(), 1000.f, 3000.f,
				[this](float InVal) { TrimWorld->SetTrimWorldSize(InVal); });
		}
		
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
	
		ImGui::Text("Behavior Weights");
		ImGui::Spacing();
		
		if (pBlendedSteering != nullptr)
		{
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Seek",
			pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight, 0.f, 1.f,
			[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight = InVal; }, "%.2f");
		
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Wander",
			pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight, 0.f, 1.f,
			[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight = InVal; }, "%.2f");
		}
		
		
		//End
		ImGui::End();
	}
#pragma endregion
	
	// Update agents
	for (ImGui_Agent& a : SteeringAgents)
	{
		if (a.Agent)
		{
			UpdateTarget(a);
		}
	}
	
	// Combined Steering Update
 // TODO: implement handling mouse click input for seek
 // TODO: implement Make sure to also evade the wanderer
}
