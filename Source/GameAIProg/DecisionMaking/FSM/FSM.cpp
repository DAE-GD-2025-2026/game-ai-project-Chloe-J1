#include "FSM.h"

#include "AIController.h"
#include "Math/UnitConversion.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"

//STATES
GameAI::FSM::State::State()
{
}

GameAI::FSM::ChaseState::ChaseState()
{
	m_steeringBehavior = std::make_unique<Seek>();
}

void GameAI::FSM::ChaseState::Tick(float DeltaTime, ASteeringAgent& Agent, UBlackboardComponent* Blackboard)
{
	GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Green, TEXT("State: Chase"));
	AActor* ThiefActor = Cast<AActor>(Blackboard->GetValueAsObject("Thief"));
	
	if (ThiefActor == nullptr) return;
	
	FTargetData target;
	target.AngularVelocity = 0;
	target.LinearVelocity = FVector2D(0, 0);
	target.Position = FVector2D(ThiefActor->GetActorLocation());
	
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("Pos: %f %f"), target.Position.X, target.Position.Y));
	// }
	
	m_steeringBehavior->SetTarget(target);	
	m_steeringBehavior->CalculateSteering(DeltaTime, Agent);
	Agent.SetSteeringBehavior(m_steeringBehavior.get());
}

GameAI::FSM::PatrolState::PatrolState(UBlackboardComponent* Blackboard)
{
	m_firstPatrolPoint = FVector2D(Blackboard->GetValueAsVector("FirstPatrolPoint"));
	m_secondPatrolPoint = FVector2D(Blackboard->GetValueAsVector("SecondPatrolPoint"));
	m_currTargetPos = m_firstPatrolPoint;
	
	m_steeringBehavior = std::make_unique<Seek>();
}

void GameAI::FSM::PatrolState::Tick(float DeltaTime, ASteeringAgent& Agent, UBlackboardComponent* Blackboard)
{
	GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Green, TEXT("State: Patrol"));
	if ((FVector2D(Agent.GetActorLocation()) - m_currTargetPos).Length() < 0.01f)
	{
		m_patrolIdx++;
		if (m_patrolIdx > 1)
			m_patrolIdx = 0;
		if (m_patrolIdx == 0)
		{
			m_currTargetPos = m_secondPatrolPoint;
		}
		else
		{
			m_currTargetPos = m_firstPatrolPoint;
		}
		
	}
	
	FTargetData target;
	target.AngularVelocity = 0;
	target.LinearVelocity = FVector2D(0, 0);
	target.Position = FVector2D(m_currTargetPos);
	
	m_steeringBehavior->SetTarget(target);
	Agent.SetSteeringBehavior(m_steeringBehavior.get());
}

GameAI::FSM::SearchState::SearchState(UBlackboardComponent* Blackboard)
{
	m_steeringBehavior = std::make_unique<Wander>();
	
	Blackboard->SetValueAsInt("StartSearchTime", FDateTime::Now().GetSecond());
}

void GameAI::FSM::SearchState::Tick(float DeltaTime, ASteeringAgent& Agent, UBlackboardComponent* Blackboard)
{
	GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Green, TEXT("State: Search"));
	FTargetData target;
	
	m_steeringBehavior->SetTarget(target);	
	m_steeringBehavior->CalculateSteering(DeltaTime, Agent);
	Agent.SetSteeringBehavior(m_steeringBehavior.get());
}

GameAI::FSM::StealState::StealState(UBlackboardComponent* Blackboard)
{
	m_treasureLocation = Blackboard->GetValueAsVector("TreasureLocation");
}

void GameAI::FSM::StealState::Tick(float DeltaTime, ASteeringAgent& Agent, UBlackboardComponent* Blackboard)
{
	AAIController* AIController = Cast<AAIController>(Agent.GetController());
	if (AIController)
	{
		AIController->MoveToLocation(m_treasureLocation);
	}
}

void GameAI::FSM::FleeState::Tick(float DeltaTime, ASteeringAgent& Agent, UBlackboardComponent* Blackboard)
{
	AActor* Guard = Cast<AActor>(Blackboard->GetValueAsObject("Guard"));
	
	AAIController* AIController = Cast<AAIController>(Agent.GetController());
	if (AIController)
	{
		FVector FleeDirection = (Agent.GetActorLocation() - Guard->GetActorLocation()).GetSafeNormal();
		FVector FleeTarget = Agent.GetActorLocation() + FleeDirection * 100.f;

		AIController->MoveToLocation(FleeTarget);
	}
}


// FSM
GameAI::FSM::FSM::FSM()
{
}

void GameAI::FSM::FSM::Tick(float DeltaTime)
{
	if (m_currState == nullptr) return;
	
	// //Check for transitions
	if (State* newState = GetStateTransition())
	 	m_currState = newState;
	
	for (const auto& Agent : m_steeringAgents)
	{
		m_currState->Tick(DeltaTime, *Agent, m_blackboard);
	}
}

void GameAI::FSM::FSM::AddState(std::unique_ptr<State>&& NewState)
{
	if (not m_currState)
		m_currState = NewState.get();
	
	m_states.emplace_back(std::move(NewState));
}

void GameAI::FSM::FSM::AddTransition(State* From, State* To, const std::function<bool()>& Condition)
{
	m_transitions.emplace_back(From, To, Condition);
}

GameAI::FSM::State* GameAI::FSM::FSM::GetStateTransition()
{
	for (const auto& transition : m_transitions)
	{
		if (transition.Condition() == true && transition.From == m_currState)
			return transition.To;
	}
	return nullptr; // No valid transition found
}

void GameAI::FSM::FSM::SetBlackboard(UBlackboardComponent* Blackboard)
{
	m_blackboard = Blackboard;
}

void GameAI::FSM::FSM::AddSteeringAgent(ASteeringAgent* SteeringAgent)
{
	m_steeringAgents.push_back(SteeringAgent);
}

