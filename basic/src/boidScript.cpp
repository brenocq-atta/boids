//--------------------------------------------------
// Boids Basic
// boidScript.cpp
// Date: 2022-01-29
// By Breno Cunha Queiroz
//--------------------------------------------------
#include "boidScript.h"
#include "boidComponent.h"
#include "settingsComponent.h"
#include <atta/componentSystem/components/transformComponent.h>
using namespace atta;

void BoidScript::update(Entity entity, float dt)
{
    SettingsComponent* s = ComponentManager::getEntityComponent<SettingsComponent>(0);
    BoidComponent* b = entity.getComponent<BoidComponent>();
    TransformComponent* t = entity.getComponent<TransformComponent>();

    vec2 force {};
    force += collisionAvoidance(entity) * s->collisionAvoidanceFactor;
    force += velocityMatching(entity) * s->velocityMatchingFactor;
    force += flockCentering(entity) * s->flockCenteringFactor;

    if(entity.getCloneId() == 0)
        LOG_DEBUG("Boid 0", "My force vector is: $0", force);

    b->acceleration = force;
}

atta::vec2 BoidScript::collisionAvoidance(Entity entity)
{
    SettingsComponent* s = ComponentManager::getEntityComponent<SettingsComponent>(0);
    BoidComponent* b = entity.getComponent<BoidComponent>();
    TransformComponent* t = entity.getComponent<TransformComponent>();

    //----- Collision Avoidance -----//
    vec2 avoidanceVector = vec2(0.0f);
    for(EntityId neighbor : b->neighbors)
    {
        TransformComponent* to = ComponentManager::getEntityComponent<TransformComponent>(neighbor);

        vec2 avoidVec = -vec2(to->position-t->position);
        if(avoidVec == vec2(0.0f)) continue;// Ignore if they are overlapping

        vec2 dir = normalize(avoidVec);
        float dist = std::max(avoidVec.length(), 0.00001f);
        avoidanceVector += dir*s->viewExpoent/(dist*dist);
    }
    if(b->neighbors.size())
        avoidanceVector /= b->neighbors.size();

    return avoidanceVector;
}

atta::vec2 BoidScript::velocityMatching(Entity entity)
{
    BoidComponent* b = entity.getComponent<BoidComponent>();

    //----- Collision Avoidance -----//
    vec2 velVector = b->velocity;
    for(EntityId neighbor : b->neighbors)
    {
        BoidComponent* bo = ComponentManager::getEntityComponent<BoidComponent>(neighbor);
        velVector += bo->velocity;
    }
    if(b->neighbors.size())
        velVector /= (b->neighbors.size() + 1);
    
    return velVector;
}

atta::vec2 BoidScript::flockCentering(Entity entity)
{
    BoidComponent* b = entity.getComponent<BoidComponent>();
    TransformComponent* t = entity.getComponent<TransformComponent>();

    //----- Collision Avoidance -----//
    vec2 centerVector = vec2(0.0f);
    for(EntityId neighbor : b->neighbors)
    {
        TransformComponent* to = ComponentManager::getEntityComponent<TransformComponent>(neighbor);
        centerVector += vec2(to->position - t->position);
    }
    if(b->neighbors.size())
        centerVector /= b->neighbors.size();
    
    return centerVector;
}
