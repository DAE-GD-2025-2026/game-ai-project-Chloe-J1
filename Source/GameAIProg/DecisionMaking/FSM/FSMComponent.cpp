// Fill out your copyright notice in the Description page of Project Settings.


#include "FSMComponent.h"

#include "BehaviorTree/BlackboardComponent.h"


// Sets default values for this component's properties
UFSMComponent::UFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// TODO Setup FSM
	FSMInstance = std::make_unique<GameAI::FSM::FSM>();
}


void UFSMComponent::AddState(std::unique_ptr<GameAI::FSM::State>&& NewState)
{
	FSMInstance->AddState(std::move(NewState));
}

void UFSMComponent::AddTransition(GameAI::FSM::State* From, GameAI::FSM::State* To, std::function<bool()> EvalFunc) const
{
	// TODO
	FSMInstance->AddTransition(From, To, EvalFunc);
}

void UFSMComponent::SetBlackboard(UBlackboardComponent* Blackboard)
{
	m_blackboard = Blackboard;
	FSMInstance->SetBlackboard(m_blackboard);
	FSMInstance->InitBlackboardValues();
}

UBlackboardComponent* UFSMComponent::GetBlackboard() const
{
	return m_blackboard;
}

void UFSMComponent::SetSteeringAgent(ASteeringAgent* SteeringAgent)
{
	FSMInstance->SetSteeringAgent(SteeringAgent);
}

// Called when the game starts
void UFSMComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// TODO
	FSMInstance->Tick(DeltaTime);
}

void UFSMComponent::StartLogic()
{
	Super::StartLogic();

	// TODO
	bIsRunning = true;
}

void UFSMComponent::StopLogic(const FString& Reason)
{
	// TODO
	bIsRunning = false;
}

bool UFSMComponent::IsRunning() const
{
	return bIsRunning;
}

