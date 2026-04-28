#pragma once
#include <memory>
#include <vector>
#include <functional>

#include "BehaviorTree/BlackboardComponent.h"
#include "Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"


namespace GameAI::FSM
{
	class State;
	// TRANSITION
	struct Transition
	{
		State* From;
		State* To;
		std::function<bool()> Condition;
	};
	
	// STATES
	class State
	{
	public:
		State();
		
		void Tick(float DeltaTime, ASteeringAgent& Agent, UBlackboardComponent* Blackboard);
		void AddTransition(State* to,const std::function<bool()>& Condition);
		State* GetStateTransition();
	protected:
		std::unique_ptr<ISteeringBehavior> m_steeringBehavior;
		std::vector<Transition> m_transitions;
	};
	
	 class TestState : public State
	{
	public:
		TestState();
	};
	
	
	// FSM
	class FSM
	{
	public:
		FSM();
		void Tick(float DeltaTime);
		
		void AddState(std::unique_ptr<State>&& NewState);
		void SetBlackboard(UBlackboardComponent* Blackboard);
		void SetSteeringAgent(ASteeringAgent* SteeringAgent);
		void InitBlackboardValues();
	private:
		State* m_currState{nullptr};
		std::vector<std::unique_ptr<State>> m_states;
		UBlackboardComponent* m_blackboard; // non-owning reference
		ASteeringAgent* m_steeringAgent;
	};
}

