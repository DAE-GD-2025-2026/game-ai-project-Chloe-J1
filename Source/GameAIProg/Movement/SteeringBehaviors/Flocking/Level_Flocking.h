// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Flock.h"
#include "Shared/Level_Base.h"
#include "GameAIProg/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "Level_Flocking.generated.h"


UCLASS()
class GAMEAIPROG_API ALevel_Flocking : public ALevel_Base
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevel_Flocking();

	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	enum class BehaviorTypes
	{
		Seek,
		Wander,
		Flee,
		Arrive,
		Evade,
		Pursuit,
		Face,
		Blended,
		Priority,
		// @ End
		Count
	};

	struct ImGui_Agent final
	{
		ASteeringAgent* Agent{nullptr};
		std::unique_ptr<ISteeringBehavior> Behavior{nullptr};
		int SelectedBehavior{static_cast<int>(BehaviorTypes::Seek)};
		int SelectedTarget = -1;
	};
	std::vector<ImGui_Agent> SteeringAgents{};
	std::vector<std::string> TargetLabels{};
	int AgentIndexToRemove = -1;
	
	bool AddAgent(BehaviorTypes BehaviorType = BehaviorTypes::Seek, bool AutoOrient = true);
	void RemoveAgent(unsigned int Index);
	void SetAgentBehavior(ImGui_Agent& Agent);
	
	void RefreshTargetLabels();
	void UpdateTarget(ImGui_Agent& Agent);
	void RefreshAgentTargets(unsigned int IndexRemoved);

	bool bUseMouseTarget{true};

	int const FlockSize{3};

	TUniquePtr<Flock> pFlock{};
	
	UPROPERTY(EditAnywhere, Category = "Flocking")
	ASteeringAgent* pAgentToEvade{nullptr}; // non owning ref
	
	
};
