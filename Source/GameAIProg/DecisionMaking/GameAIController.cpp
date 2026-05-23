// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "FSM/FSMComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"


// Sets default values
AGameAIController::AGameAIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BrainComponent = CreateDefaultSubobject<UFSMComponent>(TEXT("FSMComponent"));
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

// Called when the game starts or when spawned
void AGameAIController::BeginPlay()
{
	Super::BeginPlay();
	
	// Create Blackboard if need be
	InitFiniteStateMachine();
	
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AGameAIController::PerceptionUpdated);
}

// Called every frame
void AGameAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGameAIController::InitFiniteStateMachine()
{
	UFSMComponent* FSMComp = FindComponentByClass<UFSMComponent>();
	if (ensure(FSMComp) && FSMBlackboardAsset)
	{
		UBlackboardComponent* BlackboardComp = Blackboard; // Blackboard member of AAIController
		UseBlackboard(FSMBlackboardAsset, BlackboardComp);
		Blackboard = BlackboardComp;
		
		FSMComp->SetBlackboard(BlackboardComp);
	}
}

void AGameAIController::RunFiniteStateMachine()
{
	UFSMComponent* FSMComp = FindComponentByClass<UFSMComponent>();
	if (ensure(FSMComp))
	{
		FSMComp->StartLogic();
	}
}

void AGameAIController::RunBT()
{
	RunBehaviorTree(BehaviorTreeAsset);
}

// https://lostferry.com/2-50-using-ai-perception-component-unreal-c/
void AGameAIController::PerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		Blackboard->SetValueAsBool("IsThiefVisible", true);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("Search is over")));
		}
	}
	else
	{
		Blackboard->SetValueAsBool("IsThiefVisible", false);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("PATROL")));
		}
	}
}



