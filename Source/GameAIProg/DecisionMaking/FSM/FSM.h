#pragma once
#include <memory>
#include <vector>
#include <functional>

#include "FSM.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "Movement/SteeringBehaviors/CombinedSteering/CombinedSteeringBehaviors.h"


namespace GameAI::FSM
{
	
	
	// STATES
	class State
	{
	public:
		State();
		virtual ~State() = default;
		
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
	
	class PatrolState : public State
	{
	public:
		PatrolState(UBlackboardComponent* Blackboard);
		
		virtual void Tick(float DeltaTime, ASteeringAgent& Agent, UBlackboardComponent* Blackboard) override;
	private:
		FVector2D m_firstPatrolPoint;
		FVector2D m_secondPatrolPoint;
		FVector2D m_currTargetPos;
		int m_patrolIdx{0};
		const int m_maxPatrolPoints{2};
	};
	
	class SearchState : public State
	{
	public:
		SearchState(UBlackboardComponent* Blackboard);
		
		virtual void Tick(float DeltaTime, ASteeringAgent& Agent, UBlackboardComponent* Blackboard) override;
		
	private:
		bool m_lastSeenReached{false};
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
		void AddSteeringAgent(ASteeringAgent* SteeringAgent);
	private:
		State* m_currState{nullptr};
		std::vector<std::unique_ptr<State>> m_states;
		UBlackboardComponent* m_blackboard; // non-owning reference
		std::vector<ASteeringAgent*> m_steeringAgents;
		std::vector<Transition> m_transitions;
	};
}

