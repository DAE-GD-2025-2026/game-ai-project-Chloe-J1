#include "Level_BehaviorTree.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "DecisionMaking/GameAIController.h"
#include "DecisionMaking/FSM/FSMComponent.h"

ALevel_BehaviorTree::ALevel_BehaviorTree()
{
}

void ALevel_BehaviorTree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALevel_BehaviorTree::BeginPlay()
{
	Super::BeginPlay();
	
	Thief = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, 
	FVector{0,0,90}, FRotator::ZeroRotator);
	Thief->SetDebugRenderingEnabled(false);
	
	Guard = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, 
	FVector{200,50,90}, FRotator::ZeroRotator);
	Guard->SetDebugRenderingEnabled(false);
	
	
	// GUARD
	
	if (AGameAIController* AIController = Cast<AGameAIController>(Guard->GetController()))
	{
		AIController->RunBT();
    
		UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
		Blackboard->SetValueAsObject("Thief", Thief);
		Blackboard->SetValueAsVector("TargetLocation", Thief->GetActorLocation());
		Blackboard->SetValueAsVector("FirstPatrolPoint", FVector{130,-400,0});
		Blackboard->SetValueAsVector("SecondPatrolPoint", FVector{130,700,0});
	} 
	
	// THIEF
	
	if (AGameAIController* AIController = Cast<AGameAIController>(Thief->GetController()))
	{
		if (UBehaviorTreeComponent* BehaviorTree = Cast<UBehaviorTreeComponent>(AIController->GetBrainComponent()))
		{
			UBlackboardComponent* BlackBoard = AIController->GetBlackboardComponent();
		}
	} 
}
