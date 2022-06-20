//--------------------------------------------------
// Boids Basic
// settingsComponent.h    
// Date: 2022-01-29
// By Breno Cunha Queiroz  
//--------------------------------------------------
#ifndef SETTINGS_COMPONENT_H
#define SETTINGS_COMPONENT_H
#include <atta/pch.h>
#include <atta/componentSystem/componentManager.h>
using namespace atta;

struct SettingsComponent final : public Component
{  
    /// Boid view radius
    /** Maximum radius to view neighbors **/
    float viewRadius;

    /// Collision avoidance factor
    float collisionAvoidanceFactor;

    /// Velocity matching factor
    float velocityMatchingFactor;

    /// Flock centering factor
    float flockCenteringFactor;

    /// Measurements noise standand deviation
    float noise;
}; 
ATTA_REGISTER_COMPONENT(SettingsComponent);
template<> ComponentDescription& TypedComponentRegistry<SettingsComponent>::getDescription();

#endif// SETTINGS_COMPONENT_H
