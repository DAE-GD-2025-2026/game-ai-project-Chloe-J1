# Game AI Project
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