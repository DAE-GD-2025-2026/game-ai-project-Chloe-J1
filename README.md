# Game AI Project
**********************
STEERING BEHAVIORS
**********************
In this project I made different steering behaviors to simulate the complex movement of boids walking around autonomously.

Agents have a steering behavior and move according to their steering output.

┌────────────────────┐                                     
│                    │                                     
│      ABaseAgent    │                                     
│                    │                                     
└─────────▲──────────┘                                     
          │                                                
          │inherits                                        
┌─────────┼──────────┐            ┌───────────────────────┐
│                    │   uses     │                       │
│   ASteeringAgent   ├────────────►   ISteeringBehavior   │
│                    │            │                       │
└────────────────────┘            └─┬─────────────────────┘
                                    │                      
                                    │ inherits   ┌────────┐
                                    │            │        │
                                    ├───────────►│  Seek  │
                                    │            │        │
                                    │            └────────┘
                                    │                      
                                    │            ┌────────┐
                                    │            │        │
                                    ├───────────►│  Flee  │
                                    │            │        │
                                    │            └────────┘
                                    │                      
                                    │            ┌────────┐
                                    │            │        │
                                    └───────────►│  ....  │
                                                 │        │
                                                 └────────┘

Spatial partitioning can be enabled to optimize the neighborhood analysis.

**********************
Pathfinding
**********************
The pathfinding level uses A* to calculate an optimized path towards the end point, taking obstacles into account.
You can place mud or water on the floor by pressing 2 / 3 on the keyboard and erase it by pressing 1.

The navmesh level implements navmesh-based pathfinding combined with the Simple Stupid Funnel Algorithm (SSFA) to generate smooth, optimized paths for an agent.