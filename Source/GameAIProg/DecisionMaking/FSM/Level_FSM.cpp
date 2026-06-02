// Fill out your copyright notice in the Description page of Project Settings.


#include "Level_FSM.h"
#include "FSM.h"
#include "FSMComponent.h"
#include "DecisionMaking/GameAIController.h"
#include "Math/UnitConversion.h"


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
	
	// THIEF
	Thief = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, 
	FVector{500,100,90}, FRotator::ZeroRotator);
	Thief->SetDebugRenderingEnabled(false);
	m_pSeek = std::make_unique<Seek>();
	Thief->SetSteeringBehavior(m_pSeek.get());
	// GUARD
	Agent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, 
	FVector{0,0,90}, FRotator::ZeroRotator);
	Agent->SetDebugRenderingEnabled(false);
	
	if (AGameAIController* AIController = Cast<AGameAIController>(Agent->GetController()))
	{
		if (UFSMComponent* FSM = Cast<UFSMComponent>(AIController->GetBrainComponent()))
		{
			UBlackboardComponent* BlackBoard = FSM->GetBlackboard();
			BlackBoard->SetValueAsVector("FirstPatrolPoint", FVector{130,-400,0});
			BlackBoard->SetValueAsVector("SecondPatrolPoint", FVector{130,700,0});
			BlackBoard->SetValueAsObject("Thief", Thief);
			BlackBoard->SetValueAsInt("StartSearchTime", FDateTime::Now().GetSecond()); // Initial value, will be overwritten by Search ctor
			
			std::function<bool()> isTargetVisible = [&]()
			{
            	const float distance = 200.f;
            	if (((Agent->GetActorLocation() - Thief->GetActorLocation()).GetAbs()).Length() < distance)
            	{
            		if (GEngine)
            		{
            			GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Blue, FString::Printf(TEXT("Target visible")));
            		}
            		return true;
            	}
            	return false;
			};
			std::function<bool()> notIsTargetVisible = [&]()
			{
				const float distance = 200.f;
				if (((Agent->GetActorLocation() - Thief->GetActorLocation()).GetAbs()).Length() < distance)
				{
					return false;
				}
				return true;
			};
			
			std::function<bool()> isSearchingTooLong = [BlackBoard]()
			{
				const int32 maxSearchTime = 15;
				FDateTime date = FDateTime::Now();
				int32 timeNow = date.GetSecond();
				int32 startTime =  BlackBoard->GetValueAsInt("StartSearchTime");
				
				
				if ((timeNow - startTime) < maxSearchTime)
				{
					return false;
				}

				return true;
			};
			
			std::unique_ptr<GameAI::FSM::ChaseState> chaseState = std::make_unique<GameAI::FSM::ChaseState>();
			std::unique_ptr<GameAI::FSM::PatrolState> patrolState = std::make_unique<GameAI::FSM::PatrolState>(FSM->GetBlackboard());
			std::unique_ptr<GameAI::FSM::SearchState> searchState = std::make_unique<GameAI::FSM::SearchState>(FSM->GetBlackboard());
			FSM->AddTransition(patrolState.get(), chaseState.get(), isTargetVisible);
			FSM->AddTransition(chaseState.get(), searchState.get(), notIsTargetVisible);
			FSM->AddTransition(searchState.get(), chaseState.get(), isTargetVisible);
			FSM->AddTransition(searchState.get(), patrolState.get(), isSearchingTooLong);
			
			FSM->AddState(std::move(patrolState));
			FSM->AddState(std::move(chaseState));
			FSM->AddState(std::move(searchState));
			FSM->AddSteeringAgent(Agent);	
			AIController->RunFiniteStateMachine();
			
			
		}
	} 
	
	Thief->SetMaxLinearSpeed(900.f);
	if (AGameAIController* AIController = Cast<AGameAIController>(Thief->GetController()))
	{
		if (UFSMComponent* FSM = Cast<UFSMComponent>(AIController->GetBrainComponent()))
		{
			std::unique_ptr<GameAI::FSM::PatrolState> patrolState = std::make_unique<GameAI::FSM::PatrolState>(FSM->GetBlackboard());
			
			UBlackboardComponent* BlackBoard = FSM->GetBlackboard();
			FSM->AddSteeringAgent(Thief);	
			AIController->RunFiniteStateMachine();
		}
	}
}

void ALevel_FSM::SetTarget_Seek()
{
	if (m_pSeek == nullptr) return;
	m_pSeek->SetTarget(MouseTarget);
}

bool ALevel_FSM::IsTargetVisible()
{
	return true;
}

// Called every frame
void ALevel_FSM::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SetTarget_Seek();
}

