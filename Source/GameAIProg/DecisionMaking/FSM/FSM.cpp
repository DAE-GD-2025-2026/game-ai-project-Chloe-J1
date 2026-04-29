#include "FSM.h"

#include "Math/UnitConversion.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"

//STATES
GameAI::FSM::State::State()
{
}

GameAI::FSM::TestState::TestState()
{
	m_steeringBehavior = std::make_unique<Seek>();
}

GameAI::FSM::ChaseState::ChaseState()
{
	m_steeringBehavior = std::make_unique<Seek>();
}

void GameAI::FSM::ChaseState::Tick(float DeltaTime, ASteeringAgent& Agent, UBlackboardComponent* Blackboard)
{
	AActor* GuardActor = Cast<AActor>(Blackboard->GetValueAsObject("Thief"));
	
	if (GuardActor == nullptr) return;
	
	FTargetData target;
	target.AngularVelocity = 0;
	target.LinearVelocity = FVector2D(0, 0);
	target.Position = FVector2D(GuardActor->GetActorLocation());
	
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("Pos: %f %f"), target.Position.X, target.Position.Y));
	// }
	
	m_steeringBehavior->SetTarget(target);	
	m_steeringBehavior->CalculateSteering(DeltaTime, Agent);
	Agent.SetSteeringBehavior(m_steeringBehavior.get());
}


// FSM
GameAI::FSM::FSM::FSM()
{
}

void GameAI::FSM::FSM::Tick(float DeltaTime)
{
	if (m_currState == nullptr) return;
	
	// //Check for transitions
	// if (State* newState = GetStateTransition())
	//  	m_currState = newState;
	
	m_currState->Tick(DeltaTime, *m_steeringAgent, m_blackboard);

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
		if (transition.Condition() == true)
			return transition.To;
	}
	return nullptr; // No valid transition found
}

void GameAI::FSM::FSM::SetBlackboard(UBlackboardComponent* Blackboard)
{
	m_blackboard = Blackboard;
}

void GameAI::FSM::FSM::SetSteeringAgent(ASteeringAgent* SteeringAgent)
{
	m_steeringAgent = SteeringAgent;
}

void GameAI::FSM::FSM::InitBlackboardValues()
{
	m_blackboard->SetValueAsVector("TargetLocation", FVector(1000.f, 200.f, 0.f)); // TEST TARGET
}

