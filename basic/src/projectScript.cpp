//--------------------------------------------------
// Boid Basic
// projectScript.cpp
// Date: 2022-01-29
// By Breno Cunha Queiroz
//--------------------------------------------------
#include <atta/componentSystem/componentManager.h>
#include <atta/componentSystem/components/transformComponent.h>
#include <atta/componentSystem/components/materialComponent.h>
#include <atta/graphicsSystem/drawer.h>
#include <imgui.h>

#include "projectScript.h"
#include "boidComponent.h"
#include "settingsComponent.h"

#define PROT_BOID_EID 2// Prototype boid entity id
#define PROT_OBST_EID 4// Prototype obstacle entity id

Project::Project():
    _running(false)
{

}

void Project::onStart()
{
    _running = true;

    srand(31415);
    atta::Factory* factory = atta::ComponentManager::getPrototypeFactory(PROT_BOID_EID);
    for(atta::EntityId boid : factory->getCloneIds())
    {
        atta::TransformComponent* t = atta::ComponentManager::getEntityComponent<atta::TransformComponent>(boid);

        // Initialize each boid position
        t->position.x = ((rand()%5000)/500.0f - 1.0f);
        t->position.y = ((rand()%5000)/500.0f - 1.0f);
    }
}

void Project::onStop()
{
    _running = false;
}

void Project::onUpdateBefore(float)
{
    SettingsComponent* s = atta::ComponentManager::getEntityComponent<SettingsComponent>(0);

    // Update neighbors
    atta::Factory* factory = atta::ComponentManager::getPrototypeFactory(PROT_BOID_EID);
    for(atta::EntityId boid : factory->getCloneIds())
    {
        atta::TransformComponent* t = atta::ComponentManager::getEntityComponent<atta::TransformComponent>(boid);
        BoidComponent* boidInfo = atta::ComponentManager::getEntityComponent<BoidComponent>(boid);
        boidInfo->neighbors.clear();

        for(atta::EntityId other : factory->getCloneIds())
        {
            if(other == boid) continue;

            // Test if other is a neighbor
            atta::TransformComponent* ot = atta::ComponentManager::getEntityComponent<atta::TransformComponent>(other);
            if((vec2(ot->position)-vec2(t->position)).length() <= s->viewRadius)
                boidInfo->neighbors.push_back(other);
        }
    }
}

void Project::onUpdateAfter(float dt)
{
    // Update positions, velocities and accelerations
    atta::Factory* factory = atta::ComponentManager::getPrototypeFactory(PROT_BOID_EID);
    for(atta::EntityId boid : factory->getCloneIds())
    {
        atta::TransformComponent* t = atta::ComponentManager::getEntityComponent<atta::TransformComponent>(boid);
        BoidComponent* boidInfo = atta::ComponentManager::getEntityComponent<BoidComponent>(boid);

        // Update velocity
        boidInfo->velocity += boidInfo->acceleration;
        boidInfo->acceleration = vec2(0.0f);

        // Limit maximum velocity
        float maxVelocity = 0.001f;
        if(boidInfo->velocity.length() > maxVelocity)
            boidInfo->velocity = normalize(boidInfo->velocity)*maxVelocity;

        // Apply velocity to boid
        t->position += vec3(boidInfo->velocity, 0.0f);
        if(boidInfo->velocity.length() > 0)
            t->orientation.rotationFromVectors( 
                    normalize(vec3(boidInfo->velocity.x, boidInfo->velocity.y, 0.0f)), vec3(0, -1, 0));
    }
}

void Project::onAttaLoop()
{

}

void Project::onUIRender()
{
    ImGui::Begin("Configure");
    
    ImGui::End();
}
