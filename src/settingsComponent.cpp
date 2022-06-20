//--------------------------------------------------
// Boids Basic
// settingsComponent.h
// Date: 2022-01-29
// By Breno Cunha Queiroz  
//--------------------------------------------------
#include "settingsComponent.h"

template<>
ComponentDescription& TypedComponentRegistry<SettingsComponent>::getDescription()
{
    static ComponentDescription desc =
        {
            "Settings",
            {
                { AttributeType::FLOAT32, offsetof(SettingsComponent, viewRadius), "viewRadius" },
                { AttributeType::FLOAT32, offsetof(SettingsComponent, collisionAvoidanceFactor), "collisionAvoidanceFactor" },
                { AttributeType::FLOAT32, offsetof(SettingsComponent, velocityMatchingFactor), "velocityMatchingFactor" },
                { AttributeType::FLOAT32, offsetof(SettingsComponent, flockCenteringFactor), "flockCenteringFactor" },
                { AttributeType::FLOAT32, offsetof(SettingsComponent, noise), "noise" }
            },
            // Max instances
            1
        };

    return desc;
}
