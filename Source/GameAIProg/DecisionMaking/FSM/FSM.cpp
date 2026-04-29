#include "FSM.h"

#include "Math/UnitConversion.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"

//STATES
GameAI::FSM::State::State()
{
}

void GameAI::FSM::State::Tick(float DeltaTime, ASteeringAgent& Agent, UBlackboardComponent* Blackboard)
{
	// Update steering behavior
	if (m_steeringBehavior == nullptr) return;
	FTargetData target;
	target.AngularVelocity = 0;
	target.LinearVelocity = FVector2D(0, 0);
	target.Position = FVector2D(Blackboard->GetValueAsVector("TargetLocation"));
	
	m_steeringBehavior->SetTarget(target);
	m_steeringBehavior->CalculateSteering(DeltaTime, Agent);
	Agent.SetSteeringBehavior(m_steeringBehavior.get());
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
	AActor* GuardActor = dynamic_cast<AActor*>(Blackboard->GetValueAsObject("Guard"));
	if (GuardActor == nullptr) return;
	
	FTargetData target;
	target.AngularVelocity = 0;
	target.LinearVelocity = FVector2D(0, 0);
	target.Position = FVector2D(GuardActor->GetActorLocation());
	
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

	//Check for transitions
	if (State* newState = GetStateTransition())
	 	m_currState = newState;
	
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

