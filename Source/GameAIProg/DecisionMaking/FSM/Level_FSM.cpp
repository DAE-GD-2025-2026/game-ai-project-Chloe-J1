// Fill out your copyright notice in the Description page of Project Settings.


#include "Level_FSM.h"
#include "FSM.h"
#include "FSMComponent.h"
#include "DecisionMaking/GameAIController.h"


// Sets default values
ALevel_FSM::ALevel_FSM()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_FSM::BeginPlay()
{
	Super::BeginPlay();
	
	// Non-AI agent
	Agent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, 
	FVector{500,100,90}, FRotator::ZeroRotator);
	Agent->SetDebugRenderingEnabled(false);
	m_pSeek = std::make_unique<Seek>();
	Agent->SetSteeringBehavior(m_pSeek.get());
	
	// AI agent
	Agent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, 
	FVector{0,0,90}, FRotator::ZeroRotator);
	Agent->SetDebugRenderingEnabled(false);
	
	
	
	// TODO
	if (AGameAIController* AIController = Cast<AGameAIController>(Agent->GetController()))
	{
		if (UFSMComponent* FSM = Cast<UFSMComponent>(AIController->GetBrainComponent()))
		{
			FSM->AddState(std::make_unique<GameAI::FSM::TestState>());
			FSM->SetSteeringAgent(Agent);
			FSM->GetBlackboard()->SetValueAsObject("Guard", Agent);
			AIController->RunFiniteStateMachine();
		}
	} 
	
	
	
	
}

void ALevel_FSM::SetTarget_Seek()
{
	if (m_pSeek == nullptr) return;
	m_pSeek->SetTarget(MouseTarget);
}

// Called every frame
void ALevel_FSM::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SetTarget_Seek();
}

