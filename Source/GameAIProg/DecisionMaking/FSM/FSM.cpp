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

void GameAI::FSM::State::AddTransition(State* to,const std::function<bool()>& Condition)
{
	m_transitions.push_back(Transition{this, to, Condition});
}

GameAI::FSM::State* GameAI::FSM::State::GetStateTransition()
{
	for (const auto& transition : m_transitions)
	{
		if (transition.Condition() == true)
			return transition.To;
	}
	return nullptr; // No valid transition found
}

GameAI::FSM::TestState::TestState()
{
	m_steeringBehavior = std::make_unique<Seek>();
}


// FSM
GameAI::FSM::FSM::FSM()
{
}

void GameAI::FSM::FSM::Tick(float DeltaTime)
{
	if (m_currState == nullptr) return;

	// Check for transitions
	if (State* newState = m_currState->GetStateTransition())
		m_currState = newState;
	
	m_currState->Tick(DeltaTime, *m_steeringAgent, m_blackboard);

}

void GameAI::FSM::FSM::AddState(std::unique_ptr<State>&& NewState)
{
	if (not m_currState)
		m_currState = NewState.get();
	
	m_states.emplace_back(std::move(NewState));
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

