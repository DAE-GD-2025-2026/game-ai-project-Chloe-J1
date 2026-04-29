#pragma once
#include <memory>
#include <vector>
#include <functional>

#include "FSM.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"


namespace GameAI::FSM
{
	
	
	// STATES
	class State
	{
	public:
		State();
		
		virtual void Tick(float DeltaTime, ASteeringAgent& Agent, UBlackboardComponent* Blackboard) = 0;
	protected:
		std::unique_ptr<ISteeringBehavior> m_steeringBehavior;
	};
	
	class TestState : public State
	{
	public:
		TestState();
	};
	
	class ChaseState : public State
	{
	public:
		ChaseState();
		
		virtual void Tick(float DeltaTime, ASteeringAgent& Agent, UBlackboardComponent* Blackboard) override;
	};
	
	// TRANSITION
	struct Transition
	{
		State* From;
		State* To;
		std::function<bool()> Condition;
	};
	
	// FSM
	class FSM
	{
	public:
		FSM();
		void Tick(float DeltaTime);
		
		void AddState(std::unique_ptr<State>&& NewState);
		void AddTransition(State* From, State* To,const std::function<bool()>& Condition);
		State* GetStateTransition();
		void SetBlackboard(UBlackboardComponent* Blackboard);
		void SetSteeringAgent(ASteeringAgent* SteeringAgent);
		void InitBlackboardValues();
	private:
		State* m_currState{nullptr};
		std::vector<std::unique_ptr<State>> m_states;
		UBlackboardComponent* m_blackboard; // non-owning reference
		ASteeringAgent* m_steeringAgent;
		std::vector<Transition> m_transitions;
	};
}

