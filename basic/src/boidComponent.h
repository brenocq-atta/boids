//--------------------------------------------------
// Boids Basic
// boidComponent.h    
// Date: 2022-01-29
// By Breno Cunha Queiroz  
//--------------------------------------------------
#ifndef BOID_COMPONENT_H
#define BOID_COMPONENT_H
#include <atta/pch.h>
#include <atta/componentSystem/componentManager.h>
using namespace atta;

struct BoidComponent final : public Component
{  
    /// Boid velocity
    /** This velocity will remain the same at during boidScript computations and can be accessed by other boids. 
     * This is updated at the end of each step by the projectScript
    **/
    vec2 velocity;
    /// Boid acceleration
    /** The acceleration is calculated by the boidScript, the projectScript updates all velocities at the end of each step **/
    vec2 acceleration;
    /// Neighbors
    /** Updated by projectScript before all boidScripts **/
    std::vector<EntityId> neighbors;
}; 
ATTA_REGISTER_COMPONENT(BoidComponent);
template<> ComponentDescription& TypedComponentRegistry<BoidComponent>::getDescription();

#endif// BOID_COMPONENT_H
